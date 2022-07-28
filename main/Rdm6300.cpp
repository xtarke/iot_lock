/*
 * Rdm6300.cpp
 *
 *  Created on: Jul 28, 2022
 *      Author: Renan Augusto Starke
 */

#include "Rdm6300.h"

Rdm6300::Rdm6300(int baud_rate, uart_word_length_t data_bits,
		uart_parity_t parity, uart_stop_bits_t stop_bits,
		uart_hw_flowcontrol_t flow_cotrol) :
		Uart(baud_rate, data_bits, parity, stop_bits, flow_cotrol) {


}

