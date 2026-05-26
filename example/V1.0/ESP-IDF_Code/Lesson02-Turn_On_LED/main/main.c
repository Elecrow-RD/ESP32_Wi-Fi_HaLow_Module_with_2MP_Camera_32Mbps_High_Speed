#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"

// Define the GPIO pin number used
#define BLINK_GPIO 19

static const char *TAG = "Blink_Example";

void app_main(void)
{
    // 1. Configure GPIO parameters
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << BLINK_GPIO),   // Select GPIO 19
        .mode = GPIO_MODE_OUTPUT,               // Set as output mode
        .pull_up_en = GPIO_PULLUP_DISABLE,      // Disable pull-up
        .pull_down_en = GPIO_PULLDOWN_DISABLE,  // Disable pull-down
        .intr_type = GPIO_INTR_DISABLE,         // Disable interrupt
    };
    
    // Apply configuration
    gpio_config(&io_conf);

    uint8_t s_led_state = 1; // Start with 1 to test inverted logic (LED may be active low)
    ESP_LOGI(TAG, "GPIO %d configured successfully, start blinking...", BLINK_GPIO);

    while (1) {
        // 2. Set GPIO level (using inverted logic test)
        gpio_set_level(BLINK_GPIO, s_led_state);
        
        ESP_LOGI(TAG, "GPIO level: %d (%s)", s_led_state, s_led_state ? "HIGH" : "LOW");
        
        // Toggle state
        s_led_state = !s_led_state;

        // 3. Delay (unit: FreeRTOS tick)
        vTaskDelay(pdMS_TO_TICKS(500)); 
    }
}
