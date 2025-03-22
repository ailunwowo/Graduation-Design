#include "lvgl/lvgl.h"
#include "lvgl/demos/lv_demos.h"
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <stdio.h>
#include <string.h> 
#include "lvgl/src/misc/lv_fs.h" 
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <errno.h>

static void event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if(code == LV_EVENT_CLICKED) {
        LV_LOG_USER("Clicked");
    }
    else if(code == LV_EVENT_VALUE_CHANGED) {
        LV_LOG_USER("Toggled");
    }
}

void lv_example_button_1(void)
{
    lv_obj_t * label;

    lv_obj_t * btn1 = lv_button_create(lv_screen_active());
    lv_obj_add_event_cb(btn1, event_handler, LV_EVENT_ALL, NULL);
    lv_obj_align(btn1, LV_ALIGN_CENTER, 0, -40);
    lv_obj_remove_flag(btn1, LV_OBJ_FLAG_PRESS_LOCK);

    label = lv_label_create(btn1);
    lv_label_set_text(label, "Button");
    lv_obj_center(label);

    lv_obj_t * btn2 = lv_button_create(lv_screen_active());
    lv_obj_add_event_cb(btn2, event_handler, LV_EVENT_ALL, NULL);
    lv_obj_align(btn2, LV_ALIGN_CENTER, 0, 40);
    lv_obj_add_flag(btn2, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_set_height(btn2, LV_SIZE_CONTENT);

    label = lv_label_create(btn2);
    lv_label_set_text(label, "Toggle");
    lv_obj_center(label);

}

void FileTest(void)
{
    // 注意这里的文件路径不要带S标志，因为使用的是rlottie自己的文件系统
    lv_obj_t *lottie = lv_rlottie_create_from_file(lv_scr_act(), 300, 300, "/root/astronut.json");
    lv_obj_center(lottie);
}

int main(void)
{
    lv_init();
    /* 添加下面一行，显示lvgl 版本 */
    printf("LVGL v%d.%d.%d\n", lv_version_major(), lv_version_minor(), lv_version_patch());

    /* 创建 DRM 显示驱动 */
    lv_display_t *disp = lv_linux_drm_create();
    if (!disp) {
        printf("Failed to create DRM display\n");
        return -1;
    }

    /* 配置 DRM 设备文件和连接器 */
    lv_linux_drm_set_file(disp, "/dev/dri/card1", -1); // -1 表示自动选择连接器
    printf("Display resolution: %dx%d\n", lv_disp_get_hor_res(disp), lv_disp_get_ver_res(disp));
    /* 设置分辨率为 800x600 */
    // 创建触摸输入设备
    lv_indev_t* cdev = lv_evdev_create(LV_INDEV_TYPE_POINTER, "/dev/input/by-id/usb-WaveShare_WS170120-event-if00");

    //lv_demo_widgets();
    //lv_demo_music();
    FileTest();
    lv_example_button_1();
    /*Handle LVGL tasks*/
    while(1) {
        lv_timer_handler();
        usleep(5000);
    }

    return 0;
}


/*
环境变量设置
export TOOLCHAIN_ROOT=/home/alen/VisonFive2_SDK/VisionFive2/work/buildroot_rootfs/host/

export PATH=$TOOLCHAIN_ROOT/bin:$PATH
cmake -DCMAKE_C_COMPILER=$TOOLCHAIN_ROOT/bin/riscv64-buildroot-linux-gnu-gcc\
      -DCMAKE_CXX_COMPILER=$TOOLCHAIN_ROOT/bin/riscv64-buildroot-linux-gnu-g++\
      -DCMAKE_SYSTEM_NAME=Linux \
      -DCMAKE_SYSTEM_PROCESSOR=risc-v \
      ..
*/