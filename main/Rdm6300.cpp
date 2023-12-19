/* Copyright (c) 2023 Renan Augusto Starke
 *
 * This file is part of project "IoT Lock".
 * 
 */

/**
 * @file Rdm6300.cpp
 * @author Renan Augusto Starke
 * @date 28 Jul 2022
 * @brief File containing Rdm6300 class implementation.
 *
 */

#include <string.h>
#include "esp_log.h"
#include "Rdm6300.h"

/*
 * @brief	Construct a new Rdm6300::Rdm6300 object Rdm6300.
 * @param	Baud rate, data bits, parity, stop bits and flow control. See driver/uart.h.
 *
 * @retval None.
 */
Rdm6300::Rdm6300(int baud_rate, uart_word_length_t data_bits,
		uart_parity_t parity, uart_stop_bits_t stop_bits,
		uart_hw_flowcontrol_t flow_cotrol) :
		Uart(baud_rate, data_bits, parity, stop_bits, flow_cotrol) {

	memset(Rdm6300::data,0,sizeof(Rdm6300::data));

	tag = 0;
	checksum = 0;
	msg_checkum = 0;
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
	uint32_t my_time = 0;

	Uart::flush();
	/* Read, search and flush serial data */
	len = Rdm6300::ReadBytes(data, 64);
	for (head_index=0; head_index < (sizeof(data) - 14); head_index++){
		if (data[head_index] == 0x02)
			break;
	}

#ifdef DEBUG
	Print();
#endif

	/* Invalid head */
	if (data[head_index] != 0x02)
		return -1;
	else if (check_checksum(head_index) == false)
		return -1;

	/* Add string termination character */
	Rdm6300::data[head_index + 11] = 0x00;

	/* Convert tag. Ignore version: 2 chars after head */
	Rdm6300::tag  = strtol((char *)data + head_index + 3, NULL, 16);

	ESP_LOGI("Rdm6300::", "tag = %lu  msg_checksum = %x  cal_checksum = %x  time: %lu", tag, msg_checkum, checksum, my_time);

	/* Should suspend since Rdm6300 keep sending data while a tag is next to it */
	Time::Suspend(idle_ticks);

	return Rdm6300::tag;
}


/**
 * @brief Print received data from Rdm6300.
 * 
 */
void Rdm6300::Print(void){
	for (int i=0; i < sizeof(data);i++)
		ESP_LOGI("Rdm6300::", "data[%d] = %d", i, Rdm6300::data[i]);
}

/**
 * @brief Check checksum.
 * 
 * @param index 
 * @return true 
 * @return false 
 */
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

