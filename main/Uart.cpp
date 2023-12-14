/*
 * Uart.cpp
 *
 *  Created on: Jul 28, 2022
 *      Author: Renan Augusto Starke
 */

#include <stdio.h>
#include <stdint.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp_log.h"

#include "Uart.h"

/*
 * @brief	Uart initialization.
 * @param	baud rate, data bits, parity, stop bits and flow control. See driver/uart.h.
 *
 * @retval None.
 */
Uart::Uart(int baud_rate, uart_word_length_t data_bits,
		uart_parity_t parity, uart_stop_bits_t stop_bits,
		uart_hw_flowcontrol_t flow_cotrol){

	uart_config_t uart_config;

	uart_config.baud_rate = baud_rate;
	uart_config.data_bits = data_bits;
	uart_config.parity = parity;
	uart_config.stop_bits = stop_bits;
	uart_config.flow_ctrl = flow_cotrol;
	uart_config.rx_flow_ctrl_thresh = 122;
	uart_config.source_clk = UART_SCLK_APB;

	ESP_ERROR_CHECK(uart_driver_install(UART_PORT_NUM, BUFFER_SIZE * 2, 0, 0, NULL, ESP_INTR_FLAG_IRAM));
	ESP_ERROR_CHECK(uart_param_config(UART_PORT_NUM, &uart_config));
	ESP_ERROR_CHECK(uart_set_pin(UART_PORT_NUM, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

}

/*
 * @brief	ReadBytes from UART device. Block until bytes_to_read is received
 * @param	data: pointer to store data
 * 			bytes_to_read: wait until n bytes is received
 *
 * @retval number of bytes received.
 */
int Uart::ReadBytes(uint8_t *data, uint32_t bytes_to_read){

	int len = uart_read_bytes(UART_PORT_NUM, data, bytes_to_read, portMAX_DELAY / portTICK_PERIOD_MS);

	return len;
}

void Uart::flush(){
	uart_flush(UART_PORT_NUM);
}
