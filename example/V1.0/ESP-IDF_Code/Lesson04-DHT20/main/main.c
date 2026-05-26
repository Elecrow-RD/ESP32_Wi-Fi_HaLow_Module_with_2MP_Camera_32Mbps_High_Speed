#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "dht20.h"

static const char *TAG = "DHT20_APP";

void app_main(void)
{
    dht20_data_t measurement;

    dht20_begin();

    while (!dht20_is_calibrated()) {
        ESP_LOGI(TAG, "Waiting for DHT20 calibration...");
        vTaskDelay(pdMS_TO_TICKS(100));
    }

    ESP_LOGI(TAG, "DHT20 initialized on I2C pins SDA=45, SCL=42");

    while (1) {
        float status = dht20_read_data(&measurement);
        if (status == ESP_OK) {
            printf("Temperature: %.2f C, Humidity: %.2f %%\r\n",
                   measurement.temperature,
                   measurement.humidity);
        } else {
            ESP_LOGE(TAG, "DHT20 read failed (%d)", (int)status);
        }
        vTaskDelay(pdMS_TO_TICKS(1500));
    }
}
