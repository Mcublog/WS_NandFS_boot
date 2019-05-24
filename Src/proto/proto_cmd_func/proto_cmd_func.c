#include "proto_cmd_func.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "stm32f4xx_hal.h"

#include "io_fs.h"
#include "fw_ver.h"
//-----------------------Local variables and function-------------------------
static void _get_name(proto_cmd_t* cl_cmd, uint8_t *buf);
static void _jmp_app(proto_cmd_t* cl_cmd, uint8_t *buf);
static void _write_app(proto_cmd_t* cl_cmd, uint8_t *buf);
static void _get_fw_ver(proto_cmd_t* cl_cmd, uint8_t *buf);

static void _unknown_cmd(proto_cmd_t* cl_cmd, uint8_t *buf);

static const cmd_t cmd_list[] =
{
                        {EMPTY,            "EMPTY",             NULL},
//------------------------------------------------------------------------------
                        {Get_Name,         "GET_NAME",          &_get_name},
                        {Jmp_App,          "JMP_APP",           &_jmp_app},
                        {Write_App,        "WRITE_APP",         &_write_app},
                        {Get_Fw_Ver,       "GET_FW_VER",        &_get_fw_ver},
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
void cmd_function_crc32_error(proto_cmd_t* cl_cmd, uint8_t* buf)
{
    proto_form_head("ERROR_MSG", "string", "1", cl_cmd);

    proto_form_comp(&cl_cmd->param.p[0], (uint8_t*)"CRC32_FAIL");

    proto_cmd_form_complete(cl_cmd, buf);
}

/*-----------------------------------------------------------
/brief: Generate and send a response on PKT_CORRUPT
/param: Pointer to cmd
/param: Pointer to TX buf
/return:
-----------------------------------------------------------*/
void cmd_function_pkt_corrupt(proto_cmd_t* cl_cmd, uint8_t* buf)
{
    proto_form_head("ERROR_MSG", "string", "1", cl_cmd);

    proto_form_comp(&cl_cmd->param.p[0], (uint8_t*)"PKT_CORRUPT");

    proto_cmd_form_complete(cl_cmd, buf);
}

/*-----------------------------------------------------------
/Run CMD's handler
/param: Pointer to cmd
/param: Pointer to proto handler
/return:
-----------------------------------------------------------*/
void cmd_fuction_caller(proto_cmd_t* cl_cmd, io_proto_handler_t *con_h)
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
static void _unknown_cmd(proto_cmd_t* cl_cmd, uint8_t *buf)
{
    proto_form_head("ERROR_MSG", "string", "1" , cl_cmd);

    proto_form_comp(&cl_cmd->param.p[0],(uint8_t*)"UNKNOWN_CMD");

    proto_cmd_form_complete(cl_cmd, buf);
}

/*-----------------------------------------------------------
/Get device name
/param: Pointer to cmd
/param: Pointer to TX buf
/return:
-----------------------------------------------------------*/
#define DEVICE_NAME "Test_boot_v0"
static void _get_name(proto_cmd_t* cl_cmd, uint8_t *buf)
{
    uint8_t p0[] = DEVICE_NAME;

    proto_form_head("GET_NAME", "string", "1" , cl_cmd);
    proto_form_comp(&cl_cmd->param.p[0], p0);
    proto_cmd_form_complete(cl_cmd, buf);
}

/*-----------------------------------------------------------
/Get device name
/param: Pointer to cmd
/param: Pointer to TX buf
/return:
-----------------------------------------------------------*/
extern void set_boot_mark(void);
static void _jmp_app(proto_cmd_t* cl_cmd, uint8_t *buf)
{
    proto_form_head("JMP_APP", "string", "1" , cl_cmd);
    proto_form_comp(&cl_cmd->param.p[0], (uint8_t*)"OK");
    proto_cmd_form_complete(cl_cmd, buf);

    set_boot_mark();
}

/*-----------------------------------------------------------
/Get device name
/param: Pointer to cmd
/param: Pointer to TX buf
/return:
-----------------------------------------------------------*/
static void _write_app(proto_cmd_t* cl_cmd, uint8_t *buf)
{
    uint32_t size = cl_cmd->param.p[0].size;
    printf("Data size: %d\r\n", size);
    
//    io_fs_file firmware;
//    io_fs_file_open(&firmware, "firmware", IO_FS_O_RDWR | IO_FS_O_CREAT);
//    io_fs_file_rewind(&firmware);
//    io_fs_file_write(&firmware, cl_cmd->param.p[0].data, size);
//    io_fs_file_close(&firmware);
    
    proto_form_head("WRITE_APP", "string", "1" , cl_cmd);
    proto_form_comp(&cl_cmd->param.p[0], (uint8_t*)"OK");
    proto_cmd_form_complete(cl_cmd, buf);
}

/*-----------------------------------------------------------
/Get Firmware version
/param: Pointer to cmd
/param: Pointer to TX buf
/return:
-----------------------------------------------------------*/
static void _get_fw_ver(proto_cmd_t* cl_cmd, uint8_t *buf)
{
    proto_form_head("GET_FW_VER", "string", "1" , cl_cmd);
    proto_form_comp(&cl_cmd->param.p[0], (uint8_t*)"FW_VER");
    proto_cmd_form_complete(cl_cmd, buf);    
}
