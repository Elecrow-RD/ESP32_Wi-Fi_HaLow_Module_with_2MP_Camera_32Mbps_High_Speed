#include <stdio.h>
#include <stdbool.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"
#include "esp_log.h"

// GPIO definition
#define LED_GPIO       19
#define BUTTON_GPIO    20

static const char *TAG = "LED_Button";

void app_main(void)
{
    // ==============================
    // 1. Configure LED GPIO
    // ==============================
    gpio_config_t led_conf = {
        .pin_bit_mask = (1ULL << LED_GPIO),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };

    gpio_config(&led_conf);

    // ==============================
    // 2. Configure Button GPIO
    // ==============================
    gpio_config_t btn_conf = {
        .pin_bit_mask = (1ULL << BUTTON_GPIO),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_ENABLE, // Internal pull-down
        .intr_type = GPIO_INTR_DISABLE,
    };

    gpio_config(&btn_conf);

    ESP_LOGI(TAG, "System initialized");

    // LED state
    bool led_state = false;

    while (1)
    {
        // Read button state
        int button_level = gpio_get_level(BUTTON_GPIO);

        // Detect button press
        if (button_level == 1)
        {
            // ==============================
            // Debounce
            // ==============================
            vTaskDelay(pdMS_TO_TICKS(20));

            // Confirm button still pressed
            if (gpio_get_level(BUTTON_GPIO) == 1)
            {
                // Toggle LED state
                led_state = !led_state;

                // Set LED output
                gpio_set_level(LED_GPIO, led_state);

                ESP_LOGI(TAG,
                         "LED %s",
                         led_state ? "ON" : "OFF");

                // Wait for button release
                while (gpio_get_level(BUTTON_GPIO) == 1)
                {
                    vTaskDelay(pdMS_TO_TICKS(10));
                }

                // Release debounce
                vTaskDelay(pdMS_TO_TICKS(20));
            }
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
