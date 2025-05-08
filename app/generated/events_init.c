/*
* Copyright 2025 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#include "events_init.h"
#include <stdio.h>
#include "lvgl.h"

#include "control_device.h"

#if LV_USE_GUIDER_SIMULATOR && LV_USE_FREEMASTER
#include "freemaster_client.h"
#endif

#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

#define PWM_PATH "/sys/class/pwm/pwmchip0/pwm1/"
#define PERIOD 20000000   // 20ms (50Hz)
#define MID_DUTY 1500000  // 1.5ms (停止)
#define MIN_DUTY 500000   // 0.5ms (最大CCW速度)
#define MAX_DUTY 2500000  // 2.5ms (最大CW速度)

#define SPI_DEVICE    "/dev/spidev1.0"
#define LED_COUNT     10       // 总共10个LED灯珠
#define SPI_SPEED_HZ  8000000  // 8 MHz
#define SPI_BITS      8
#define SPI_MODE      SPI_MODE_0
#define RESET_DELAY   64       // Reset信号长度

// WS2812B协议参数
#define T0H  0x60  // '0' bit: 01100000
#define T1H  0x7C  // '1' bit: 01111100

int spi_fd;

extern void deinit_sensor_display();

static void screen_sensor_bt_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_sensor, guider_ui.screen_sensor_del, &guider_ui.screen_del, setup_scr_screen_sensor, LV_SCR_LOAD_ANIM_OVER_BOTTOM, 200, 200, false, true);
        break;
    }
    default:
        break;
    }
}

static void screen_control_bt_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_contrl, guider_ui.screen_contrl_del, &guider_ui.screen_del, setup_scr_screen_contrl, LV_SCR_LOAD_ANIM_OVER_BOTTOM, 200, 200, false, true);
        break;
    }
    default:
        break;
    }
}

static void screen_map_bt_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_map, guider_ui.screen_map_del, &guider_ui.screen_del, setup_scr_screen_map, LV_SCR_LOAD_ANIM_OVER_BOTTOM, 200, 200, false, true);
        break;
    }
    default:
        break;
    }
}

void events_init_screen (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_sensor_bt, screen_sensor_bt_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_control_bt, screen_control_bt_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_map_bt, screen_map_bt_event_handler, LV_EVENT_ALL, ui);
}

static void screen_sensor_back_bt_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        deinit_sensor_display();
        ui_load_scr_animation(&guider_ui, &guider_ui.screen, guider_ui.screen_del, &guider_ui.screen_sensor_del, setup_scr_screen, LV_SCR_LOAD_ANIM_OVER_TOP, 200, 200, true, true);
        break;
    }
    default:
        break;
    }
}

void events_init_screen_sensor (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_sensor_back_bt, screen_sensor_back_bt_event_handler, LV_EVENT_ALL, ui);
}

static void screen_contrl_btn_1_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen, guider_ui.screen_del, &guider_ui.screen_contrl_del, setup_scr_screen, LV_SCR_LOAD_ANIM_OVER_TOP, 200, 200, true, true);
        break;
    }
    default:
        break;
    }
}

static void screen_contrl_sw_kgc_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        lv_obj_t * status_obj = lv_event_get_target(e);
        int status = lv_obj_has_state(status_obj, LV_STATE_CHECKED) ? true : false;
        if (status) {
            // 开启窗户
            k_motor_control();
           
        } else {
            // 关闭窗户
            g_motor_control();
        }
        break;
    }
    default:
        break;
    }
}

static void screen_contrl_sw_wuhuaqi_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        lv_obj_t * status_obj = lv_event_get_target(e);
        int status = lv_obj_has_state(status_obj, LV_STATE_CHECKED) ? true : false;
        if (status) {
            // 开启加湿器
            k_wuhuaqi_control();
        } else {
            // 关闭加湿器
            g_wuhuaqi_control();
        }
        break;
    }
    default:
        break;
    }
}

static void screen_contrl_sw_carme_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        lv_obj_t * status_obj = lv_event_get_target(e);
        int status = lv_obj_has_state(status_obj, LV_STATE_CHECKED) ? true : false;
        if (status) {
            // 开启加湿器
            k_camare_control();
        } else {
            // 关闭加湿器
            g_camare_control();
        }
        break;
    }
    default:
        break;
    }
}

static void screen_contrl_slider_fan_event_handler(lv_event_t * e)
{
    lv_obj_t * slider = lv_event_get_target(e);

    char buffer[64];
    sprintf(buffer, "/root/control_device/control_fan_pwm0.sh -s %d &", lv_slider_get_value(slider));
    system(buffer);
}

// PWM初始化函数
static int pwm_init() {
    int fd;
    char path[256];
    
    system("echo 1 > /sys/class/pwm/pwmchip0/export");
    // 设置周期
    snprintf(path, sizeof(path), "%speriod", PWM_PATH);
    fd = open(path, O_WRONLY);
    if (fd < 0) return -1;
    dprintf(fd, "%d", PERIOD);
    close(fd);
    
    // 设置初始占空比(停止)
    snprintf(path, sizeof(path), "%sduty_cycle", PWM_PATH);
    fd = open(path, O_WRONLY);
    if (fd < 0) return -1;
    dprintf(fd, "%d", MID_DUTY);
    close(fd);
    
    // 设置极性
    snprintf(path, sizeof(path), "%spolarity", PWM_PATH);
    fd = open(path, O_WRONLY);
    if (fd < 0) return -1;
    dprintf(fd, "normal");
    close(fd);
    
    // 启用PWM
    snprintf(path, sizeof(path), "%senable", PWM_PATH);
    fd = open(path, O_WRONLY);
    if (fd < 0) return -1;
    dprintf(fd, "1");
    close(fd);
    
    return 0;
}

// 设置舵机函数
static void set_servo(int direction, int speed) {
    int duty_cycle;
    char path[256];
    int fd;
    
    // 计算占空比
    if (direction == 1) { // CW
        duty_cycle = MID_DUTY + (speed * (MAX_DUTY - MID_DUTY) / 100);
    } else { // CCW
        duty_cycle = MID_DUTY - (speed * (MID_DUTY - MIN_DUTY) / 100);
    }
    
    // 设置占空比
    snprintf(path, sizeof(path), "%sduty_cycle", PWM_PATH);
    fd = open(path, O_WRONLY);
    if (fd < 0) return;
    dprintf(fd, "%d", duty_cycle);
    close(fd);
}

static void screen_contrl_slider_mg90s_event_handler(lv_event_t * e) {
    lv_obj_t * slider = lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);
    static int32_t last_value = 50; // 初始中间值
    int32_t current_value = lv_slider_get_value(slider);
    
    // 初始化PWM (只需一次)
    static int initialized = 0;
    if (!initialized) {
        if (pwm_init() < 0) {
            printf("PWM初始化失败!\n");
            return;
        }
        initialized = 1;
    }
    
    // 值变化处理
    if(current_value !=0 ){
        if (current_value > last_value) {
            // 值增加 - 顺时针旋转 (速度8%)
            set_servo(1, 4);
        } else if (current_value < last_value) {
            // 值减少 - 逆时针旋转 (速度8%)
            set_servo(0, 8);
        }
    }
    if(code == LV_EVENT_RELEASED) {
        // 释放事件 - 停止舵机
        set_servo(1, 0);
    }
    
    last_value = current_value;
    
    // 可选: 添加短暂延时后停止 (0.1秒)
    // usleep(100000);
    // set_servo(1, 0); // 停止
}

// 将RGB转换为WS2812B的SPI数据格式
void rgb_to_spi(uint8_t r, uint8_t g, uint8_t b, uint8_t *spi_buf) {
    uint8_t grb[3] = {g, r, b};  // WS2812B使用GRB顺序
    for (int i = 0; i < 24; i++) {
        uint8_t bit = (grb[i / 8] >> (7 - (i % 8))) & 0x01;
        spi_buf[i] = bit ? T1H : T0H;
    }
}

// 初始化SPI
int init_spi() {
    uint8_t mode = SPI_MODE;
    uint8_t bits = SPI_BITS;  // 将宏定义赋值给变量
    uint32_t speed = SPI_SPEED_HZ;

    if ((spi_fd = open(SPI_DEVICE, O_RDWR)) < 0) {
        perror("Failed to open SPI device");
        return -1;
    }

    if (ioctl(spi_fd, SPI_IOC_WR_MODE, &mode) < 0 ||
        ioctl(spi_fd, SPI_IOC_WR_BITS_PER_WORD, &bits) < 0 ||  // 使用变量地址
        ioctl(spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) < 0) {
        perror("Failed to set SPI parameters");
        close(spi_fd);
        return -1;
    }

    return 0;
}

// 控制单个LED
void set_led(int led_pos, uint8_t r, uint8_t g, uint8_t b) {
    static uint8_t led_data[LED_COUNT][24] = {0};
    uint8_t output_buf[RESET_DELAY + (LED_COUNT * 24) + RESET_DELAY];
    uint8_t reset_buf[RESET_DELAY] = {0};

    if (led_pos >= 0 && led_pos < LED_COUNT) {
        rgb_to_spi(r, g, b, led_data[led_pos]);
    }

    memset(output_buf, 0x00, sizeof(output_buf));
    memcpy(output_buf + RESET_DELAY, led_data, sizeof(led_data));
    
    write(spi_fd, reset_buf, sizeof(reset_buf));
    write(spi_fd, output_buf, sizeof(output_buf));
    write(spi_fd, reset_buf, sizeof(reset_buf));
}

// 清除所有LED
void clear_all() {
    for (int i = 0; i < LED_COUNT; i++) {
        set_led(i, 0x00, 0x00, 0x00);
    }
}
// 渐变红绿蓝效果
void gradient_rgb_effect() {
    // 定义渐变颜色参数
    const uint8_t steps = 30;  // 每个颜色渐变步数
    const uint16_t delay_ms = 50;  // 每步延迟(ms)
    
    while(1) {  // 无限循环，可按需修改
        // 红色渐变到绿色
        for (uint8_t s = 0; s < steps; s++) {
            uint8_t r = 255 * (steps - s) / steps;
            uint8_t g = 255 * s / steps;
            for (int i = 0; i < LED_COUNT; i++) {
                set_led(i, r, g, 0);
            }
            usleep(delay_ms * 1000);
        }
        
        // 绿色渐变到蓝色
        for (uint8_t s = 0; s < steps; s++) {
            uint8_t g = 255 * (steps - s) / steps;
            uint8_t b = 255 * s / steps;
            for (int i = 0; i < LED_COUNT; i++) {
                set_led(i, 0, g, b);
            }
            usleep(delay_ms * 1000);
        }
        
        // 蓝色渐变到红色
        for (uint8_t s = 0; s < steps; s++) {
            uint8_t b = 255 * (steps - s) / steps;
            uint8_t r = 255 * s / steps;
            for (int i = 0; i < LED_COUNT; i++) {
                set_led(i, r, 0, b);
            }
            usleep(delay_ms * 1000);
        }
    }
}
static void screen_contrl_slider_ws2812b_event_handler(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * slider = lv_event_get_target(e);
    static int spi_initialized = 0;
    static int last_value = -1; // 记录上一次的值
    
    // 初始化SPI (仅一次)
    if (!spi_initialized) {
        if (init_spi() < 0) {
            printf("SPI初始化失败!\n");
            return;
        }
        spi_initialized = 1;
        clear_all(); // 初始时关闭所有LED
    }
    
    if (code == LV_EVENT_VALUE_CHANGED) {
        int current_value = lv_slider_get_value(slider);
        
        // 避免重复处理相同的值
        if (current_value == last_value) return;
        
        // 根据滑动条值设置不同颜色模式
        if (current_value < 32) {
            // 0-31: 关闭所有LED
            clear_all();
        } 
        else if (current_value < 64) {
            // 32-63: 红色渐变
            uint8_t intensity = (current_value - 32) * 8;
            for (int i = 0; i < LED_COUNT; i++) {
                set_led(i, intensity, 0, 0);
            }
        }
        else if (current_value < 96) {
            // 64-95: 绿色渐变
            uint8_t intensity = (current_value - 64) * 8;
            for (int i = 0; i < LED_COUNT; i++) {
                set_led(i, 0, intensity, 0);
            }
        }
        else if (current_value < 128) {
            // 96-127: 蓝色渐变
            uint8_t intensity = (current_value - 96) * 8;
            for (int i = 0; i < LED_COUNT; i++) {
                set_led(i, 0, 0, intensity);
            }
        }
        else if (current_value < 160) {
            // 128-159: 暖白色
            uint8_t intensity = (current_value - 128) * 4;
            for (int i = 0; i < LED_COUNT; i++) {
                set_led(i, intensity, intensity/2, intensity/4);
            }
        }
        else if (current_value < 192) {
            // 160-191: 彩虹色分布
            for (int i = 0; i < LED_COUNT; i++) {
                uint8_t pos = (i * 255 / LED_COUNT + (current_value - 160) * 4) % 255;
                if (pos < 85) {
                    set_led(i, pos * 3, 255 - pos * 3, 0);
                } else if (pos < 170) {
                    pos -= 85;
                    set_led(i, 255 - pos * 3, 0, pos * 3);
                } else {
                    pos -= 170;
                    set_led(i, 0, pos * 3, 255 - pos * 3);
                }
            }
        }
        else {
            // 192-255: RGB渐变效果
            static uint8_t hue = 0;
            hue = (current_value - 192) * 4;
            for (int i = 0; i < LED_COUNT; i++) {
                uint8_t pos = (i * 255 / LED_COUNT + hue) % 255;
                if (pos < 85) {
                    set_led(i, pos * 3, 255 - pos * 3, 0);
                } else if (pos < 170) {
                    pos -= 85;
                    set_led(i, 255 - pos * 3, 0, pos * 3);
                } else {
                    pos -= 170;
                    set_led(i, 0, pos * 3, 255 - pos * 3);
                }
            }
        }
        
        last_value = current_value;
    }
    else if (code == LV_EVENT_RELEASED) {
        // 释放时保持当前状态，不执行任何操作
        // 或者可以添加特殊效果，例如：
        // clear_all();
        // set_led(0, 0, 255, 0); // 点亮第一个LED为绿色表示确认
        // usleep(500000);
        // set_led(0, 0, 0, 0);
    }
}

void events_init_screen_contrl (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_contrl_btn_1, screen_contrl_btn_1_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_contrl_sw_kgc, screen_contrl_sw_kgc_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_contrl_sw_wuhuaqi, screen_contrl_sw_wuhuaqi_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_contrl_sw_carme, screen_contrl_sw_carme_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_contrl_slider_fan, screen_contrl_slider_fan_event_handler, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_add_event_cb(ui->screen_contrl_slider_mg90s, screen_contrl_slider_mg90s_event_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui->screen_contrl_slider_ws2812b, screen_contrl_slider_ws2812b_event_handler, LV_EVENT_ALL, NULL);
}

static void screen_map_btn_1_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen, guider_ui.screen_del, &guider_ui.screen_map_del, setup_scr_screen, LV_SCR_LOAD_ANIM_OVER_TOP, 200, 200, true, true);
        break;
    }
    default:
        break;
    }
}

void events_init_screen_map (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_map_btn_1, screen_map_btn_1_event_handler, LV_EVENT_ALL, ui);
}


void events_init(lv_ui *ui)
{

}
