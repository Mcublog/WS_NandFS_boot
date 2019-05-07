#ifndef CONSOLE_DATA_PARSE_H
#define CONSOLE_DATA_PARSE_H

#include <stdint.h>

typedef struct
{
    uint8_t* data;//��������� �� �� ������ ����������
    uint32_t size;//������ ����������
}comp_t;

typedef struct
{
    comp_t p[20];  
}console_param_t;

typedef struct
{
    uint32_t size_all;//������ ����� ������ � ������ ������� ��������� �������    
    comp_t   name;//��� �������
    comp_t   type;//��� string/bin
    comp_t   size;//���-�� ����������
    console_param_t param;//���������
}console_cmd_t;

typedef enum
{
	EMPTY = 0,
    ID_LAST_CMD
}cmd_id_t;//������� ������, ������ ������������ �������

typedef struct
{
    cmd_id_t id;
    char* s;
    void (*func_cmd)(console_cmd_t* cl_cmd, uint8_t *buf);
}cmd_t;

//���� ������ ��� ������
#define CLI_CRC_ERROR   (0)//������ crc
#define CLI_OK          (1)//��� ok
#define CLI_CORRUPT     (2)//����� ���������, ������ �� ����������

uint32_t check_start_stop_symb(const uint8_t* buf);

uint32_t console_cmd_get_size(const uint8_t* buf);
uint32_t console_cmd_parse(const uint8_t* buf, console_cmd_t* cl_cmd);

cmd_id_t get_cmd_id(comp_t* comp,const cmd_t* cmd_list);


void console_form_comp(comp_t* comp, const uint8_t *data);
void console_form_head(const char* name, const char* type, const char* size , console_cmd_t* cl_cmd);

void console_form_comp_bin(comp_t* comp, const uint8_t *data, const uint32_t size);
uint32_t console_cmd_form(console_cmd_t* cmd,
                          uint8_t *name, uint8_t *type, uint8_t *size,
                          comp_t *param,
                          uint8_t *b);
uint32_t console_cmd_form_complete(console_cmd_t* cmd, uint8_t *b);

void cmd_function_crc32_error(console_cmd_t* cl_cmd, uint8_t* buf);
void cmd_function_pkt_corrupt(console_cmd_t* cl_cmd, uint8_t* buf);

#endif
