#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <jpeglib.h>
#include <stdint.h>

#include "gui_guider.h"
#include "lvgl.h"

#include "control_device.h"

// 摄像头和控件参数
#define VIDEO_DEVICE "/dev/video4"
#define WIDTH 320
#define HEIGHT 240
#define FPS 30

// 全局资源
static int v4l2_fd = -1;
static void **buffers = NULL;
static size_t *buffer_lengths = NULL;
static int buffer_count = 0;
static struct jpeg_decompress_struct cinfo;
static lv_obj_t *cam_image = NULL;
static lv_timer_t *cam_timer = NULL;
static uint8_t *image_buf = NULL;
static lv_image_dsc_t img_dsc;

static struct jpeg_error_mgr jerr;
// JPEG 错误处理
static void jpeg_error_exit(j_common_ptr cinfo) {
    char jpeg_last_error[1024];
    (*cinfo->err->format_message)(cinfo, jpeg_last_error);
    printf("JPEG error: %s\n", jpeg_last_error);
}

static void cam_timer_cb(lv_timer_t *timer) {
    if (!cam_image || !image_buf) {
        printf("cam_timer_cb: cam_image or image_buf is NULL\n");
        return;
    }

    struct v4l2_buffer buf = { .type = V4L2_BUF_TYPE_VIDEO_CAPTURE, .memory = V4L2_MEMORY_MMAP };
    if (ioctl(v4l2_fd, VIDIOC_DQBUF, &buf) < 0) {
        printf("cam_timer_cb: Failed to dequeue buffer\n");
        return;
    }

    if (buf.index >= buffer_count || buf.bytesused == 0) {
        printf("cam_timer_cb: Invalid buffer index %u or bytesused %u\n", buf.index, buf.bytesused);
        goto cleanup;
    }

    // 保存 MJPG 数据以调试
    static int frame_count = 0;
    if (frame_count < 5) {
        char filename[32];
        snprintf(filename, sizeof(filename), "/root/mjpg_%d.mjpg", frame_count);
        FILE *fp = fopen(filename, "wb");
        if (fp) {
            if (buf.bytesused > buffer_lengths[buf.index]) {
                printf("Frame %d truncated: %u > %zu\n", frame_count, buf.bytesused, buffer_lengths[buf.index]);
                buf.bytesused = buffer_lengths[buf.index];
            }
            fwrite(buffers[buf.index], 1, buf.bytesused, fp);
            fclose(fp);
        }
        frame_count++;
    }

    // 初始化 JPEG 解码
    jpeg_mem_src(&cinfo, buffers[buf.index], buf.bytesused);
    if (jpeg_read_header(&cinfo, TRUE) != JPEG_HEADER_OK) {
        printf("cam_timer_cb: Failed to read JPEG header\n");
        jpeg_abort_decompress(&cinfo);
        goto cleanup;
    }

    cinfo.out_color_space = JCS_RGB;
    if (!jpeg_start_decompress(&cinfo)) {
        printf("cam_timer_cb: Failed to start decompression\n");
        jpeg_abort_decompress(&cinfo);
        goto cleanup;
    }

    if (cinfo.output_width != WIDTH || cinfo.output_height != HEIGHT) {
        printf("cam_timer_cb: Unexpected frame size: %dx%d\n", cinfo.output_width, cinfo.output_height);
        jpeg_finish_decompress(&cinfo);
        goto cleanup;
    }

    uint8_t *rgb = malloc(WIDTH * HEIGHT * 3);
    if (!rgb) {
        printf("cam_timer_cb: Failed to allocate RGB buffer\n");
        jpeg_finish_decompress(&cinfo);
        goto cleanup;
    }

    JSAMPARRAY row_pointer = malloc(sizeof(JSAMPROW));
    if (!row_pointer) {
        printf("cam_timer_cb: Failed to allocate row_pointer\n");
        free(rgb);
        jpeg_finish_decompress(&cinfo);
        goto cleanup;
    }

    while (cinfo.output_scanline < HEIGHT) {
        row_pointer[0] = rgb + cinfo.output_scanline * WIDTH * 3;
        jpeg_read_scanlines(&cinfo, row_pointer, 1);
    }
    jpeg_finish_decompress(&cinfo);
    free(row_pointer);

    // 转换为 BGRA
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            int rgb_idx = (y * WIDTH + x) * 3;
            int bgra_idx = (y * WIDTH + x) * 4;
            image_buf[bgra_idx] = rgb[rgb_idx + 2];     // B
            image_buf[bgra_idx + 1] = rgb[rgb_idx + 1]; // G
            image_buf[bgra_idx + 2] = rgb[rgb_idx];     // R
            image_buf[bgra_idx + 3] = 0xFF;             // A
        }
    }
    free(rgb);

    // 更新图像
    lv_image_cache_drop(NULL);
    lv_obj_invalidate(cam_image);
    printf("cam_timer_cb: Frame updated\n");

