/*
 * Tasg.h
 *
 *  Created on: Dec 18, 2023
 *      Author: xtarke
 */

#ifndef MAIN_TAGS_H_
#define MAIN_TAGS_H_

#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_system.h"


#ifdef __cplusplus // only actually define the class if this is C++

class Tags{
public:
	Tags();
	int add_new(uint32_t tag);
	int32_t search(uint32_t tag);
	void print();

	enum {MAX_TAGS = 128};


private:
	uint32_t tags_memory[MAX_TAGS];
	int32_t tags_top_index;
	esp_err_t nvs_err;

	int32_t find_space();

	SemaphoreHandle_t xSemaphore_tags;



protected:



};

#endif

#ifdef __cplusplus
    #define EXPORT_C extern "C"
#else
    #define EXPORT_C
#endif

EXPORT_C void tags_task(void *param);

#endif /* MAIN_TAGS_H_ */
