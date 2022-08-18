/*
 * Time.cpp
 *
 *  Created on: Aug 16, 2022
 *      Author: Renan Augusto Starke
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "Time.h"

Time::Time() {
	// TODO Auto-generated constructor stub

}


uint32_t Time::GetTime(){

	TickType_t ticks = xTaskGetTickCount();

	return (uint32_t)ticks ;
}

void Time::Suspend(uint32_t ticks){
	vTaskDelay(ticks);
}
