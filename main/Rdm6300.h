/*
 * Rdm6300.h
 *
 *  Created on: Jul 28, 2022
 *      Author: Renan Augusto Starke
 */

#ifndef MAIN_RDM6300_H_
#define MAIN_RDM6300_H_

#include "Uart.h"

class Rdm6300 : public Uart {
public:
	Rdm6300(int baud_rate, uart_word_length_t data_bits,
			uart_parity_t parity, uart_stop_bits_t stop_bits,
			uart_hw_flowcontrol_t flow_cotrol);


private:
	//vitual
};

#endif /* MAIN_RDM6300_H_ */
