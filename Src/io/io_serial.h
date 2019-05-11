#ifndef IO_SERIAL_H
#define IO_SERIAL_H

#include <stdint.h>
#include "stm32f4xx_hal.h"

typedef enum
{
    IO_NONE,    
    IO_UART,
    IO_USB,
}io_serial_type_h;

typedef enum
{
    TX_CALLBACK,
    RX_CALLBACK,
    IDLE_CALLBACK,
//-----------------    
    LAST_CALLBACK
}io_callback_id_t;

typedef struct
{
    io_serial_type_h   type;
    void *phuart;
    void *phdma;
}io_serial_h;

typedef void (*io_callback_handler_t)(io_serial_h *ser);

void io_serial_init(io_serial_h *ser);
void io_serial_deinit(io_serial_h *ser);

io_serial_type_h io_serial_get_type(io_serial_h *ser);
void io_serial_set_idle_irq(io_serial_h *ser);

void io_serial_tx(io_serial_h *ser, uint8_t *buf, uint32_t size);

uint32_t io_serial_get_dma_bytes_waiting(io_serial_h *ser);
void io_serial_set_dma_rx_and_idle_irq(io_serial_h *ser, uint8_t *buf, uint32_t size);

#endif // IO_SERIAL_H
