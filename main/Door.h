/*
 * Door.h
 *
 *  Created on: Feb 8, 2024
 *      Author: Renan Augusto Starke
 */

#ifndef MAIN_DOOR_H_
#define MAIN_DOOR_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "driver/gpio.h"

class Door {
public:
	Door();
	~Door();

	void open();

private:
	const gpio_num_t door_GPIO = GPIO_NUM_13;

	SemaphoreHandle_t xSemaphore_door;
};

#endif /* MAIN_DOOR_H_ */
