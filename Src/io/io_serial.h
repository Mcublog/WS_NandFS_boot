#ifndef IO_SERIAL_H
#define IO_SERIAL_H

#include <stdint.h>
#include "stm32f4xx_hal.h"

typedef struct
{
    UART_HandleTypeDef *phuart;
    DMA_HandleTypeDef  *phdma;
}io_serial_h;

void io_serial_init(io_serial_h *ser);
void io_serial_deinit(io_serial_h *ser);

#endif // IO_SERIAL_H
