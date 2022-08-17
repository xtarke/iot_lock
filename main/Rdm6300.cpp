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
	last_tag_time = GetTime();
}

/*
 * @brief	ReadBytes from Rdm6300 sensor using UART interface. ReadBytes should block until data is received
 * @param	None
 *
 * @retval number of bytes received.
 */
uint32_t Rdm6300::WaitAndRead(void){

	int len = 0;
	int head_index = 0;
	uint32_t my_time;
	uint32_t my_tag;


	memset(data,0,sizeof(data));

	size_t size;

	uart_get_buffered_data_len(2,&size);
	ESP_LOGI("Rdm6300::", "size %d", size);

	//Rdm6300::ReadBytes(data, (int)size);

	Uart::flush();
	uart_get_buffered_data_len(2,&size);
	ESP_LOGI("Rdm6300::", "size %d", size);


	/* Read, search and flush serial data */
	len = Rdm6300::ReadBytes(data, 64);
	for (head_index=32; head_index < (sizeof(data) - 14); head_index++){
		if (data[head_index] == 0x02)
			break;
	}

#ifdef DEBUG
	Print();
#endif

	ESP_LOGI("Rdm6300::", "head_index %d", head_index);


	/* Invalid head */
	if (data[head_index] != 0x02)
		return -1;
	else if (check_checksum(head_index) == false)
		return -1;

	/* Add string termination character */
	Rdm6300::data[head_index + 11] = 0x00;

	/* Convert tag. Ignore version: 2 chars after head */
	my_tag = strtol((char *)data + head_index + 3, NULL, 16);

	my_time = GetTime();

	if (my_time  <  last_tag_time + idle_ticks){
#ifdef DEBUG
		ESP_LOGI("Rdm6300::", "%d   %d", my_time, last_tag_time + idle_ticks);
#endif
		Uart::flush();
		return -1;
	}

	Rdm6300::last_tag_time = my_time;
	Rdm6300::tag = my_tag;

	ESP_LOGI("Rdm6300::", "tag = %d  msg_checksum = %x  cal_checksum = %x  time: %d", tag, msg_checkum, checksum, my_time);
	ESP_LOGI("Rdm6300::", "-------------------------");

	return Rdm6300::tag;
}



void Rdm6300::Print(void){

	for (int i=0; i < sizeof(data);i++)
		ESP_LOGI("Rdm6300::", "data[%d] = %d", i, Rdm6300::data[i]);

}


bool Rdm6300::check_checksum(int index){
	char byte[3] = {0};

	/* Data check sum */
	byte[0] = data[index + 11];
	byte[1] = data[index + 12];
	Rdm6300::msg_checkum = strtol((char *)byte, NULL, 16);

	Rdm6300::checksum = 0;
	/* Calc checksum */
	for (int i=1; i < 11; i+=2){
		byte[0] = data[index + i];
		byte[1] = data[index + 1 + i];
		byte[3] = 0;

		uint8_t val = strtol((char *)byte, NULL, 16);

#ifdef DEBUG
		ESP_LOGI("Rdm6300::", "byte[%d] = %x", i, val);
#endif

		Rdm6300::checksum ^= val;
	}

	if (checksum == msg_checkum)
		return true;
	else
		return false;
}

