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

	checksum = 0;
	msg_checkum = 0;
	tag = 0;
}

/*
 * @brief	ReadBytes from Rdm6300 sensor using UART interface. ReadBytes should block until data is received
 * @param	None
 *
 * @retval number of bytes received.
 */
int Rdm6300::WaitAndRead(void){

	int len = 0;

	len = Rdm6300::ReadBytes(data, 14);

	Print();

	/* Incorrect number of bytes */
	if (len != 14)
		return -1;
	/* Invalid head */
	else if (data[0] != 0x02)
		return -1;
	/* Invalid tail */
	else if (data[13] != 0x03)
		return -1;
	//else if (check_checksum() == false)
	//	return -1;

	check_checksum();

	/* Add string termination character */
	data[11] = 0x00;

	/* Convert tag. Ignore version: 2 chars after head */
	tag = strtol((char *)data + 3, NULL, 16);

	ESP_LOGI("Rdm6300::", "tag = %d  msg_checksum = %x  cal_checksum = %x", tag, msg_checkum, checksum);
	ESP_LOGI("Rdm6300::", "-------------------------");

	return Rdm6300::tag;
}



void Rdm6300::Print(void){

	for (int i=0; i < 14;i++)
		ESP_LOGI("Rdm6300::", "data[%d] = %d", i, Rdm6300::data[i]);

}


bool Rdm6300::check_checksum(){
	char byte[3] = {0};

	/* Data check sum */
	byte[0] = data[11];
	byte[1] = data[12];
	msg_checkum = strtol((char *)byte, NULL, 16);

	/* Calc checksum */
	for (int i=1; i < 11; i+=2){
		byte[0] = data[i];
		byte[1] = data[1+i];
		byte[3] = 0;

		uint8_t val = strtol((char *)byte, NULL, 16);

		checksum ^= val;
	}

	if (checksum == msg_checkum)
		return true;
	else
		return false;
}

