/*
 * Mqtt.h
 *
 *  Created on: Dec 14, 2023
 *      Author: xtarke
 */

#ifndef MAIN_MQTT_H_
#define MAIN_MQTT_H_


#ifdef __cplusplus // only actually define the class if this is C++

namespace Mqtt {
	void Init(void);
	};

#endif


#ifdef __cplusplus
    #define EXPORT_C extern "C"
#else
    #define EXPORT_C
#endif


#endif /* MAIN_MQTT_H_ */
