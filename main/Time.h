/*
 * Time.h
 *
 *  Created on: Aug 16, 2022
 *      Author: Renan Augusto Starke
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
