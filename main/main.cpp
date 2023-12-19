/*
 * Copyright (c) 2023 Renan Augusto Starke
 *
 * This file is part of project "IoT Lock".
 * 
 */

/**
 * @file main.cpp
 * @author Renan Augusto Starke
 * @date 28 Jul 2022
 * @brief  File containing main function (FreeRTOS main thread)
 *
 */

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_log.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#include "Wifi.h"
#include "Mqtt.h"

#include "Rdm6300.h"
#include "Tags.h"

/**
 * @brief Main function (main FreeRTOS thread). Initialize hardware and runs the main loop.
 * 
 * @return None 
 */

extern "C" void app_main(void)
{
	/* Initialize NVS */
	esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);

	/* Initialize WiFi and MQTT5*/
	Wifi::Init();
	mqtt5_init();

	/* RFID storage class */
	Tags tags_storage;
	/* RFID sensor class */
	Rdm6300 tag_sensor(9600,UART_DATA_8_BITS,UART_PARITY_DISABLE,UART_STOP_BITS_1, UART_HW_FLOWCTRL_DISABLE);

	while (1){
		/* Wait for a new tag */
		uint32_t tag = tag_sensor.WaitAndRead();

		/* Check if a read tag is in permissive list */
		if ((tag != 0) && (tags_storage.search(tag) != -1))
			ESP_LOGI("Main::", "Open door for: %lu", tag);

		//ToDo: Actually needs to open a door

	}
}
