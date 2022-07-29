/*
 * Rdm6300.cpp
 *
 *  Created on: Jul 28, 2022
 *      Author: Renan Augusto Starke
 */

#include <string.h>

#include "esp_log.h"


#include "Rdm6300.h"


/*
 * @brief	Rdm6300 initialization.
 * @param	baud rate, data bits, parity, stop bits and flow control. See driver/uart.h.
 *
 * @retval None.
 */
Rdm6300::Rdm6300(int baud_rate, uart_word_length_t data_bits,
		uart_parity_t parity, uart_stop_bits_t stop_bits,
		uart_hw_flowcontrol_t flow_cotrol) :
		Uart(baud_rate, data_bits, parity, stop_bits, flow_cotrol) {

	memset(Rdm6300::data,0,sizeof(Rdm6300::data));
}

/*
 * @brief	ReadBytes from Rdm6300 sensor using UART interface. ReadBytes should block until data is received
 * @param	None
 *
 * @retval number of bytes received.
 */
int Rdm6300::WaitAndRead(void){
	return Rdm6300::ReadBytes(data, 5);
}



void Rdm6300::Print(void){

	for (int i=0; i < 5;i++)
		ESP_LOGI("Rdm6300::", "data[%d] = %x", i, Rdm6300::data[i]);

}

