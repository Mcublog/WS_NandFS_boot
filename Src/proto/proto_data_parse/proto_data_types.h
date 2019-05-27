#ifndef PROTO_DATA_TYPES_H
#define PROTO_DATA_TYPES_H

#include <stdint.h>

typedef struct
{
    uint8_t* data;// Pointer to the start of CMD's components
    uint32_t size;//CMD's components size
}comp_t;

typedef struct
{
    comp_t p[20];
}proto_param_t;

typedef struct
{
    uint32_t size_all;// The size of the entire package including service characters
    comp_t   name;//CMD's name
    comp_t   type;//Type: string/bin
    comp_t   size;//Number of parameters
    proto_param_t param;//Parameters
}proto_cmd_t;

typedef struct
{
    cmd_id_t id;
    char* s;
    void (*func_cmd)(proto_cmd_t* cl_cmd, uint8_t *buf);
}cmd_t;

typedef enum
{
    PROTO_CRC_ERROR,
    PROTO_OK,
    PROTO_CORRUPT // Packet corrupted
}proto_status_t;

#endif // PROTO_DATA_TYPES_H
