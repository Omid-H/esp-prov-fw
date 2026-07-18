/*
 * SPDX-FileCopyrightText: 2026 Nariman Ziaie
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Based on Espressif Systems network provisioning examples.
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_system.h"

#include "wifi_prov.h"

static const char *TAG = "main";

// Use the built-in BOOT button (GPIO 0) on most ESP32 dev boards
#define FACTORY_RESET_BUTTON_PIN GPIO_NUM_0

// Define the GPIO pin for the LED (GPIO 2 is common for onboard LEDs)
#define BLINK_GPIO GPIO_NUM_2

// Clock tick timing (Must add up to 1000ms for a perfect 1-second interval)
#define TICK_ON_DURATION_MS  100   // Short, sharp flash
#define TICK_OFF_DURATION_MS 900   // Remaining time of the second

// Function Declaration
void factory_reset_task(void *pvParameters);
static void configure_led(void);

// The linker needs extern "C" so the ESP-IDF bootloader can bind the entry point
extern "C" void app_main(void)
{
    ESP_LOGI(TAG, "System Booted. Launching Provisioning Component...");
    
    // Spawn the factory reset monitor as a background FreeRTOS task
    // It runs independently of your main logic.
    xTaskCreate(factory_reset_task, "factory_reset_task", 2048, NULL, 10, NULL);

    // Call your Wi-Fi Provisioning initializer (this will block until connected)
    ESP_LOGI(TAG, "Launching Provisioning Component...");
    init_wifi_provisioning();

    ESP_LOGI(TAG, "Network Link Up! Entering core application logic...");

    // Configure the GPIO pin
    configure_led();

    while (true) {
        ESP_LOGI(TAG, "Tick!");
        // Turn the LED ON for a brief moment
        gpio_set_level(BLINK_GPIO, 1);
        vTaskDelay(pdMS_TO_TICKS(TICK_ON_DURATION_MS));

        // Turn the LED OFF for the rest of the second
        gpio_set_level(BLINK_GPIO, 0);
        vTaskDelay(pdMS_TO_TICKS(TICK_OFF_DURATION_MS));
    }
}

// Task to monitor the reset button in the background
void factory_reset_task(void *pvParameters)
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

    ESP_LOGI(TAG, "Factory reset task started. Hold BOOT button for 3s to wipe device.");

    while (1)
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

// Function to configure the LED GPIO pin
static void configure_led(void)
{
    ESP_LOGI(TAG, "Configuring GPIO LED for clock tick style...");
    ESP_ERROR_CHECK(gpio_reset_pin(BLINK_GPIO)); 
    ESP_ERROR_CHECK(gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT));
}