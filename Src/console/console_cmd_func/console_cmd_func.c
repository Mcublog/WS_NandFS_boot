#include "console_cmd_func.h"

#include <stdint.h>
#include <stdlib.h>

#include "stm32f4xx_hal.h"

//-----------------------Local variables and fucntion-------------------------
static void _get_name(console_cmd_t* cl_cmd, uint8_t *buf);
static void _unknown_cmd(console_cmd_t* cl_cmd, uint8_t *buf);    

static const cmd_t cmd_list[] =
{
                        {EMPTY,            "EMPTY",             NULL},
//------------------------------------------------------------------------------
                        {Get_Name,         "GET_NAME",          &_get_name},
//------------------------------------------------------------------------------
                        {ID_LAST_CMD,      "LAST_CMD",          NULL}
};
//----------------------------------------------------------------------------

/*-----------------------------------------------------------
/������������ ��������� �� ������ crc32
/���������: ��������� �� ���������� ������� �
/����� ��� ��������
/���������:
-----------------------------------------------------------*/
void cmd_function_crc32_error(console_cmd_t* cl_cmd, uint8_t* buf)
{
    console_form_head("ERROR_MSG", "string", "1", cl_cmd);
    
    console_form_comp(&cl_cmd->param.p[0], (uint8_t*)"CRC32_FAIL");
    
    console_cmd_form_complete(cl_cmd, buf);
}

/*-----------------------------------------------------------
/������������ ��������� � ����������� ������
/���������: ��������� �� ���������� ������� �
/����� ��� ��������
/���������:
-----------------------------------------------------------*/
void cmd_function_pkt_corrupt(console_cmd_t* cl_cmd, uint8_t* buf)
{
    console_form_head("ERROR_MSG", "string", "1", cl_cmd);
    
    console_form_comp(&cl_cmd->param.p[0], (uint8_t*)"PKT_CORRUPT");
    
    console_cmd_form_complete(cl_cmd, buf); 
}

/*-----------------------------------------------------------
/���������� �������
/���������: ��������� �� ���������� ������� �
/����� ��������� �������
/���������:
-----------------------------------------------------------*/                       
void cmd_fuction_caller(console_cmd_t* cl_cmd, io_console_handler_t *con_h)
{
    volatile cmd_id_t id = get_cmd_id(&cl_cmd->name, cmd_list);
    if (cmd_list[id].func_cmd != NULL)
    {
        cmd_list[id].func_cmd(cl_cmd, con_h->buf);
    }
    else
    {
        _unknown_cmd(cl_cmd, con_h->buf);
    }
}

/*-----------------------------------------------------------
/������������ ��������� � ����������� �������
/���������: ��������� �� ���������� ������� �
/����� ��� ��������
/���������:
-----------------------------------------------------------*/
static void _unknown_cmd(console_cmd_t* cl_cmd, uint8_t *buf)
{
    console_form_head("ERROR_MSG", "string", "1" , cl_cmd);

    console_form_comp(&cl_cmd->param.p[0],(uint8_t*)"UNKNOWN_CMD");
    
    console_cmd_form_complete(cl_cmd, buf);    
}

/*-----------------------------------------------------------
/�������� ��� ����������, ������������ ��� ���������� �� ��� �����
/���������: ��������� �� ���������� ������� �
/����� ��� ��������
/���������:
-----------------------------------------------------------*/ 
#define DEVICE_NAME "Test_boot_v0"
static void _get_name(console_cmd_t* cl_cmd, uint8_t *buf)
{
    uint8_t p0[]=DEVICE_NAME;

    console_form_head("GET_NAME", "string", "1" , cl_cmd);
    console_form_comp(&cl_cmd->param.p[0], p0);
    console_cmd_form_complete(cl_cmd, buf);     
}

