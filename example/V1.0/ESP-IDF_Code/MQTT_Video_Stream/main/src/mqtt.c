/*
 * Copyright 2022-2023 Morse Micro
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief Throughput measurement using iperf.
 *
 * The Iperf parameters are specified using the defines in the file. Additional defines in
 * @c mm_app_loadconfig.c and @c mm_app_common.c are used to configure the network stack and WLAN
 * interface.
 *
 * @note It is assumed that you have followed the steps in the @ref GETTING_STARTED guide and are
 * therefore familiar with how to build, flash, and monitor an application using the MM-IoT-SDK
 * framework.
 *
 * This file demonstrates how to run iperf using the Morse Micro WLAN API.
 */

#include <endian.h>
#include <string.h>

#include "mmosal.h"
#include "mmwlan.h"
#include "mmipal.h"

#include "mm_app_common.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_camera.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

static const char *TAG = "MAIN";

#define CONFIG_XCLK_FREQ 20000000 

void mqtt_app_start(void);

static esp_err_t init_camera(void)
{
    camera_config_t camera_cfg = {
        .pin_pwdn = -1,
        .pin_reset = -1,
        .pin_xclk = 47,
        .pin_sccb_sda = 45,
        .pin_sccb_scl = 42,

        .pin_d7 = 38,
        .pin_d6 = 48,
        .pin_d5 = 46,
        .pin_d4 = 18,
        .pin_d3 = 14,
        .pin_d2 = 12,
        .pin_d1 = 13,
        .pin_d0 = 17,
        .pin_vsync = 40,
        .pin_href = 39,
        .pin_pclk = 21,

        .xclk_freq_hz = CONFIG_XCLK_FREQ,
        .ledc_timer = LEDC_TIMER_0,
        .ledc_channel = LEDC_CHANNEL_0,

        .pixel_format = PIXFORMAT_JPEG,
        .frame_size = FRAMESIZE_HVGA,
        .jpeg_quality = 8,
        .fb_count = 2,
        .grab_mode = CAMERA_GRAB_LATEST,
        .fb_location = CAMERA_FB_IN_PSRAM,
     
    };
    esp_err_t ret = esp_camera_init(&camera_cfg);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Camera Init Failed");
        return ESP_FAIL;
    }

    sensor_t *s = esp_camera_sensor_get();
    s->set_vflip(s, 0);
    if (s->id.PID == OV3660_PID)
    {
        s->set_saturation(s, -2);
    }
    
    if (s->id.PID == OV2640_PID || s->id.PID == OV3660_PID)
    {
        s->set_vflip(s, 0);
    }
    else if (s->id.PID == GC0308_PID)
    {
        s->set_hmirror(s, 0);
    }
    else if (s->id.PID == GC032A_PID)
    {
        s->set_vflip(s, 0);
    }

    return ret;
}

void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    app_wlan_init();
    app_wlan_start();

    if (init_camera() != ESP_OK)
    {
        return;
    }

    mqtt_app_start();
}
