/*
 * Uart.h
 *
 *  Created on: Jul 28, 2022
 *      Author: Renan Augusto Starke
 */

#ifndef MAIN_UART_H_
#define MAIN_UART_H_

#include <stdint.h>
#include "driver/uart.h"

class Uart {
public:
	Uart(int baud_rate, uart_word_length_t data_bits,
			uart_parity_t parity, uart_stop_bits_t stop_bits,
			uart_hw_flowcontrol_t flow_cotrol);

protected:
	int ReadBytes(uint8_t *data, uint32_t bytes_to_read);

private:
	enum {UART_PORT_NUM = 2, RXD_PIN = 16, TXD_PIN = 17, BUFFER_SIZE = 1024};



};

#endif /* MAIN_UART_H_ */
