/* Copyright (c) 2023 Renan Augusto Starke
 *
 * This file is part of project "IoT Lock".
 * 
 */

/**
 * @file Time.cpp
 * @author Renan Augusto Starke
 * @date 16 Aug 2022
 * @brief File containing Time class implementation.
 *
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "Time.h"

/**
 * @brief Construct a new Time:: Time object
 * 
 */

Time::Time() {

}

/**
 * @brief Return ticks from FreeRTOS. Calls SDK "get ticks" function.
 * 
 * @return uint32_t Ticks.
 */
uint32_t Time::GetTime(){

	TickType_t ticks = xTaskGetTickCount();

	return (uint32_t)ticks ;
}

/**
 * @brief Suspend task. Calls SDK delay function.
 * 
 * @param ticks 
 */
void Time::Suspend(uint32_t ticks){
	vTaskDelay(ticks);
}
