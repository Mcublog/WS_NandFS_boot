#include "console_cmd_func.h"

#include <stdint.h>
#include <stdlib.h>

#include "stm32f4xx_hal.h"

//-----------------------Local variables and function-------------------------
static void _get_name(console_cmd_t* cl_cmd, uint8_t *buf);
static void _jmp_app(console_cmd_t* cl_cmd, uint8_t *buf);
static void _write_app(console_cmd_t* cl_cmd, uint8_t *buf);

static void _unknown_cmd(console_cmd_t* cl_cmd, uint8_t *buf);

static const cmd_t cmd_list[] =
{
                        {EMPTY,            "EMPTY",             NULL},
//------------------------------------------------------------------------------
                        {Get_Name,         "GET_NAME",          &_get_name},
                        {Jmp_App,          "JMP_APP",           &_jmp_app},
                        {Write_App,        "WRITE_APP",         &_write_app},
//------------------------------------------------------------------------------
                        {ID_LAST_CMD,      "LAST_CMD",          NULL}
};
//----------------------------------------------------------------------------

/*-----------------------------------------------------------
/brief: Generate and send a response on CRC32_FAIL
/param: Pointer to cmd
/param: Pointer to TX buf
/return:
-----------------------------------------------------------*/
void cmd_function_crc32_error(console_cmd_t* cl_cmd, uint8_t* buf)
{
    console_form_head("ERROR_MSG", "string", "1", cl_cmd);

    console_form_comp(&cl_cmd->param.p[0], (uint8_t*)"CRC32_FAIL");

    console_cmd_form_complete(cl_cmd, buf);
}

/*-----------------------------------------------------------
/brief: Generate and send a response on PKT_CORRUPT
/param: Pointer to cmd
/param: Pointer to TX buf
/return:
-----------------------------------------------------------*/
void cmd_function_pkt_corrupt(console_cmd_t* cl_cmd, uint8_t* buf)
{
    console_form_head("ERROR_MSG", "string", "1", cl_cmd);

    console_form_comp(&cl_cmd->param.p[0], (uint8_t*)"PKT_CORRUPT");

    console_cmd_form_complete(cl_cmd, buf);
}

/*-----------------------------------------------------------
/Run CMD's handler
/param: Pointer to cmd
/param: Pointer to console handler
/return:
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
/brief: Generate and send a response on UNKNOWN_CMD
/param: Pointer to cmd
/param: Pointer to TX buf
/return:
-----------------------------------------------------------*/
static void _unknown_cmd(console_cmd_t* cl_cmd, uint8_t *buf)
{
    console_form_head("ERROR_MSG", "string", "1" , cl_cmd);

    console_form_comp(&cl_cmd->param.p[0],(uint8_t*)"UNKNOWN_CMD");

    console_cmd_form_complete(cl_cmd, buf);
}

/*-----------------------------------------------------------
/Get device name
/param: Pointer to cmd
/param: Pointer to TX buf
/return:
-----------------------------------------------------------*/
#define DEVICE_NAME "Test_boot_v0"
static void _get_name(console_cmd_t* cl_cmd, uint8_t *buf)
{
    uint8_t p0[] = DEVICE_NAME;

    console_form_head("GET_NAME", "string", "1" , cl_cmd);
    console_form_comp(&cl_cmd->param.p[0], p0);
    console_cmd_form_complete(cl_cmd, buf);
}

/*-----------------------------------------------------------
/Get device name
/param: Pointer to cmd
/param: Pointer to TX buf
/return:
-----------------------------------------------------------*/
extern void set_boot_mark(void);
static void _jmp_app(console_cmd_t* cl_cmd, uint8_t *buf)
{
    console_form_head("JMP_APP", "string", "1" , cl_cmd);
    console_form_comp(&cl_cmd->param.p[0], (uint8_t*)"OK");
    console_cmd_form_complete(cl_cmd, buf);

    set_boot_mark();
}

/*-----------------------------------------------------------
/Get device name
/param: Pointer to cmd
/param: Pointer to TX buf
/return:
-----------------------------------------------------------*/
static void _write_app(console_cmd_t* cl_cmd, uint8_t *buf)
{
    uint8_t bin_size = cl_cmd->param.p[0].size;
    printf("Bin size: %d\r\n", bin_size);

    console_form_head("WRITE_APP", "string", "1" , cl_cmd);
    console_form_comp(&cl_cmd->param.p[0], (uint8_t*)"OK");
    console_cmd_form_complete(cl_cmd, buf);
}
