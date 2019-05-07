#ifndef IO_SERIAL_H
#define IO_SERIAL_H

#include <stdint.h>

void io_serial_init(void);

void io_serial_console(uint8_t *pbuf, uint32_t size);
void io_serial_console_start_rx(void);
uint32_t io_serial_console_data_check(void);
uint32_t io_serial_console_parse(void);
void io_serial_console_process(uint32_t status);

void io_serial_console_continue_woking(void);

#endif // IO_SERIAL_H
