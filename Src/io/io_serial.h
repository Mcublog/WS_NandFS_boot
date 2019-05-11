#ifndef IO_SERIAL_H
#define IO_SERIAL_H

#include <stdint.h>
#include "stm32f4xx_hal.h"

typedef enum
{
    IO_UART,
    IO_USB
}io_serial_type_h;

typedef struct
{
    io_serial_type_h   type;
    UART_HandleTypeDef *phuart;
    DMA_HandleTypeDef  *phdma;
}io_serial_h;

void io_serial_init(io_serial_h *ser);
void io_serial_deinit(io_serial_h *ser);

io_serial_type_h io_serial_get_type(io_serial_h *ser);

#endif // IO_SERIAL_H
