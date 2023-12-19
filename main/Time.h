/*
 * Copyright (c) 2023 Renan Augusto Starke
 *
 * This file is part of project "IoT Lock".
 * 
 */

/**
 * @file Time.h
 * @author Renan Augusto Starke
 * @date 16 Aug 2022
 * @brief File containing Time class definiton.
 *
 */

#ifndef MAIN_TIME_H_
#define MAIN_TIME_H_

#include <stdint.h>

class Time {
public:
	Time();

	uint32_t GetTime();
	void Suspend(uint32_t ticks);

};

#endif /* MAIN_TIME_H_ */
