/*
* Copyright 2025 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#include "lvgl.h"
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "sys/types.h"
#include "sys/stat.h"
#include "sys/ioctl.h"
#include "stdlib.h"
#include "string.h"
#include <poll.h>
#include <sys/select.h>
#include <sys/time.h>
#include <signal.h>
#include "gui_guider.h"
#include "events_init.h"
#include "widgets_init.h"
#include "custom.h"

// 传感器数据结构
typedef struct {
    int fd;                 // 设备文件描述符
    lv_timer_t *timer;      // 数据更新定时器
    struct {
        int temp;          // 温度 (0.01℃)
        unsigned int press; // 气压 (Pa)
        unsigned int hum;   // 湿度 (0.01%)
    } data;
} BME280_Handle;

// 在custom.h中添加MPU6050数据结构
typedef struct {
    int fd_mpu;                // MPU6050设备文件描述符
    struct {
        int16_t Accel_X_RAW;
        int16_t Accel_Y_RAW;
        int16_t Accel_Z_RAW;
        int16_t Gyro_X_RAW;
        int16_t Gyro_Y_RAW;
        int16_t Gyro_Z_RAW;
        float Temp;
    } mpu_data;
} MPU6050_Handle;

// 在custom.h中添加SGP30结构体
typedef struct {
    int fd;                 // 设备描述符
    struct {
        uint16_t co2;      // CO2浓度(ppm)
        uint16_t tvoc;     // TVOC浓度(ppb)
    } data;
} SGP30_Handle;

static SGP30_Handle sgp30 = { .fd = -1 };

static MPU6050_Handle mpu6050 = { .fd_mpu = -1 };

static BME280_Handle bme280 = { .fd = -1 };

// 初始化BME280传感器
static int bme280_init() {
    bme280.fd = open("/dev/bme280", O_RDONLY);
    if (bme280.fd < 0) {
        perror("BME280打开失败");
        return -1;
    }

    return 0;
}

// 读取传感器数据
static int bme280_read_data() {
    struct {
        int temp;
        unsigned int press;
        unsigned int hum;
    } raw_data;

    ssize_t ret = read(bme280.fd, &raw_data, sizeof(raw_data));
    if (ret != sizeof(raw_data)) {
        if (errno == EAGAIN) return -1;
        perror("BME280读取失败");
        return -2;
    }

    // 数据转换（根据实际传感器数据格式调整）
    bme280.data.temp = raw_data.temp;
    bme280.data.press = raw_data.press;
    bme280.data.hum = raw_data.hum;
    return 0;
}
// MPU6050初始化（在custom.c中添加）
static int mpu6050_init() {
    mpu6050.fd_mpu = open("/dev/mpu6050i2c", O_RDONLY);
    if (mpu6050.fd_mpu < 0) {
        perror("MPU6050打开失败");
        return -1;
    }
    return 0;
}

// MPU6050数据读取（500ms周期）
static int mpu6050_read_data() {
    int16_t buffer[7];
    ssize_t ret = read(mpu6050.fd_mpu, buffer, sizeof(buffer));
    
    if (ret == 0) {
        mpu6050.mpu_data.Accel_X_RAW = buffer[0];
        mpu6050.mpu_data.Accel_Y_RAW = buffer[1];
        mpu6050.mpu_data.Accel_Z_RAW = buffer[2];
        mpu6050.mpu_data.Gyro_X_RAW = buffer[3];
        mpu6050.mpu_data.Gyro_Y_RAW = buffer[4];
        mpu6050.mpu_data.Gyro_Z_RAW = buffer[5];
        mpu6050.mpu_data.Temp = buffer[6];
        return 0;
    }
    return -1;
}

// SGP30初始化（仅打开设备）
static int sgp30_init() {
    sgp30.fd = open("/dev/sgp30", O_RDONLY);
    if (sgp30.fd < 0) {
        perror("SGP30打开失败");
        return -1;
    }
    return 0;
}

// 数据读取与解析（网页3数据格式验证）
static int sgp30_read_data() {
    char raw[64] = {0};
    ssize_t ret = read(sgp30.fd, raw, sizeof(raw));
    
    if (ret > 0) {
        // 解析数据格式："CO2 TVOC"（如"752 4"）
        if(sscanf(raw, "%hu %hu", &sgp30.data.co2, &sgp30.data.tvoc) == 2) {
            // 过滤初始化数据（网页1特性）
            if(sgp30.data.co2 == 400 && sgp30.data.tvoc == 0) 
                return -2; // 初始化未完成
            return 0;
        }
    }
    return -1; // 数据异常
}
// 更新界面显示
static void update_sensor_display(lv_timer_t *timer) {
    lv_ui *ui = (lv_ui *)lv_timer_get_user_data(timer);
    
    if (bme280_read_data() == 0) {
        // 更新温度显示 (带单位)
        lv_label_set_text_fmt(ui->screen_sensor_temp_label, "%.1fC", bme280.data.temp / 100.0f);
        
        // 更新湿度显示 (带百分比)
        lv_label_set_text_fmt(ui->screen_sensor_humi_label, "%.1f%%", bme280.data.hum / 1024.0f);
        
        // 更新气压显示 (转换为hPa)
        lv_label_set_text_fmt(ui->screen_sensor_ya_label, "%.2fhPa", bme280.data.press / 25600.0f);
        
        // 根据数值变化颜色（示例：高温报警）
        if (bme280.data.temp > 3000) { // 30℃以上显示红色
            lv_obj_set_style_text_color(ui->screen_sensor_temp_label, 
                lv_color_hex(0xFF0000), LV_PART_MAIN);
        } else {
            lv_obj_set_style_text_color(ui->screen_sensor_temp_label, 
                lv_color_hex(0xFFFFFF), LV_PART_MAIN);
        }
    } else {
        // 显示错误状态
        lv_label_set_text(ui->screen_sensor_temp_label, "N/A");
        lv_label_set_text(ui->screen_sensor_humi_label, "N/A");
        lv_label_set_text(ui->screen_sensor_ya_label, "N/A");
    }

    /* MPU6050数据显示 */
    if (mpu6050_read_data() == 0) {
        // 加速度显示（±2g量程，参考网页4[4](@ref)）
        lv_label_set_text_fmt(ui->screen_sensor_label_Acc, 
            "Acc.X=%dg Y=%dg Z=%dg", 
            mpu6050.mpu_data.Accel_X_RAW,
            mpu6050.mpu_data.Accel_Y_RAW,
            mpu6050.mpu_data.Accel_Z_RAW);
            
        // 陀螺仪显示（±250dps，参考网页1[1](@ref)）
        lv_label_set_text_fmt(ui->screen_sensor_label_gyro,
            "Gyro.X=%d Y=%d Z=%d",
            mpu6050.mpu_data.Gyro_X_RAW,
            mpu6050.mpu_data.Gyro_Y_RAW,
            mpu6050.mpu_data.Gyro_Z_RAW);
            
        // 温度显示
        lv_label_set_text_fmt(ui->screen_sensor_label_temp,
            "Temp= %fC", (mpu6050.mpu_data.Temp / 340.0) + 36.53);
    } else {
        lv_label_set_text(ui->screen_sensor_label_Acc, "N/A");
        lv_label_set_text(ui->screen_sensor_label_gyro, "N/A");
        lv_label_set_text(ui->screen_sensor_label_temp, "N/A");
    }

    /* 新增SGP30显示 */
    int sgp_ret = sgp30_read_data();
    if(sgp_ret == 0) {
        // CO2显示（网页4量程400-60000ppm）
        lv_label_set_text_fmt(ui->screen_sensor_co2_label, 
            "%dppm", sgp30.data.co2);
            
        // TVOC显示（网页4量程0-60000ppb）
        lv_label_set_text_fmt(ui->screen_sensor_voc_label,
            "%dppb", sgp30.data.tvoc);
            
        // 空气质量预警（参考网页5建议）
        if(sgp30.data.co2 > 1000 || sgp30.data.tvoc > 500) {
            lv_obj_set_style_text_color(ui->screen_sensor_co2_label, 
                lv_color_hex(0xFF4500), LV_PART_MAIN);
        }
    } else if(sgp_ret == -2) {
        lv_label_set_text(ui->screen_sensor_co2_label, "Calibrating...");
        lv_label_set_text(ui->screen_sensor_voc_label, "Calibrating...");
    } else {
        lv_label_set_text(ui->screen_sensor_co2_label, "N/A");
        lv_label_set_text(ui->screen_sensor_voc_label, "N/A");
    }
}

