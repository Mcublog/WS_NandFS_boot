#ifndef IO_PROTO_H
#define IO_PROTO_H

#include <stdint.h>
#include "io_serial.h"

typedef struct
{
    io_serial_h *ser;
    uint8_t     *buf;
    uint32_t    maxsize;
}io_proto_handler_t;

void io_proto_init(io_serial_h *ser, uint8_t *pbuf, uint32_t size);
void io_proto_process(void);

#endif // IO_PROTO_H
