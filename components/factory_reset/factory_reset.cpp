/*
 * SPDX-FileCopyrightText: 2026 Nariman Ziaie
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Based on Espressif Systems network provisioning examples.
 */

#include "factory_reset.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_system.h"

#define FACTORY_RESET_BUTTON_PIN GPIO_NUM_0
static const char *TAG = "factory_reset";

static void factory_reset_task(void *pvParameters)
{
    // 1. Configure the GPIO pin
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;        // No interrupts, we will poll it
    io_conf.mode = GPIO_MODE_INPUT;               // Set as Input
    io_conf.pin_bit_mask = (1ULL << FACTORY_RESET_BUTTON_PIN);
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;      // Enable internal pull-up resistor
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    gpio_config(&io_conf);

    int hold_time_ms = 0;
    const int required_hold_time_ms = 3000; // 3 seconds to trigger reset

    ESP_LOGI(TAG, "Factory reset monitor task started. Hold BOOT button for 3s to wipe device.");

    while (true)
    {
        // GPIO 0 is pulled HIGH by default, and goes LOW (0) when the button is pressed
        if (gpio_get_level(FACTORY_RESET_BUTTON_PIN) == 0) 
        {
            hold_time_ms += 100; // Increment counter
            
            if (hold_time_ms % 1000 == 0) {
                ESP_LOGW(TAG, "Reset button held for %d seconds...", hold_time_ms / 1000);
            }

            // If held for 3 seconds, execute the wipe
            if (hold_time_ms >= required_hold_time_ms)
            {
                ESP_LOGE(TAG, "FACTORY RESET TRIGGERED! Wiping NVS and rebooting...");
                
                // Erase the entire NVS partition (wipes Wi-Fi credentials)
                ESP_ERROR_CHECK(nvs_flash_erase());
                
                // Give the serial console a moment to print the log before dying
                vTaskDelay(pdMS_TO_TICKS(500)); 
                
                // Reboot the ESP32
                esp_restart();
            }
        }
        else
        {
            // If button is released before 3 seconds, reset the counter
            if (hold_time_ms > 0) {
                ESP_LOGI(TAG, "Reset aborted.");
                hold_time_ms = 0;
            }
        }

        // Check the button state every 100 milliseconds
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

extern "C" void init_factory_reset_monitor(void)
{
    xTaskCreate(factory_reset_task, "factory_reset_task", 2048, NULL, 10, NULL);
}
