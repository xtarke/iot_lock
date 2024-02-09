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
#include "freertos/queue.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "driver/gpio.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#include "Wifi.h"
#include "Mqtt.h"

#include "Rdm6300.h"
#include "Tags.h"
#include "Door.h"



static void door_button_task(void* arg)
{

	Door *my_door = (Door *)arg;

	gpio_reset_pin(GPIO_NUM_23);
	/* Set the GPIO as a push/pull output */
	gpio_set_direction(GPIO_NUM_23, GPIO_MODE_INPUT);
	gpio_set_pull_mode(GPIO_NUM_23, GPIO_PULLUP_ONLY);
	gpio_pullup_en(GPIO_NUM_23);

	int level = 0;

	TickType_t currentTime;
	TickType_t openedTime = 0;

    for(;;) {
    	level = gpio_get_level(GPIO_NUM_23);
    	vTaskDelay(700 /  portTICK_PERIOD_MS);

    	/* Get the time in MS. */
		currentTime = pdTICKS_TO_MS( xTaskGetTickCount() );

    	if (level){

    		/* Re open door after 10s */
    		if (currentTime > openedTime + 10000){
    			ESP_LOGI("door_button_task::", "Open door for button");
    			my_door->open();
    			openedTime = currentTime;

    			char *string = "open";

    			mqtt5_publish("lpae/button_log",string);
    		}
    	}
    }
}


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
	/* Door */
	Door my_door;

	xTaskCreate(door_button_task, "door_button_task", 2048, (void *)&my_door, 10, NULL);

	while (1){
		/* Wait for a new tag */
		uint32_t tag = tag_sensor.WaitAndRead();

		char string[64];
		snprintf(string,64,"%ld",tag);

		/* Check if a read tag is in permissive list */
		if ((tag != 0) && (tags_storage.search(tag) != -1)) {
			ESP_LOGI("Main::", "Open door for: %lu", tag);
			my_door.open();

			mqtt5_publish("lpae/tag_open",string);
		}
		else{
			mqtt5_publish("lpae/tag_denied",string);
		}
	}
}
