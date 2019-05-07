#ifndef CONSOLE_DATA_CTRL_H
#define CONSOLE_DATA_CTRL_H

#include <stdint.h>
#include "stm32f4xx_hal.h"

typedef struct
{
    UART_HandleTypeDef *huart;
    DMA_HandleTypeDef  *hdma_uart_rx;
    uint8_t  *buf;
    uint32_t maxsize;
}console_data_ctrl_t;

void console_init(console_data_ctrl_t *con_data_h,
                  UART_HandleTypeDef *huart, DMA_HandleTypeDef *hdma_uart_rx,
                  uint8_t *buf, uint32_t maxsize);
void console_start_rx(console_data_ctrl_t *con_data_h);
uint32_t console_check_data(console_data_ctrl_t *con_data_h);

void console_idle_on(console_data_ctrl_t *con_data_h);
void console_idle_off(console_data_ctrl_t *con_data_h);

#endif
