/*
 * Tags.cpp
 *
 *  Created on: Dec 18, 2023
 *      Author: xtarke
 */

#include "Tags.h"

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "driver/gpio.h"

#include "Mqtt.h"


#define STORAGE_NAMESPACE "taqs_storage"


void tags_task(void *param) {

	Tags *p = (Tags *)param;

	p->print();

	while (1){
		/* Block until a new tag is received from mqtt */
		uint32_t tag = get_added_tag();

		/* Add, or delete from NVS storage */
		p->add_new(tag);
		ESP_LOGI("Tags::", "MQTT add new tag: %lu", tag);
	}
}


Tags::Tags(){

	nvs_handle_t my_handle;
	esp_err_t err;

	memset(tags_memory, 0, sizeof(tags_memory));

	// Open
	err = nvs_open(STORAGE_NAMESPACE, NVS_READONLY, &my_handle);
	if (err != ESP_OK){
		ESP_LOGI("Tags::", "NVS open error: %x", err);
		nvs_err = err;
		return;
	}

	// Read the size of memory space required for blob
	size_t required_size = 0;  // value will default to 0, if not set yet in NVS
	err = nvs_get_blob(my_handle, "tags", NULL, &required_size);
	if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND)
	{
		ESP_LOGI("Tags::", "NVS nvs_get_blob error: %d", err);
		nvs_err = err;
		return;
	}

	ESP_LOGI("Tags::", "NVS open sucessfull. Size: %d", required_size);

	// Read previously saved blob if available
	//tags_memory = (uint32_t*)malloc(required_size + sizeof(uint32_t));
	tags_top_index = (required_size/sizeof(uint32_t));

	ESP_LOGI("Tags::", "tags_top_index: %lu", tags_top_index);

	if (required_size > 0) {

		if ((required_size / sizeof(uint32_t))  > Tags::MAX_TAGS)
			required_size = Tags::MAX_TAGS * sizeof(uint32_t);

		err = nvs_get_blob(my_handle, "tags", tags_memory, &required_size);
		if (err != ESP_OK) {
			nvs_err = err;
			return;
		}
	}

	nvs_close(my_handle);

	xSemaphore_tags = xSemaphoreCreateMutex();

	/* Create and send class instace to RTOS task */
	Tags *p = this;
	xTaskCreate(tags_task, "tags_task", 4096, p, 10, NULL);
}

int Tags::add_new(uint32_t tag){

	nvs_handle_t my_handle;
	esp_err_t err;

	/* First space to store new tag */
	int32_t new_index = find_space();
	int32_t found_idex = Tags::search(tag);

	/* Add new */
	xSemaphoreTake(xSemaphore_tags, portMAX_DELAY);
	if (found_idex == -1){
		if (new_index < 0){
			ESP_LOGI("Tags::", "No tag space left");
			return ESP_FAIL;
		}

		ESP_LOGI("Tags::", "Add new tag to index: %ld", new_index);
		tags_memory[new_index] = tag;
	}
	else {
		/* Remove a tag when added a found one */
		ESP_LOGI("Tags::", "Remove tag from index: %ld", found_idex);
		tags_memory[found_idex] = 0;
	}
	xSemaphoreGive(xSemaphore_tags);


	/* Commit to NVS storage */
	err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &my_handle);

	if (err != ESP_OK){
		ESP_LOGI("Tags::", "NVS open error: %x", err);
		nvs_err = err;
		return ESP_FAIL;
	}

	err = nvs_set_blob(my_handle, "tags", tags_memory, sizeof(tags_memory));
	if (err != ESP_OK){
		ESP_LOGI("Tags::", "NVS write error: %x", err);
		nvs_err = err;
		return ESP_FAIL;
	}
	err = nvs_commit(my_handle);

	nvs_close(my_handle);



	return ESP_OK;
}

int32_t Tags::find_space(){

	for (int i=0;i < Tags::MAX_TAGS; i++)
		if (tags_memory[i] == 0)
			return i;

	return -1;
}


int32_t Tags::search(uint32_t tag){

	int32_t ret = -1;

	xSemaphoreTake(xSemaphore_tags, portMAX_DELAY);
	for (int i=0; i < Tags::MAX_TAGS; i++)
	{
		if (tag == tags_memory[i]){
			ret = i;
			break;
		}
	}
	xSemaphoreGive(xSemaphore_tags);

	return ret;
}

void Tags::print(){

	for (int i=0; i < Tags::MAX_TAGS; i++)
	{
		ESP_LOGI("Tags::", "Stored tag: %lu", tags_memory[i]);
	}
}


