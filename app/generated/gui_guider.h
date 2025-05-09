/*
* Copyright 2025 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#ifndef GUI_GUIDER_H
#define GUI_GUIDER_H
#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"


typedef struct
{
  
	lv_obj_t *screen;
	bool screen_del;
	lv_obj_t *screen_img_1;
	lv_obj_t *screen_label_1;
	lv_obj_t *screen_sensor_bt;
	lv_obj_t *screen_sensor_bt_label;
	lv_obj_t *screen_control_bt;
	lv_obj_t *screen_control_bt_label;
	lv_obj_t *screen_map_bt;
	lv_obj_t *screen_map_bt_label;
	lv_obj_t *screen_sensor;
	bool screen_sensor_del;
	lv_obj_t *screen_sensor_label_1;
	lv_obj_t *screen_sensor_back_bt;
	lv_obj_t *screen_sensor_back_bt_label;
	lv_obj_t *screen_sensor_label_2;
	lv_obj_t *screen_sensor_label_3;
	lv_obj_t *screen_sensor_label_4;
	lv_obj_t *screen_sensor_label_5;
	lv_obj_t *screen_sensor_label_6;
	lv_obj_t *screen_sensor_cont_1;
	lv_obj_t *screen_sensor_label_7;
	lv_obj_t *screen_sensor_label_Acc;
	lv_obj_t *screen_sensor_label_gyro;
	lv_obj_t *screen_sensor_label_temp;
	lv_obj_t *screen_sensor_co2_label;
	lv_obj_t *screen_sensor_voc_label;
	lv_obj_t *screen_sensor_humi_label;
	lv_obj_t *screen_sensor_temp_label;
	lv_obj_t *screen_sensor_ya_label;
	lv_obj_t *screen_contrl;
	bool screen_contrl_del;
	lv_obj_t *screen_contrl_label_1;
	lv_obj_t *screen_contrl_btn_1;
	lv_obj_t *screen_contrl_btn_1_label;
	lv_obj_t *screen_contrl_slider_mg90s;
	lv_obj_t *screen_contrl_slider_fan;
	lv_obj_t *screen_contrl_sw_kgc;
	lv_obj_t *screen_contrl_sw_wuhuaqi;
	lv_obj_t *screen_contrl_sw_carme;
	lv_obj_t *screen_contrl_label_2;
	lv_obj_t *screen_contrl_label_3;
	lv_obj_t *screen_contrl_label_4;
	lv_obj_t *screen_contrl_label_5;
	lv_obj_t *screen_contrl_label_6;
	lv_obj_t *screen_contrl_label_7;
	lv_obj_t *screen_contrl_slider_ws2812b;
	lv_obj_t *screen_map;
	lv_obj_t* map_img;
	bool screen_map_del;
	lv_obj_t *screen_map_label_1;
	lv_obj_t *screen_map_btn_1;
	lv_obj_t *screen_map_btn_1_label;
}lv_ui;

typedef void (*ui_setup_scr_t)(lv_ui * ui);

void ui_init_style(lv_style_t * style);

void ui_load_scr_animation(lv_ui *ui, lv_obj_t ** new_scr, bool new_scr_del, bool * old_scr_del, ui_setup_scr_t setup_scr,
                           lv_screen_load_anim_t anim_type, uint32_t time, uint32_t delay, bool is_clean, bool auto_del);

void ui_animation(void * var, uint32_t duration, int32_t delay, int32_t start_value, int32_t end_value, lv_anim_path_cb_t path_cb,
                  uint32_t repeat_cnt, uint32_t repeat_delay, uint32_t playback_time, uint32_t playback_delay,
                  lv_anim_exec_xcb_t exec_cb, lv_anim_start_cb_t start_cb, lv_anim_completed_cb_t ready_cb, lv_anim_deleted_cb_t deleted_cb);


void init_scr_del_flag(lv_ui *ui);

void setup_bottom_layer(void);

void setup_ui(lv_ui *ui);

void video_play(lv_ui *ui);

void init_keyboard(lv_ui *ui);

extern lv_ui guider_ui;


void setup_scr_screen(lv_ui *ui);
void setup_scr_screen_sensor(lv_ui *ui);
void setup_scr_screen_contrl(lv_ui *ui);
void setup_scr_screen_map(lv_ui *ui);
LV_IMAGE_DECLARE(_car_XRGB8888_722x480);

LV_IMAGE_DECLARE(_date_RGB565A8_720x480);

LV_IMAGE_DECLARE(_control2_RGB565A8_720x480);

LV_IMAGE_DECLARE(_version_RGB565A8_720x480);

LV_FONT_DECLARE(lv_font_SourceHanSerifSC_Regular_46)
LV_FONT_DECLARE(lv_font_montserratMedium_16)
LV_FONT_DECLARE(lv_font_SourceHanSerifSC_Regular_16)
LV_FONT_DECLARE(lv_font_montserratMedium_12)
LV_FONT_DECLARE(lv_font_SourceHanSerifSC_Regular_26)
LV_FONT_DECLARE(lv_font_montserratMedium_26)
LV_FONT_DECLARE(lv_font_montserratMedium_20)


#ifdef __cplusplus
}
#endif
#endif
