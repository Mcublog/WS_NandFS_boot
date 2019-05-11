#ifndef IO_CONSOLE_H
#define IO_CONSOLE_H

#include <stdint.h>
#include "io_serial.h"

typedef struct
{
    io_serial_h *ser;
    uint8_t     *buf;
    uint32_t    maxsize;
}io_console_handler_t;

void io_console_init(io_serial_h *ser, uint8_t *pbuf, uint32_t size);
void io_console_process(void);

#endif // IO_CONSOLE_H
