/*
 * Door.cpp
 *
 *  Created on: Feb 8, 2024
 *      Author: Renan Augusto Starke
 */

#include "Door.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"


Door::Door() {
	gpio_reset_pin(door_GPIO);
	/* Set the GPIO as a push/pull output */
	gpio_set_direction(door_GPIO, GPIO_MODE_OUTPUT);
	/* Force level */
	gpio_set_level(door_GPIO, 0);

	xSemaphore_door = xSemaphoreCreateMutex();
}

Door::~Door() {
	// TODO Auto-generated destructor stub
}


void Door::open(){
	xSemaphoreTake(xSemaphore_door, portMAX_DELAY);
	gpio_set_level(door_GPIO, 1);
	vTaskDelay(100 /  portTICK_PERIOD_MS);
	gpio_set_level(door_GPIO, 0);
	xSemaphoreGive(xSemaphore_door);
}