// 初始化传感器显示系统
void init_sensor_display(lv_ui *ui) {
    // 初始化硬件
    if (bme280_init() != 0) {
        lv_label_set_text(ui->screen_sensor_temp_label, "bme280 faild!");
        return;
    }
     /* MPU6050初始化 */
     if (mpu6050_init() != 0) {
        lv_label_set_text(ui->screen_sensor_label_Acc, "MPU6050 faild!");
    }

    /* 新增SGP30初始化 */
    if (sgp30_init() != 0) {
        lv_label_set_text(ui->screen_sensor_co2_label, "SGP30 faild!");
    }
    // 创建定时器（500ms更新一次）
    bme280.timer = lv_timer_create(update_sensor_display, 500, ui);
    lv_timer_set_repeat_count(bme280.timer, -1); // 无限循环
    
    // 立即更新一次显示
    lv_timer_ready(bme280.timer);
}

// 清理传感器资源
void deinit_sensor_display() {
    if (bme280.timer) {
        lv_timer_del(bme280.timer);
        bme280.timer = NULL;
    }
    if (bme280.fd >= 0) {
        close(bme280.fd);
        bme280.fd = -1;

        close(mpu6050.fd_mpu);
        mpu6050.fd_mpu = -1;
        
        close(sgp30.fd);
        sgp30.fd = -1;
    }
}

