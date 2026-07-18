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
#include "sdkconfig.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_system.h"

#include "wifi_prov.h"
#include "status_led.h"
#include "factory_reset.h"

static const char *TAG = "main";

extern "C" void app_main(void)
{
    ESP_LOGI(TAG, "System Booted. Launching Dynamic Status LED...");
    // Initialize the status LED component
    init_status_led();

    ESP_LOGI(TAG, "Launching Factory Reset Monitor...");
    // Spawn the factory reset monitor background task
    init_factory_reset_monitor();

    // Call your Wi-Fi Provisioning initializer (this will block until connected)
    ESP_LOGI(TAG, "Launching Provisioning Component...");
    init_wifi_provisioning();

    ESP_LOGI(TAG, "Network Link Up! Core application initialized successfully.");

    // Delete the calling app_main startup task to free stack memory resources.
    vTaskDelete(NULL);
}