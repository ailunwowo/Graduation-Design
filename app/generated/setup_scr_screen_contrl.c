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
#include "gui_guider.h"
#include "events_init.h"
#include "widgets_init.h"
#include "custom.h"



void setup_scr_screen_contrl(lv_ui *ui)
{
    //Write codes screen_contrl
    ui->screen_contrl = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_contrl, 720, 480);
    lv_obj_set_scrollbar_mode(ui->screen_contrl, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_contrl, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_contrl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_src(ui->screen_contrl, &_control2_RGB565A8_720x480, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_opa(ui->screen_contrl, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_recolor_opa(ui->screen_contrl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_contrl_label_1
    ui->screen_contrl_label_1 = lv_label_create(ui->screen_contrl);
    lv_obj_set_pos(ui->screen_contrl_label_1, 178, 0);
    lv_obj_set_size(ui->screen_contrl_label_1, 364, 62);
    lv_label_set_text(ui->screen_contrl_label_1, "座舱操控界面");
    lv_label_set_long_mode(ui->screen_contrl_label_1, LV_LABEL_LONG_WRAP);

    //Write style for screen_contrl_label_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_contrl_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_contrl_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_contrl_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_contrl_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_contrl_label_1, lv_color_hex(0xf00000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_contrl_label_1, &lv_font_SourceHanSerifSC_Regular_46, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_contrl_label_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_contrl_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_contrl_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_contrl_label_1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_contrl_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_contrl_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_contrl_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_contrl_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_contrl_btn_1
    ui->screen_contrl_btn_1 = lv_button_create(ui->screen_contrl);
    lv_obj_set_pos(ui->screen_contrl_btn_1, 601, 418);
    lv_obj_set_size(ui->screen_contrl_btn_1, 100, 50);
    ui->screen_contrl_btn_1_label = lv_label_create(ui->screen_contrl_btn_1);
    lv_label_set_text(ui->screen_contrl_btn_1_label, "返回");
    lv_label_set_long_mode(ui->screen_contrl_btn_1_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_contrl_btn_1_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_contrl_btn_1, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_contrl_btn_1_label, LV_PCT(100));

    //Write style for screen_contrl_btn_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_contrl_btn_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_contrl_btn_1, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_contrl_btn_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_contrl_btn_1, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_contrl_btn_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_contrl_btn_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_contrl_btn_1, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_contrl_btn_1, &lv_font_SourceHanSerifSC_Regular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_contrl_btn_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_contrl_btn_1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_contrl_slider_mg90s
    ui->screen_contrl_slider_mg90s = lv_slider_create(ui->screen_contrl);
    lv_obj_set_pos(ui->screen_contrl_slider_mg90s, 159, 400);
    lv_obj_set_size(ui->screen_contrl_slider_mg90s, 160, 8);
    lv_slider_set_range(ui->screen_contrl_slider_mg90s, 0, 100);
    lv_slider_set_mode(ui->screen_contrl_slider_mg90s, LV_SLIDER_MODE_NORMAL);
    lv_slider_set_value(ui->screen_contrl_slider_mg90s, 0, LV_ANIM_OFF);

    //Write style for screen_contrl_slider_mg90s, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_contrl_slider_mg90s, 60, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_contrl_slider_mg90s, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_contrl_slider_mg90s, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_contrl_slider_mg90s, 50, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_outline_width(ui->screen_contrl_slider_mg90s, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_contrl_slider_mg90s, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_contrl_slider_mg90s, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_contrl_slider_mg90s, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_contrl_slider_mg90s, lv_color_hex(0x2195f6), LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_contrl_slider_mg90s, LV_GRAD_DIR_NONE, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_contrl_slider_mg90s, 50, LV_PART_INDICATOR|LV_STATE_DEFAULT);

    //Write style for screen_contrl_slider_mg90s, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_contrl_slider_mg90s, 255, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_contrl_slider_mg90s, lv_color_hex(0x2195f6), LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_contrl_slider_mg90s, LV_GRAD_DIR_NONE, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_contrl_slider_mg90s, 50, LV_PART_KNOB|LV_STATE_DEFAULT);

    //Write codes screen_contrl_slider_fan
    ui->screen_contrl_slider_fan = lv_slider_create(ui->screen_contrl);
    lv_obj_set_pos(ui->screen_contrl_slider_fan, 156, 338);
    lv_obj_set_size(ui->screen_contrl_slider_fan, 160, 8);
    lv_slider_set_range(ui->screen_contrl_slider_fan, 0, 100);
    lv_slider_set_mode(ui->screen_contrl_slider_fan, LV_SLIDER_MODE_NORMAL);
    lv_slider_set_value(ui->screen_contrl_slider_fan, 0, LV_ANIM_OFF);

    //Write style for screen_contrl_slider_fan, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_contrl_slider_fan, 60, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_contrl_slider_fan, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_contrl_slider_fan, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_contrl_slider_fan, 50, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_contrl_slider_fan, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_outline_width(ui->screen_contrl_slider_fan, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_contrl_slider_fan, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_contrl_slider_fan, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_contrl_slider_fan, lv_color_hex(0x2195f6), LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_contrl_slider_fan, LV_GRAD_DIR_NONE, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_contrl_slider_fan, 50, LV_PART_INDICATOR|LV_STATE_DEFAULT);

    //Write style for screen_contrl_slider_fan, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_contrl_slider_fan, 255, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_contrl_slider_fan, lv_color_hex(0x2195f6), LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_contrl_slider_fan, LV_GRAD_DIR_NONE, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_contrl_slider_fan, 50, LV_PART_KNOB|LV_STATE_DEFAULT);

    //Write codes screen_contrl_sw_kgc
    ui->screen_contrl_sw_kgc = lv_switch_create(ui->screen_contrl);
    lv_obj_set_pos(ui->screen_contrl_sw_kgc, 154, 73);
    lv_obj_set_size(ui->screen_contrl_sw_kgc, 96, 35);

    //Write style for screen_contrl_sw_kgc, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_contrl_sw_kgc, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_contrl_sw_kgc, lv_color_hex(0xe6e2e6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_contrl_sw_kgc, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_contrl_sw_kgc, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_contrl_sw_kgc, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_contrl_sw_kgc, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_contrl_sw_kgc, Part: LV_PART_INDICATOR, State: LV_STATE_CHECKED.
    lv_obj_set_style_bg_opa(ui->screen_contrl_sw_kgc, 255, LV_PART_INDICATOR|LV_STATE_CHECKED);
    lv_obj_set_style_bg_color(ui->screen_contrl_sw_kgc, lv_color_hex(0x2195f6), LV_PART_INDICATOR|LV_STATE_CHECKED);
    lv_obj_set_style_bg_grad_dir(ui->screen_contrl_sw_kgc, LV_GRAD_DIR_NONE, LV_PART_INDICATOR|LV_STATE_CHECKED);
    lv_obj_set_style_border_width(ui->screen_contrl_sw_kgc, 0, LV_PART_INDICATOR|LV_STATE_CHECKED);

    //Write style for screen_contrl_sw_kgc, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_contrl_sw_kgc, 255, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_contrl_sw_kgc, lv_color_hex(0xffffff), LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_contrl_sw_kgc, LV_GRAD_DIR_NONE, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_contrl_sw_kgc, 0, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_contrl_sw_kgc, 10, LV_PART_KNOB|LV_STATE_DEFAULT);

    //Write codes screen_contrl_sw_wuhuaqi
    ui->screen_contrl_sw_wuhuaqi = lv_switch_create(ui->screen_contrl);
    lv_obj_set_pos(ui->screen_contrl_sw_wuhuaqi, 154, 135);
    lv_obj_set_size(ui->screen_contrl_sw_wuhuaqi, 96, 35);

    //Write style for screen_contrl_sw_wuhuaqi, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_contrl_sw_wuhuaqi, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_contrl_sw_wuhuaqi, lv_color_hex(0xe6e2e6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_contrl_sw_wuhuaqi, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_contrl_sw_wuhuaqi, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_contrl_sw_wuhuaqi, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_contrl_sw_wuhuaqi, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_contrl_sw_wuhuaqi, Part: LV_PART_INDICATOR, State: LV_STATE_CHECKED.
    lv_obj_set_style_bg_opa(ui->screen_contrl_sw_wuhuaqi, 255, LV_PART_INDICATOR|LV_STATE_CHECKED);
    lv_obj_set_style_bg_color(ui->screen_contrl_sw_wuhuaqi, lv_color_hex(0x2195f6), LV_PART_INDICATOR|LV_STATE_CHECKED);
    lv_obj_set_style_bg_grad_dir(ui->screen_contrl_sw_wuhuaqi, LV_GRAD_DIR_NONE, LV_PART_INDICATOR|LV_STATE_CHECKED);
    lv_obj_set_style_border_width(ui->screen_contrl_sw_wuhuaqi, 0, LV_PART_INDICATOR|LV_STATE_CHECKED);

    //Write style for screen_contrl_sw_wuhuaqi, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_contrl_sw_wuhuaqi, 255, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_contrl_sw_wuhuaqi, lv_color_hex(0xffffff), LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_contrl_sw_wuhuaqi, LV_GRAD_DIR_NONE, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_contrl_sw_wuhuaqi, 0, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_contrl_sw_wuhuaqi, 10, LV_PART_KNOB|LV_STATE_DEFAULT);

    //Write codes screen_contrl_sw_carme
    ui->screen_contrl_sw_carme = lv_switch_create(ui->screen_contrl);
    lv_obj_set_pos(ui->screen_contrl_sw_carme, 154, 207);
    lv_obj_set_size(ui->screen_contrl_sw_carme, 96, 35);

    //Write style for screen_contrl_sw_carme, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_contrl_sw_carme, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_contrl_sw_carme, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_contrl_sw_carme, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_contrl_sw_carme, lv_color_hex(0xe6e2e6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_contrl_sw_carme, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_contrl_sw_carme, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_contrl_sw_carme, Part: LV_PART_INDICATOR, State: LV_STATE_CHECKED.
    lv_obj_set_style_bg_opa(ui->screen_contrl_sw_carme, 255, LV_PART_INDICATOR|LV_STATE_CHECKED);
    lv_obj_set_style_bg_color(ui->screen_contrl_sw_carme, lv_color_hex(0x2195f6), LV_PART_INDICATOR|LV_STATE_CHECKED);
    lv_obj_set_style_bg_grad_dir(ui->screen_contrl_sw_carme, LV_GRAD_DIR_NONE, LV_PART_INDICATOR|LV_STATE_CHECKED);
    lv_obj_set_style_border_width(ui->screen_contrl_sw_carme, 0, LV_PART_INDICATOR|LV_STATE_CHECKED);

    //Write style for screen_contrl_sw_carme, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_contrl_sw_carme, 255, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_contrl_sw_carme, lv_color_hex(0xffffff), LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_contrl_sw_carme, LV_GRAD_DIR_NONE, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_contrl_sw_carme, 0, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_contrl_sw_carme, 10, LV_PART_KNOB|LV_STATE_DEFAULT);

    //Write codes screen_contrl_label_2
    ui->screen_contrl_label_2 = lv_label_create(ui->screen_contrl);
    lv_obj_set_pos(ui->screen_contrl_label_2, 52, 73);
    lv_obj_set_size(ui->screen_contrl_label_2, 105, 28);
    lv_label_set_text(ui->screen_contrl_label_2, "开关窗:");
    lv_label_set_long_mode(ui->screen_contrl_label_2, LV_LABEL_LONG_WRAP);

    //Write style for screen_contrl_label_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_contrl_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_contrl_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_contrl_label_2, lv_color_hex(0x00ff08), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_contrl_label_2, &lv_font_SourceHanSerifSC_Regular_26, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_contrl_label_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_contrl_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_contrl_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_contrl_label_2, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_contrl_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_contrl_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_contrl_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_contrl_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_contrl_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_contrl_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_contrl_label_3
    ui->screen_contrl_label_3 = lv_label_create(ui->screen_contrl);
    lv_obj_set_pos(ui->screen_contrl_label_3, 0, 135);
    lv_obj_set_size(ui->screen_contrl_label_3, 142, 28);
    lv_label_set_text(ui->screen_contrl_label_3, "开关加湿器:");
    lv_label_set_long_mode(ui->screen_contrl_label_3, LV_LABEL_LONG_WRAP);

    //Write style for screen_contrl_label_3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_contrl_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_contrl_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_contrl_label_3, lv_color_hex(0x00ff08), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_contrl_label_3, &lv_font_SourceHanSerifSC_Regular_26, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_contrl_label_3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_contrl_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_contrl_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_contrl_label_3, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_contrl_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_contrl_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_contrl_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_contrl_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_contrl_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_contrl_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_contrl_label_4
    ui->screen_contrl_label_4 = lv_label_create(ui->screen_contrl);
    lv_obj_set_pos(ui->screen_contrl_label_4, 0, 207);
    lv_obj_set_size(ui->screen_contrl_label_4, 144, 28);
    lv_label_set_text(ui->screen_contrl_label_4, "开关摄像头:");
    lv_label_set_long_mode(ui->screen_contrl_label_4, LV_LABEL_LONG_WRAP);

    //Write style for screen_contrl_label_4, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_contrl_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_contrl_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_contrl_label_4, lv_color_hex(0x00ff08), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_contrl_label_4, &lv_font_SourceHanSerifSC_Regular_26, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_contrl_label_4, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_contrl_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_contrl_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_contrl_label_4, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_contrl_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_contrl_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_contrl_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_contrl_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_contrl_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_contrl_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_contrl_label_5
    ui->screen_contrl_label_5 = lv_label_create(ui->screen_contrl);
    lv_obj_set_pos(ui->screen_contrl_label_5, 29, 324);
    lv_obj_set_size(ui->screen_contrl_label_5, 120, 28);
    lv_label_set_text(ui->screen_contrl_label_5, "风力调节:");
    lv_label_set_long_mode(ui->screen_contrl_label_5, LV_LABEL_LONG_WRAP);

    //Write style for screen_contrl_label_5, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_contrl_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_contrl_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_contrl_label_5, lv_color_hex(0x00ff08), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_contrl_label_5, &lv_font_SourceHanSerifSC_Regular_26, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_contrl_label_5, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_contrl_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_contrl_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_contrl_label_5, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_contrl_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_contrl_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_contrl_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_contrl_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_contrl_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_contrl_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_contrl_label_6
    ui->screen_contrl_label_6 = lv_label_create(ui->screen_contrl);
    lv_obj_set_pos(ui->screen_contrl_label_6, 29, 389);
    lv_obj_set_size(ui->screen_contrl_label_6, 120, 28);
    lv_label_set_text(ui->screen_contrl_label_6, "座椅调节:");
    lv_label_set_long_mode(ui->screen_contrl_label_6, LV_LABEL_LONG_WRAP);

    //Write style for screen_contrl_label_6, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_contrl_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_contrl_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_contrl_label_6, lv_color_hex(0x00ff08), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_contrl_label_6, &lv_font_SourceHanSerifSC_Regular_26, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_contrl_label_6, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_contrl_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_contrl_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_contrl_label_6, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_contrl_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_contrl_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_contrl_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_contrl_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_contrl_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_contrl_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_contrl_label_7
    ui->screen_contrl_label_7 = lv_label_create(ui->screen_contrl);
    lv_obj_set_pos(ui->screen_contrl_label_7, 3, 268);
    lv_obj_set_size(ui->screen_contrl_label_7, 144, 28);
    lv_label_set_text(ui->screen_contrl_label_7, "氛围灯调节:");
    lv_label_set_long_mode(ui->screen_contrl_label_7, LV_LABEL_LONG_WRAP);

    //Write style for screen_contrl_label_7, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_contrl_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_contrl_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_contrl_label_7, lv_color_hex(0x00ff08), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_contrl_label_7, &lv_font_SourceHanSerifSC_Regular_26, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_contrl_label_7, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_contrl_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_contrl_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_contrl_label_7, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_contrl_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_contrl_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_contrl_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_contrl_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_contrl_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_contrl_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_contrl_slider_ws2812b
    ui->screen_contrl_slider_ws2812b = lv_slider_create(ui->screen_contrl);
    lv_obj_set_pos(ui->screen_contrl_slider_ws2812b, 154, 279);
    lv_obj_set_size(ui->screen_contrl_slider_ws2812b, 160, 8);
    lv_slider_set_range(ui->screen_contrl_slider_ws2812b, 0, 255);
    lv_slider_set_mode(ui->screen_contrl_slider_ws2812b, LV_SLIDER_MODE_NORMAL);
    lv_slider_set_value(ui->screen_contrl_slider_ws2812b, 0, LV_ANIM_OFF);

    //Write style for screen_contrl_slider_ws2812b, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_contrl_slider_ws2812b, 60, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_contrl_slider_ws2812b, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_contrl_slider_ws2812b, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_contrl_slider_ws2812b, 50, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_outline_width(ui->screen_contrl_slider_ws2812b, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_contrl_slider_ws2812b, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_contrl_slider_ws2812b, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_contrl_slider_ws2812b, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_contrl_slider_ws2812b, lv_color_hex(0x2195f6), LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_contrl_slider_ws2812b, LV_GRAD_DIR_NONE, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_contrl_slider_ws2812b, 50, LV_PART_INDICATOR|LV_STATE_DEFAULT);

    //Write style for screen_contrl_slider_ws2812b, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_contrl_slider_ws2812b, 255, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_contrl_slider_ws2812b, lv_color_hex(0x2195f6), LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_contrl_slider_ws2812b, LV_GRAD_DIR_NONE, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_contrl_slider_ws2812b, 50, LV_PART_KNOB|LV_STATE_DEFAULT);

    //The custom code of screen_contrl.


    //Update current screen layout.
    lv_obj_update_layout(ui->screen_contrl);

    //Init events for screen.
    events_init_screen_contrl(ui);
}
