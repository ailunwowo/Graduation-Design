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

#include "rvc_test.h"

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
    
    // lf_system_init();
    // lf_system_ui();
    app_main();
    /*Handle LVGL tasks*/
    while(1) {
        lv_timer_handler();
        usleep(5000);
    }

    return 0;
}

