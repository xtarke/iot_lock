/*
 * Rdm6300.h
 *
 *  Created on: Jul 28, 2022
 *      Author: Renan Augusto Starke
 */

#ifndef MAIN_RDM6300_H_
#define MAIN_RDM6300_H_

#include "Uart.h"
#include "Time.h"

class Rdm6300 : public Uart, Time  {
public:
	Rdm6300(int baud_rate, uart_word_length_t data_bits,
			uart_parity_t parity, uart_stop_bits_t stop_bits,
			uart_hw_flowcontrol_t flow_cotrol);

	uint32_t WaitAndRead();

	void Print();

private:
	const uint32_t idle_ticks = 200;

	uint8_t checksum;
	uint8_t msg_checkum;
	uint32_t tag;

	bool check_checksum();
	uint8_t data[14];

	uint32_t last_tag_time;

};

#endif /* MAIN_RDM6300_H_ */