// 屏幕事件回调（优化功耗）
static void screen_event_cb(lv_event_t *e) {
    lv_obj_t *screen = lv_event_get_current_target(e);
    lv_ui *ui = (lv_ui *)lv_event_get_user_data(e);
    
    switch(lv_event_get_code(e)) {
    case LV_EVENT_SCREEN_LOAD_START:
        if (bme280.timer) lv_timer_resume(bme280.timer);
        break;
    case LV_EVENT_SCREEN_UNLOADED:
        if (bme280.timer) lv_timer_pause(bme280.timer);
        break;
    }
}

// 自定义界面初始化（在setup_ui后调用）
void sensor_hh_init(lv_ui *ui) {
    // 初始化传感器显示
    init_sensor_display(ui);
    
    // 绑定屏幕事件
    lv_obj_add_event_cb(ui->screen_sensor, screen_event_cb, LV_EVENT_ALL, ui);
    
}


void setup_scr_screen_sensor(lv_ui *ui)
{
    //Write codes screen_sensor
    ui->screen_sensor = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_sensor, 720, 480);
    lv_obj_set_scrollbar_mode(ui->screen_sensor, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_sensor, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_sensor, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_src(ui->screen_sensor, &_date_RGB565A8_720x480, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_opa(ui->screen_sensor, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_recolor_opa(ui->screen_sensor, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_sensor_label_1
    ui->screen_sensor_label_1 = lv_label_create(ui->screen_sensor);
    lv_obj_set_pos(ui->screen_sensor_label_1, 178, 0);
    lv_obj_set_size(ui->screen_sensor_label_1, 364, 62);
    lv_label_set_text(ui->screen_sensor_label_1, "车内实时数据");
    lv_label_set_long_mode(ui->screen_sensor_label_1, LV_LABEL_LONG_WRAP);

    //Write style for screen_sensor_label_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_sensor_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_sensor_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_sensor_label_1, lv_color_hex(0xf00000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_sensor_label_1, &lv_font_SourceHanSerifSC_Regular_46, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_sensor_label_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_sensor_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_sensor_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_sensor_label_1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_sensor_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_sensor_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_sensor_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_sensor_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_sensor_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_sensor_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_sensor_back_bt
    ui->screen_sensor_back_bt = lv_button_create(ui->screen_sensor);
    lv_obj_set_pos(ui->screen_sensor_back_bt, 604, 418);
    lv_obj_set_size(ui->screen_sensor_back_bt, 100, 50);
    ui->screen_sensor_back_bt_label = lv_label_create(ui->screen_sensor_back_bt);
    lv_label_set_text(ui->screen_sensor_back_bt_label, "返回");
    lv_label_set_long_mode(ui->screen_sensor_back_bt_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_sensor_back_bt_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_sensor_back_bt, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_sensor_back_bt_label, LV_PCT(100));

    //Write style for screen_sensor_back_bt, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_sensor_back_bt, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_sensor_back_bt, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_sensor_back_bt, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_sensor_back_bt, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_sensor_back_bt, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_sensor_back_bt, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_sensor_back_bt, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_sensor_back_bt, &lv_font_SourceHanSerifSC_Regular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_sensor_back_bt, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_sensor_back_bt, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_sensor_label_2
    ui->screen_sensor_label_2 = lv_label_create(ui->screen_sensor);
    lv_obj_set_pos(ui->screen_sensor_label_2, 27, 320);
    lv_obj_set_size(ui->screen_sensor_label_2, 79, 28);
    lv_label_set_text(ui->screen_sensor_label_2, "温度：");
    lv_label_set_long_mode(ui->screen_sensor_label_2, LV_LABEL_LONG_WRAP);

    //Write style for screen_sensor_label_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_sensor_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_sensor_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_sensor_label_2, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_sensor_label_2, &lv_font_SourceHanSerifSC_Regular_26, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_sensor_label_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_sensor_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_sensor_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_sensor_label_2, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_sensor_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_sensor_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_sensor_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_sensor_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_sensor_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_sensor_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_sensor_label_3
    ui->screen_sensor_label_3 = lv_label_create(ui->screen_sensor);
    lv_obj_set_pos(ui->screen_sensor_label_3, 27, 240);
    lv_obj_set_size(ui->screen_sensor_label_3, 79, 28);
    lv_label_set_text(ui->screen_sensor_label_3, "湿度：");
    lv_label_set_long_mode(ui->screen_sensor_label_3, LV_LABEL_LONG_WRAP);

    //Write style for screen_sensor_label_3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_sensor_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_sensor_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_sensor_label_3, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_sensor_label_3, &lv_font_SourceHanSerifSC_Regular_26, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_sensor_label_3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_sensor_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_sensor_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_sensor_label_3, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_sensor_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_sensor_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_sensor_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_sensor_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_sensor_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_sensor_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_sensor_label_4
    ui->screen_sensor_label_4 = lv_label_create(ui->screen_sensor);
    lv_obj_set_pos(ui->screen_sensor_label_4, 27, 406);
    lv_obj_set_size(ui->screen_sensor_label_4, 79, 28);
    lv_label_set_text(ui->screen_sensor_label_4, "压强：");
    lv_label_set_long_mode(ui->screen_sensor_label_4, LV_LABEL_LONG_WRAP);

    //Write style for screen_sensor_label_4, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_sensor_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_sensor_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_sensor_label_4, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_sensor_label_4, &lv_font_SourceHanSerifSC_Regular_26, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_sensor_label_4, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_sensor_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_sensor_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_sensor_label_4, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_sensor_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_sensor_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_sensor_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_sensor_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_sensor_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_sensor_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_sensor_label_5
    ui->screen_sensor_label_5 = lv_label_create(ui->screen_sensor);
    lv_obj_set_pos(ui->screen_sensor_label_5, 27, 80);
    lv_obj_set_size(ui->screen_sensor_label_5, 67, 28);
    lv_label_set_text(ui->screen_sensor_label_5, "Co2:");
    lv_label_set_long_mode(ui->screen_sensor_label_5, LV_LABEL_LONG_WRAP);

    //Write style for screen_sensor_label_5, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_sensor_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_sensor_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_sensor_label_5, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_sensor_label_5, &lv_font_montserratMedium_26, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_sensor_label_5, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_sensor_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_sensor_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_sensor_label_5, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_sensor_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_sensor_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_sensor_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_sensor_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_sensor_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_sensor_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_sensor_label_6
    ui->screen_sensor_label_6 = lv_label_create(ui->screen_sensor);
    lv_obj_set_pos(ui->screen_sensor_label_6, 27, 163);
    lv_obj_set_size(ui->screen_sensor_label_6, 62, 28);
    lv_label_set_text(ui->screen_sensor_label_6, "VoC:");
    lv_label_set_long_mode(ui->screen_sensor_label_6, LV_LABEL_LONG_WRAP);

    //Write style for screen_sensor_label_6, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_sensor_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_sensor_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_sensor_label_6, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_sensor_label_6, &lv_font_montserratMedium_26, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_sensor_label_6, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_sensor_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_sensor_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_sensor_label_6, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_sensor_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_sensor_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_sensor_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_sensor_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_sensor_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_sensor_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_sensor_cont_1
    ui->screen_sensor_cont_1 = lv_obj_create(ui->screen_sensor);
    lv_obj_set_pos(ui->screen_sensor_cont_1, 329, 151);
    lv_obj_set_size(ui->screen_sensor_cont_1, 362, 212);
    lv_obj_set_scrollbar_mode(ui->screen_sensor_cont_1, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_sensor_cont_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_sensor_cont_1, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->screen_sensor_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->screen_sensor_cont_1, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->screen_sensor_cont_1, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_sensor_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_sensor_cont_1, 123, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_sensor_cont_1, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_sensor_cont_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_sensor_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_sensor_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_sensor_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_sensor_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_sensor_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_sensor_label_7
    ui->screen_sensor_label_7 = lv_label_create(ui->screen_sensor_cont_1);
    lv_obj_set_pos(ui->screen_sensor_label_7, 98, 0);
    lv_obj_set_size(ui->screen_sensor_label_7, 171, 27);
    lv_label_set_text(ui->screen_sensor_label_7, "汽车姿态检测");
    lv_label_set_long_mode(ui->screen_sensor_label_7, LV_LABEL_LONG_WRAP);

    //Write style for screen_sensor_label_7, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_sensor_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_sensor_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_sensor_label_7, lv_color_hex(0x83ff00), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_sensor_label_7, &lv_font_SourceHanSerifSC_Regular_26, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_sensor_label_7, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_sensor_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_sensor_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_sensor_label_7, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_sensor_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_sensor_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_sensor_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_sensor_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_sensor_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_sensor_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_sensor_label_Acc
    ui->screen_sensor_label_Acc = lv_label_create(ui->screen_sensor_cont_1);
    lv_obj_set_pos(ui->screen_sensor_label_Acc, 5, 58);
    lv_obj_set_size(ui->screen_sensor_label_Acc, 337, 26);
    lv_label_set_text(ui->screen_sensor_label_Acc, "Acc. X =");
    lv_label_set_long_mode(ui->screen_sensor_label_Acc, LV_LABEL_LONG_WRAP);

    //Write style for screen_sensor_label_Acc, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_sensor_label_Acc, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_sensor_label_Acc, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_sensor_label_Acc, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_sensor_label_Acc, &lv_font_montserratMedium_20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_sensor_label_Acc, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_sensor_label_Acc, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_sensor_label_Acc, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_sensor_label_Acc, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_sensor_label_Acc, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_sensor_label_Acc, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_sensor_label_Acc, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_sensor_label_Acc, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_sensor_label_Acc, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_sensor_label_Acc, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_sensor_label_gyro
    ui->screen_sensor_label_gyro = lv_label_create(ui->screen_sensor_cont_1);
    lv_obj_set_pos(ui->screen_sensor_label_gyro, 5, 108);
    lv_obj_set_size(ui->screen_sensor_label_gyro, 343, 26);
    lv_label_set_text(ui->screen_sensor_label_gyro, "Gyro. X =");
    lv_label_set_long_mode(ui->screen_sensor_label_gyro, LV_LABEL_LONG_WRAP);

    //Write style for screen_sensor_label_gyro, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_sensor_label_gyro, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_sensor_label_gyro, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_sensor_label_gyro, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_sensor_label_gyro, &lv_font_montserratMedium_20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_sensor_label_gyro, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_sensor_label_gyro, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_sensor_label_gyro, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_sensor_label_gyro, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_sensor_label_gyro, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_sensor_label_gyro, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_sensor_label_gyro, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_sensor_label_gyro, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_sensor_label_gyro, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_sensor_label_gyro, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_sensor_label_temp
    ui->screen_sensor_label_temp = lv_label_create(ui->screen_sensor_cont_1);
    lv_obj_set_pos(ui->screen_sensor_label_temp, 5, 159);
    lv_obj_set_size(ui->screen_sensor_label_temp, 347, 26);
    lv_label_set_text(ui->screen_sensor_label_temp, "Temp. =");
    lv_label_set_long_mode(ui->screen_sensor_label_temp, LV_LABEL_LONG_WRAP);

    //Write style for screen_sensor_label_temp, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_sensor_label_temp, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_sensor_label_temp, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_sensor_label_temp, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_sensor_label_temp, &lv_font_montserratMedium_20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_sensor_label_temp, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_sensor_label_temp, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_sensor_label_temp, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_sensor_label_temp, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_sensor_label_temp, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_sensor_label_temp, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_sensor_label_temp, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_sensor_label_temp, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_sensor_label_temp, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_sensor_label_temp, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_sensor_co2_label
    ui->screen_sensor_co2_label = lv_label_create(ui->screen_sensor);
    lv_obj_set_pos(ui->screen_sensor_co2_label, 94, 80);
    lv_obj_set_size(ui->screen_sensor_co2_label, 179, 28);
    lv_label_set_text(ui->screen_sensor_co2_label, "0\n");
    lv_label_set_long_mode(ui->screen_sensor_co2_label, LV_LABEL_LONG_WRAP);

    //Write style for screen_sensor_co2_label, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_sensor_co2_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_sensor_co2_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_sensor_co2_label, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_sensor_co2_label, &lv_font_montserratMedium_26, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_sensor_co2_label, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_sensor_co2_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_sensor_co2_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_sensor_co2_label, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_sensor_co2_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_sensor_co2_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_sensor_co2_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_sensor_co2_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_sensor_co2_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_sensor_co2_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_sensor_voc_label
    ui->screen_sensor_voc_label = lv_label_create(ui->screen_sensor);
    lv_obj_set_pos(ui->screen_sensor_voc_label, 94, 163);
    lv_obj_set_size(ui->screen_sensor_voc_label, 179, 28);
    lv_label_set_text(ui->screen_sensor_voc_label, "0\n");
    lv_label_set_long_mode(ui->screen_sensor_voc_label, LV_LABEL_LONG_WRAP);

    //Write style for screen_sensor_voc_label, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_sensor_voc_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_sensor_voc_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_sensor_voc_label, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_sensor_voc_label, &lv_font_montserratMedium_26, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_sensor_voc_label, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_sensor_voc_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_sensor_voc_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_sensor_voc_label, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_sensor_voc_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_sensor_voc_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_sensor_voc_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_sensor_voc_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_sensor_voc_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_sensor_voc_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_sensor_humi_label
    ui->screen_sensor_humi_label = lv_label_create(ui->screen_sensor);
    lv_obj_set_pos(ui->screen_sensor_humi_label, 94, 240);
    lv_obj_set_size(ui->screen_sensor_humi_label, 79, 28);
    lv_label_set_text(ui->screen_sensor_humi_label, "0\n");
    lv_label_set_long_mode(ui->screen_sensor_humi_label, LV_LABEL_LONG_WRAP);

    //Write style for screen_sensor_humi_label, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_sensor_humi_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_sensor_humi_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_sensor_humi_label, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_sensor_humi_label, &lv_font_montserratMedium_26, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_sensor_humi_label, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_sensor_humi_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_sensor_humi_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_sensor_humi_label, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_sensor_humi_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_sensor_humi_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_sensor_humi_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_sensor_humi_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_sensor_humi_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_sensor_humi_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_sensor_temp_label
    ui->screen_sensor_temp_label = lv_label_create(ui->screen_sensor);
    lv_obj_set_pos(ui->screen_sensor_temp_label, 94, 320);
    lv_obj_set_size(ui->screen_sensor_temp_label, 79, 28);
    lv_label_set_text(ui->screen_sensor_temp_label, "0\n");
    lv_label_set_long_mode(ui->screen_sensor_temp_label, LV_LABEL_LONG_WRAP);

    //Write style for screen_sensor_temp_label, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_sensor_temp_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_sensor_temp_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_sensor_temp_label, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_sensor_temp_label, &lv_font_montserratMedium_26, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_sensor_temp_label, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_sensor_temp_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_sensor_temp_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_sensor_temp_label, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_sensor_temp_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_sensor_temp_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_sensor_temp_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_sensor_temp_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_sensor_temp_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_sensor_temp_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_sensor_ya_label
    ui->screen_sensor_ya_label = lv_label_create(ui->screen_sensor);
    lv_obj_set_pos(ui->screen_sensor_ya_label, 94, 406);
    lv_obj_set_size(ui->screen_sensor_ya_label, 180, 28);
    lv_label_set_text(ui->screen_sensor_ya_label, "0\n");
    lv_label_set_long_mode(ui->screen_sensor_ya_label, LV_LABEL_LONG_WRAP);

    //Write style for screen_sensor_ya_label, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_sensor_ya_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_sensor_ya_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_sensor_ya_label, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_sensor_ya_label, &lv_font_montserratMedium_26, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_sensor_ya_label, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_sensor_ya_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_sensor_ya_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_sensor_ya_label, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_sensor_ya_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_sensor_ya_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_sensor_ya_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_sensor_ya_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_sensor_ya_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_sensor_ya_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //The custom code of screen_sensor.
    
    sensor_hh_init(ui);

    //Update current screen layout.
    lv_obj_update_layout(ui->screen_sensor);

    //Init events for screen.
    events_init_screen_sensor(ui);
}
