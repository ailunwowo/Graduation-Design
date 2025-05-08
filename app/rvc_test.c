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
#include "gui_guider.h"
#include "custom.h"

#include "rvc_test.h"

lv_ui guider_ui;

void lf_system_init(void)
{
    //关闭加湿器
    system("/root/control_device/wuhuaqi off");
    //加载步进电机驱动模块
    system("insmod /root/control_device/motor.ko");
    //加载传感器的驱动
    system("insmod /root/sensor_device/bme280.ko");
    system("insmod /root/sensor_device/sgp30.ko");
    system("insmod /root/sensor_device/mpu6050.ko");

}

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


void lf_system_ui()
{
    // 注意这里的文件路径不要带S标志，因为使用的是rlottie自己的文件系统
    lv_obj_t *lottie = lv_rlottie_create_from_file(lv_scr_act(), 300, 300, "/root/astronut.json");
    lv_obj_center(lottie);
}

void app_main(void)
{
    setup_ui(&guider_ui);  
}
