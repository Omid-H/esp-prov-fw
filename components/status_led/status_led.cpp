/*
 * SPDX-FileCopyrightText: 2026 Nariman Ziaie
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Based on Espressif Systems network provisioning examples.
 */

#include "status_led.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

// Define the GPIO pin for the LED (GPIO 2 is common for onboard LEDs)
#define BLINK_GPIO GPIO_NUM_2

static const char *TAG = "status_led";

// Shared state indicator (Volatile to ensure background task reads fresh memory)
static volatile led_status_t g_led_status = LED_STATE_PROVISIONING;

extern "C" void set_led_status(led_status_t status)
{
    g_led_status = status;
    ESP_LOGI(TAG, "LED status updated to: %d", status);
}

// Function to configure the LED GPIO pin
static void configure_led(void)
{
    ESP_LOGI(TAG, "Configuring GPIO LED for dynamic status updates...");
    ESP_ERROR_CHECK(gpio_reset_pin(BLINK_GPIO)); 
    ESP_ERROR_CHECK(gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT));
}

// Background task to blink the status LED dynamically based on current state
static void led_status_task(void *pvParameters)
{
    while (true)
    {
        switch (g_led_status)
        {
        case LED_STATE_PROVISIONING:
            // Fast blink: 100ms ON / 100ms OFF
            gpio_set_level(BLINK_GPIO, 1);
            vTaskDelay(pdMS_TO_TICKS(100));
            gpio_set_level(BLINK_GPIO, 0);
            vTaskDelay(pdMS_TO_TICKS(100));
            break;
        case LED_STATE_CONNECTING:
            // Medium blink: 500ms ON / 500ms OFF
            gpio_set_level(BLINK_GPIO, 1);
            vTaskDelay(pdMS_TO_TICKS(500));
            gpio_set_level(BLINK_GPIO, 0);
            vTaskDelay(pdMS_TO_TICKS(500));
            break;
        case LED_STATE_CONNECTED:
            // Slow heartbeat: 100ms ON / 1900ms OFF
            gpio_set_level(BLINK_GPIO, 1);
            vTaskDelay(pdMS_TO_TICKS(100));
            gpio_set_level(BLINK_GPIO, 0);
            vTaskDelay(pdMS_TO_TICKS(1900));
            break;
        case LED_STATE_ERROR:
            // Double-blink: 100ms ON / 100ms OFF / 100ms ON / 700ms OFF
            gpio_set_level(BLINK_GPIO, 1);
            vTaskDelay(pdMS_TO_TICKS(100));
            gpio_set_level(BLINK_GPIO, 0);
            vTaskDelay(pdMS_TO_TICKS(100));
            gpio_set_level(BLINK_GPIO, 1);
            vTaskDelay(pdMS_TO_TICKS(100));
            gpio_set_level(BLINK_GPIO, 0);
            vTaskDelay(pdMS_TO_TICKS(700));
            break;
        }
    }
}

extern "C" void init_status_led(void)
{
    configure_led();
    xTaskCreate(led_status_task, "led_status_task", 2048, NULL, 2, NULL);
}

