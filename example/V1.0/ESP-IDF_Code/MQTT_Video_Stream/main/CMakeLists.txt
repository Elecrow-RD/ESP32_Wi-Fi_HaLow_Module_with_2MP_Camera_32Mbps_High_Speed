set(src
    "src/mm_app_loadconfig.c"
    "src/mm_app_common.c"
    "src/mqtt.c"
    "src/mqtt_pic_client.c")
set (inc
    "src")

idf_component_register(SRCS ${src}
                        INCLUDE_DIRS ${inc}
                        PRIV_REQUIRES morselib mm_shims mmipal mmiperf lwip mqtt json nvs_flash esp_event esp32-camera )
