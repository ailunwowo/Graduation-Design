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

// void map_display(lv_ui *ui)
// {
//   //lv_obj_set_size(screen, 250, 250);   // 每张瓦片是这个宽高参数		
//   ui->map_img = lv_img_create(ui->screen_map);
//   lv_img_set_src(ui->map_img , "A:/root/control_device/3338.bin");
//  // lv_obj_set_size(map_img , 250, 250);
// }

void setup_scr_screen_map(lv_ui *ui)
{
    //Write codes screen_map
    ui->screen_map = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_map, 720, 480);
    lv_obj_set_scrollbar_mode(ui->screen_map, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_map, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_map, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_src(ui->screen_map, &_version_RGB565A8_720x480, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_opa(ui->screen_map, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_recolor_opa(ui->screen_map, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_map_label_1
    ui->screen_map_label_1 = lv_label_create(ui->screen_map);
    lv_obj_set_pos(ui->screen_map_label_1, 178, 0);
    lv_obj_set_size(ui->screen_map_label_1, 364, 62);
    lv_label_set_text(ui->screen_map_label_1, "地图");
    lv_label_set_long_mode(ui->screen_map_label_1, LV_LABEL_LONG_WRAP);

    //Write style for screen_map_label_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_map_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_map_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_map_label_1, lv_color_hex(0xf00000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_map_label_1, &lv_font_SourceHanSerifSC_Regular_46, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_map_label_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_map_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_map_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_map_label_1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_map_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_map_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_map_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_map_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_map_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_map_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_map_btn_1
    ui->screen_map_btn_1 = lv_button_create(ui->screen_map);
    lv_obj_set_pos(ui->screen_map_btn_1, 600, 418);
    lv_obj_set_size(ui->screen_map_btn_1, 100, 50);
    ui->screen_map_btn_1_label = lv_label_create(ui->screen_map_btn_1);
    lv_label_set_text(ui->screen_map_btn_1_label, "返回");
    lv_label_set_long_mode(ui->screen_map_btn_1_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_map_btn_1_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_map_btn_1, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_map_btn_1_label, LV_PCT(100));

    //Write style for screen_map_btn_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_map_btn_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_map_btn_1, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_map_btn_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_map_btn_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_map_btn_1, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_map_btn_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_map_btn_1, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_map_btn_1, &lv_font_SourceHanSerifSC_Regular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_map_btn_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_map_btn_1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //The custom code of screen_map.

    ui->map_img = lv_img_create(ui->screen_map);
    lv_img_set_src(ui->map_img , "A:/root/control_device/3338.bin");
    lv_obj_center(ui->map_img);
    //Update current screen layout.
    lv_obj_update_layout(ui->screen_map);

    //Init events for screen.
    events_init_screen_map(ui);
}