cleanup:
    if (ioctl(v4l2_fd, VIDIOC_QBUF, &buf) < 0) {
        printf("QBUF failed\n");
    }
}

// 初始化 V4L2
static int init_v4l2() {
    v4l2_fd = open(VIDEO_DEVICE, O_RDWR);
    if (v4l2_fd < 0) {
        printf("init_v4l2: Failed to open %s\n", VIDEO_DEVICE);
        return -1;
    }

    struct v4l2_format fmt = {
        .type = V4L2_BUF_TYPE_VIDEO_CAPTURE,
        .fmt.pix.width = WIDTH,
        .fmt.pix.height = HEIGHT,
        .fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG,
        .fmt.pix.field = V4L2_FIELD_ANY
    };
    if (ioctl(v4l2_fd, VIDIOC_S_FMT, &fmt) < 0) {
        printf("init_v4l2: Failed to set format\n");
        close(v4l2_fd);
        v4l2_fd = -1;
        return -1;
    }

    struct v4l2_streamparm parm = {
        .type = V4L2_BUF_TYPE_VIDEO_CAPTURE,
        .parm.capture.timeperframe.numerator = 1,
        .parm.capture.timeperframe.denominator = FPS
    };
    if (ioctl(v4l2_fd, VIDIOC_S_PARM, &parm) < 0) {
        printf("init_v4l2: Failed to set framerate\n");
        close(v4l2_fd);
        v4l2_fd = -1;
        return -1;
    }

    struct v4l2_requestbuffers req = {
        .count = 4,
        .type = V4L2_BUF_TYPE_VIDEO_CAPTURE,
        .memory = V4L2_MEMORY_MMAP
    };
    if (ioctl(v4l2_fd, VIDIOC_REQBUFS, &req) < 0) {
        printf("init_v4l2: Failed to request buffers\n");
        close(v4l2_fd);
        v4l2_fd = -1;
        return -1;
    }
    buffer_count = req.count;
    if (buffer_count == 0) {
        printf("init_v4l2: No buffers allocated\n");
        close(v4l2_fd);
        v4l2_fd = -1;
        return -1;
    }

    buffers = calloc(buffer_count, sizeof(void *));
    buffer_lengths = calloc(buffer_count, sizeof(size_t));
    if (!buffers || !buffer_lengths) {
        printf("init_v4l2: Failed to allocate buffer arrays\n");
        free(buffers);
        free(buffer_lengths);
        close(v4l2_fd);
        v4l2_fd = -1;
        return -1;
    }

    for (int i = 0; i < buffer_count; i++) {
        struct v4l2_buffer buf = {
            .type = V4L2_BUF_TYPE_VIDEO_CAPTURE,
            .memory = V4L2_MEMORY_MMAP,
            .index = i
        };
        if (ioctl(v4l2_fd, VIDIOC_QUERYBUF, &buf) < 0) {
            printf("init_v4l2: Failed to query buffer %d\n", i);
            close(v4l2_fd);
            v4l2_fd = -1;
            return -1;
        }
        buffers[i] = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, v4l2_fd, buf.m.offset);
        buffer_lengths[i] = buf.length;
        if (buffers[i] == MAP_FAILED) {
            printf("init_v4l2: Failed to mmap buffer %d\n", i);
            close(v4l2_fd);
            v4l2_fd = -1;
            return -1;
        }
        if (ioctl(v4l2_fd, VIDIOC_QBUF, &buf) < 0) {
            printf("init_v4l2: Failed to queue buffer %d\n", i);
            close(v4l2_fd);
            v4l2_fd = -1;
            return -1;
        }
    }

    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(v4l2_fd, VIDIOC_STREAMON, &type) < 0) {
        printf("init_v4l2: Failed to start streaming\n");
        close(v4l2_fd);
        v4l2_fd = -1;
        return -1;
    }
    printf("init_v4l2: V4L2 initialized successfully\n");
    return 0;
}

