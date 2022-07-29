/* WiFi iot door lock

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
   */
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#include "Wifi.h"

#include "Rdm6300.h"

extern "C" void app_main(void)
{

	//Initialize NVS
	esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);

	Wifi::Init();


	Rdm6300 teste(9600,UART_DATA_8_BITS,UART_PARITY_DISABLE,UART_STOP_BITS_1, UART_HW_FLOWCTRL_DISABLE);


	while (1){
		teste.WaitAndRead();
		teste.Print();
	}


}
