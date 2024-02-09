/*
 * Copyright (c) 2023 Renan Augusto Starke
 *
 * This file is part of project "IoT Lock".
 * 
 */

/**
 * @file Mqtt.h
 * @author Renan Augusto Starke
 * @date 14 Dec 2022
 * @brief File containing Mqtt namespace definitions.
 *
 */

#ifndef MAIN_MQTT_H_
#define MAIN_MQTT_H_

#ifdef __cplusplus // only actually define the class if this is C++


#endif

#ifdef __cplusplus
    #define EXPORT_C extern "C"
#else
    #define EXPORT_C
#endif

EXPORT_C uint32_t get_added_tag();
EXPORT_C void mqtt5_init(void);
EXPORT_C void mqtt5_publish(const char *topic, char *msg);


#endif /* MAIN_MQTT_H_ */
