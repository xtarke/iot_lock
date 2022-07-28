
/*
 * Wifi.hpp
 *
 *  Created on: Jul 19, 2022
 *      Author: Renan Augusto Starke
 *
 *      Instituto Federal de Santa Catarina
 */
#ifndef _WIFI_H
#define _WIFI_H

#include <stdint.h>
#include "freertos/event_groups.h"
#include "esp_event.h"


/*  WiFi configuration via project configuration menu */
#define EXAMPLE_ESP_WIFI_SSID      CONFIG_ESP_WIFI_SSID
#define EXAMPLE_ESP_WIFI_PASS      CONFIG_ESP_WIFI_PASSWORD
#define EXAMPLE_ESP_MAXIMUM_RETRY  CONFIG_ESP_MAXIMUM_RETRY

/* The event group allows multiple bits for each event, but we only care about two events:
 * - we are connected to the AP with an IP
 * - we failed to connect after the maximum amount of retries */
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

#ifdef __cplusplus // only actually define the class if this is C++

namespace Wifi {
	void Init(void);
	};

#endif

#ifdef __cplusplus
    #define EXPORT_C extern "C"
#else
    #define EXPORT_C
#endif

#endif /* _WIFI_H */