void k_camare_control() {
    // 检查 LVGL 颜色深度
    if (LV_COLOR_DEPTH != 32) {
        printf("k_camare_control: LV_COLOR_DEPTH must be 32, got %d\n", LV_COLOR_DEPTH);
        return;
    }

    // 获取当前活动屏幕
    lv_obj_t *screen = lv_screen_active();
    if (!screen) {
        printf("k_camare_control: No active screen\n");
        return;
    }

    // 检查 USB 设备状态
    if (access(VIDEO_DEVICE, F_OK) != 0) {
        printf("k_camare_control: Camera device %s not found\n", VIDEO_DEVICE);
        return;
    }

    // 初始化 V4L2
    if (init_v4l2() < 0) {
        printf("k_camare_control: Failed to initialize V4L2\n");
        return;
    }

    // 初始化 JPEG 解码器
    cinfo.err = jpeg_std_error(&jerr);
    jerr.error_exit = jpeg_error_exit;
    jpeg_create_decompress(&cinfo);
    printf("k_camare_control: JPEG decompressor initialized\n");

    // 创建图像控件
    cam_image = lv_image_create(screen);
    if (!cam_image) {
        printf("k_camare_control: Failed to create image\n");
        g_camare_control();
        return;
    }
    lv_obj_set_size(cam_image, WIDTH, HEIGHT);
    lv_obj_set_pos(cam_image, 361, 122);
    printf("k_camare_control: Image widget created at (361, 122)\n");

    // 分配图像缓冲区并初始化为灰色
    image_buf = malloc(WIDTH * HEIGHT * 4);
    if (!image_buf) {
        printf("k_camare_control: Failed to allocate image_buf\n");
        g_camare_control();
        return;
    }
    for (int i = 0; i < WIDTH * HEIGHT * 4; i += 4) {
        image_buf[i] = 0x80;     // B
        image_buf[i + 1] = 0x80; // G
        image_buf[i + 2] = 0x80; // R
        image_buf[i + 3] = 0xFF; // A
    }
    printf("k_camare_control: Image buffer initialized\n");

    // 设置图像描述符
    img_dsc.header.magic = LV_IMAGE_HEADER_MAGIC;
    img_dsc.header.cf = LV_COLOR_FORMAT_ARGB8888;
    img_dsc.header.w = WIDTH;
    img_dsc.header.h = HEIGHT;
    img_dsc.header.stride = WIDTH * 4;
    img_dsc.data = image_buf;
    img_dsc.data_size = WIDTH * HEIGHT * 4;
    lv_image_set_src(cam_image, &img_dsc);
    lv_obj_invalidate(cam_image);
    printf("k_camare_control: Image descriptor set\n");

    // 延迟启动定时器
    cam_timer = lv_timer_create(cam_timer_cb, 1000 / FPS, NULL);
    if (!cam_timer) {
        printf("k_camare_control: Failed to create timer\n");
        g_camare_control();
        return;
    }
    lv_timer_set_repeat_count(cam_timer, -1);
    lv_timer_pause(cam_timer);
    lv_timer_resume(cam_timer); // 确保渲染循环执行后再启动
    printf("k_camare_control: Camera initialized successfully\n");
}

void g_camare_control() {
    // 删除定时器
    if (cam_timer) {
        lv_timer_delete(cam_timer);
        cam_timer = NULL;
        printf("g_camare_control: Timer deleted\n");
    }

    // 删除图像控件
    if (cam_image) {
        lv_obj_delete(cam_image);
        cam_image = NULL;
        printf("g_camare_control: Image widget deleted\n");
    }

    // 释放图像缓冲区
    if (image_buf) {
        free(image_buf);
        image_buf = NULL;
        printf("g_camare_control: Image buffer freed\n");
    }

    // 释放 JPEG 解码器
    jpeg_destroy_decompress(&cinfo);
    printf("g_camare_control: JPEG decompressor destroyed\n");

    // 释放 V4L2 资源
    if (v4l2_fd >= 0) {
        enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        ioctl(v4l2_fd, VIDIOC_STREAMOFF, &type);
        for (int i = 0; i < buffer_count; i++) {
            if (buffers[i]) {
                munmap(buffers[i], buffer_lengths[i]);
                buffers[i] = NULL;
            }
        }
        free(buffers);
        free(buffer_lengths);
        buffers = NULL;
        buffer_lengths = NULL;
        close(v4l2_fd);
        v4l2_fd = -1;
        buffer_count = 0;
        printf("g_camare_control: V4L2 resources released\n");
    }
    printf("g_camare_control: Camera resources fully released\n");
}

void k_motor_control() {
    system("/root/control_device/test_motor 1 800 3000 &");
}

void g_motor_control() {
    system("/root/control_device/test_motor 0 800 3000 &");
}

void k_wuhuaqi_control() {
    system("/root/control_device/wuhuaqi on &");
}

void g_wuhuaqi_control() {
    system("/root/control_device/wuhuaqi off &");
}