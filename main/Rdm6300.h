/*
 * Copyright (c) 2023 Renan Augusto Starke
 *
 * This file is part of project "IoT Lock".
 * 
 */

/**
 * @file Rdm6300.h
 * @author Renan Augusto Starke
 * @date 28 Jul 2023
 * @brief File containing Rdm6300 class definiton.
 *
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

	uint32_t tag;
	uint8_t checksum;
	uint8_t msg_checkum;

	bool check_checksum(int index);
	uint8_t data[64];

};

#endif /* MAIN_RDM6300_H_ */
