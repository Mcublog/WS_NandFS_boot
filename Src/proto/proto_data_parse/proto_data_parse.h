#ifndef CONSOLE_DATA_PARSE_H
#define CONSOLE_DATA_PARSE_H

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

typedef enum
{
    EMPTY = 0,
//-------------------------
    Get_Name,
    Jmp_App,
    Write_App,
//-------------------------
    ID_LAST_CMD
}cmd_id_t;//CMD's table, Index matches command

typedef struct
{
    cmd_id_t id;
    char* s;
    void (*func_cmd)(proto_cmd_t* cl_cmd, uint8_t *buf);
}cmd_t;

//ERROR codes
#define CLI_CRC_ERROR   (0)
#define CLI_OK          (1)
#define CLI_CORRUPT     (2)// Packet corrupted

uint32_t check_start_stop_symb(const uint8_t* buf);

uint32_t proto_cmd_get_size(const uint8_t* buf);
uint32_t proto_cmd_parse(const uint8_t* buf, proto_cmd_t* cl_cmd);

cmd_id_t get_cmd_id(comp_t* comp,const cmd_t* cmd_list);


void proto_form_comp(comp_t* comp, const uint8_t *data);
void proto_form_head(const char* name, const char* type, const char* size , proto_cmd_t* cl_cmd);

void proto_form_comp_bin(comp_t* comp, const uint8_t *data, const uint32_t size);
uint32_t proto_cmd_form(proto_cmd_t* cmd,
                          uint8_t *name, uint8_t *type, uint8_t *size,
                          comp_t *param,
                          uint8_t *b);
uint32_t proto_cmd_form_complete(proto_cmd_t* cmd, uint8_t *b);

void cmd_function_crc32_error(proto_cmd_t* cl_cmd, uint8_t* buf);
void cmd_function_pkt_corrupt(proto_cmd_t* cl_cmd, uint8_t* buf);

#endif
