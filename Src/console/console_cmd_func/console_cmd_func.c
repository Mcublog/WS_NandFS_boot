#include "console_cmd_func.h"

#include <stdint.h>
#include <stdlib.h>

#include "stm32f4xx_hal.h"

//-----------------------Local variables and fucntion-------------------------
static const cmd_t cmd_list[]=
{
                         {EMPTY,            "EMPTY",        NULL},
//------------------------------------------------------------------------------                        

//------------------------------------------------------------------------------                         
                         {ID_LAST_CMD,      "LAST_CMD",         NULL}
};

static void _unknown_cmd(console_cmd_t* cl_cmd, uint8_t *buf);
//----------------------------------------------------------------------------

/*-----------------------------------------------------------
/Сформировать сообщение об ошибке crc32
/принимает: указетель на консольную команду и
/буфер для отправки
/вовращает:
-----------------------------------------------------------*/
void cmd_function_crc32_error(console_cmd_t* cl_cmd, uint8_t* buf)
{
    console_form_head("ERROR_MSG", "string", "1", cl_cmd);
    
    console_form_comp(&cl_cmd->param.p[0], (uint8_t*)"CRC32_FAIL");
    
    console_cmd_form_complete(cl_cmd, buf);
}

/*-----------------------------------------------------------
/Сформировать сообщение о испорченном пакете
/принимает: указетель на консольную команду и
/буфер для отправки
/вовращает:
-----------------------------------------------------------*/
void cmd_function_pkt_corrupt(console_cmd_t* cl_cmd, uint8_t* buf)
{
    console_form_head("ERROR_MSG", "string", "1", cl_cmd);
    
    console_form_comp(&cl_cmd->param.p[0], (uint8_t*)"PKT_CORRUPT");
    
    console_cmd_form_complete(cl_cmd, buf); 
}

/*-----------------------------------------------------------
/Выполнение команад
/принимает: указетель на консольную команду и
/хедер обработки консоли
/вовращает:
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
/Сформировать сообщение о неизвестной команде
/принимает: указетель на консольную команду и
/буфер для отправки
/вовращает:
-----------------------------------------------------------*/
static void _unknown_cmd(console_cmd_t* cl_cmd, uint8_t *buf)
{
    console_form_head("ERROR_MSG", "string", "1" , cl_cmd);

    console_form_comp(&cl_cmd->param.p[0],(uint8_t*)"UNKNOWN_CMD");
    
    console_cmd_form_complete(cl_cmd, buf);    
}

