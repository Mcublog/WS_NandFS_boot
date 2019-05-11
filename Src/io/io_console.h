#ifndef IO_CONSOLE_H
#define IO_CONSOLE_H

#include <stdint.h>
#include "io_serial.h"

void io_console_init(io_serial_h *ser, uint8_t *pbuf, uint32_t size);
void io_console_start_rx(void);
uint32_t io_console_data_check(void);
uint32_t io_console_parse(void);
void io_console_process(uint32_t status);

void io_console_continue_woking(void);

#endif // IO_CONSOLE_H
