#include "console_data_parse.h"

#include <string.h> 
#include <stdlib.h>

#include "../crc32/crc32.h"


#define CL_START_SYM ('[')
#define CL_STOP_SYM (']')

#define CL_SIZE_START_STOP_SYMB (2)
#define CL_SIZE_END_SYMB (2)

#define CL_START_CMD_NAME_IDX (6) // Start of [CMD's Name] components

#define CRC32_OFFSET_IDX (7)    // Crc offset. It is to get crc from package
#define CRC32_OFFSET_COUNT (8)  // Number of service characters (Subtract from the total package size)

//[Total packet size][CMD's name][Type][Number of parameters][Param1]..[P20]
//[CMD's components]

//-----------------------Local variables and function-------------------------
static uint8_t* set_cmd_comp(uint8_t *s, comp_t* comp, uint8_t *b);
static uint8_t* set_cmd_comp_bin(   comp_t* comp,
                                    uint8_t* data, uint32_t size,
                                    uint8_t *b);

static uint32_t get_cmd_param(  console_cmd_t* cl_cmd,
                                uint32_t num_param, uint8_t* param);
//*-----------------------------------------------------------

/*-----------------------------------------------------------
/brief: Check start/stop characters of the components
/param: Pointer to data buffer
/return: 1 if char finded
-----------------------------------------------------------*/
 uint32_t check_start_stop_symb(const uint8_t* buf)
 {
     if (buf[0]=='['  && buf[5]==']') return 1;
     return 0;
 }
 
 /*-----------------------------------------------------------
/brief: Compare two buffers. Lenght b1 == b2
/param: Pointer to data buffer1
/param: Pointer to data buffer2
/param: Buffer size
/return: 1 if buffer not equal
-----------------------------------------------------------*/
uint32_t buffncmp(uint8_t *b1, uint8_t *b2, uint32_t num)
{
    for (uint32_t i = 0; i < num; i++)
    {
        if (b1[i] != b2[i]) return 1;
    }
    return 0;
} 

/*-----------------------------------------------------------
/brief: Check crc from buffer
/param: Pointer to a buf containing CMD
/param: Data size
/param: Pointer to crc32. This returns the calculated crc32.
/return: 1 if buffer not equal
-----------------------------------------------------------*/
uint32_t console_cmd_check_crc32(const uint8_t* buf, uint32_t size, uint32_t *crc32)
{
    uint32_t crc_pkt = 0, crc = 0;
    uint32_t i = 0;
    
    for (i = 0; i < 4; i++)  
    {
        if      (i == 0) crc_pkt  = (buf[size - CRC32_OFFSET_IDX +i]);
        else if (i == 1) crc_pkt |= (buf[size - CRC32_OFFSET_IDX +i] << 8);
        else if (i == 2) crc_pkt |= (buf[size - CRC32_OFFSET_IDX +i] << 16);
        else             crc_pkt |= (buf[size - CRC32_OFFSET_IDX +i] << 24);
    }
    crc = xcrc32 (buf, size - CRC32_OFFSET_COUNT, 0xFFFFFFFF); 
    if (crc == crc_pkt) return 1;
    return 0;  
}

/*-----------------------------------------------------------
/brief: Get component from buffer
/param: Pointer to component
/param: Pointer to buff
/return: Return pointer to next component
-----------------------------------------------------------*/
static uint8_t* get_cmd_comp(comp_t* comp, uint8_t* buf)
{
    uint8_t* s;//start char
    uint8_t* e;//end char
    uint32_t n;//number of char to copy
    uint8_t* b = buf;
    
    s = (uint8_t*) strchr((const char*) buf, CL_START_SYM); 
    e = (uint8_t*) strchr((const char*) buf, CL_STOP_SYM); 
    n = e - s;
    comp->size = n - 1;
    comp->data = s + 1;
    b += (n + 1);//Go to next component
    return b;
}

/*-----------------------------------------------------------
/brief: Set a string component to TX buf
/param: Pointer to data string
/param: Pointer to component
/param: Pointer to TX buff
/return: Return pointer to next free space
-----------------------------------------------------------*/
static uint8_t* set_cmd_comp(uint8_t *s, comp_t* comp, uint8_t *b)
{
    comp->size = strlen((char*) s);
    *b = CL_START_SYM;
    b++;
    comp->data = b;
    memcpy(b, s, comp->size);
    b += comp->size;
    *b = CL_STOP_SYM;
    b++;
    return (b++);
}

/*-----------------------------------------------------------
/brief: Set a binary component to TX buf
/param: Pointer to component
/param: Pointer to binary data
/param: Data size
/param: Pointer to TX buff
/return: Return pointer to next free space
-----------------------------------------------------------*/
static uint8_t* set_cmd_comp_bin(comp_t* comp,
                                 uint8_t* data, uint32_t size,
                                 uint8_t *b)
{
    comp->size = size;
    *b = CL_START_SYM;
    b++;
    comp->data = b;
    memcpy(b, data, comp->size);
    b += comp->size;
    *b = CL_STOP_SYM;
    b++;
    return (b++);
}

/*-----------------------------------------------------------
/brief: Get a number of components
/param: Pointer to CMD
/param: Number of parameters
/param: Pointer to first param
/return: 1 if all good
-----------------------------------------------------------*/
static uint32_t get_cmd_param(console_cmd_t* cl_cmd,
                              uint32_t num_param, uint8_t* param)
{
    for (uint32_t i = 0; i < num_param; i++)
    {      
        param = get_cmd_comp(&cl_cmd->param.p[i], param);
    }
    return 1;
}

/*-----------------------------------------------------------
/brief: Set the total numbers of data all package
/param: Pointer to CMD
/param: Pointer to buff. This returns the formatted package.
/param: Pointer to first param
/return: Total number of all package
-----------------------------------------------------------*/
uint32_t console_cmd_set_size_and_end(console_cmd_t* cmd, uint8_t *b)
{
    uint32_t crc = 0;
    uint32_t crc_idx = 0;
    volatile uint32_t size_pkt = 0;
    uint32_t num = atoi((const char*)cmd->size.data); 
    uint32_t size = cmd->name.size + CL_SIZE_START_STOP_SYMB;
    uint32_t i=0;
       
    size = size + cmd->type.size + CL_SIZE_START_STOP_SYMB;
    size = size + cmd->size.size + CL_SIZE_START_STOP_SYMB;
    
    for (i = 0; i < num; i++)
    {
        size = size + cmd->param.p[i].size + CL_SIZE_START_STOP_SYMB;
    }
    
    size = size + CL_SIZE_START_STOP_SYMB + CL_SIZE_START_STOP_SYMB + CL_SIZE_END_SYMB;
           
    for (i = 0; i < CL_SIZE_START_STOP_SYMB + CRC32_LENGHT; i++)
    {
       if     (i == 0)    b[size + i] = CL_START_SYM;
       else if(i == 1)
       {
           b[size + i]= 0;
           crc_idx = size + i;
       }
       else if (i == 5)   b[size + i]= CL_STOP_SYM;
       else b[size + i]= 0;
    }
    
    size = size + CL_SIZE_END_SYMB + CL_SIZE_START_STOP_SYMB + CRC32_LENGHT;
    b[0] = CL_START_SYM;
    *((uint32_t*)&b[1]) = size;  
    b[5] = CL_STOP_SYM;    
    
    *((uint32_t*)&b[crc_idx]) = 0;
    
    // crc without stop and service char
    crc = xcrc32 (b, size - CL_SIZE_START_STOP_SYMB - CL_SIZE_END_SYMB-CRC32_LENGHT, 0xffffffff);
    *((uint32_t*)&b[crc_idx]) = crc;

    for (i = 0; i < CL_SIZE_END_SYMB; i++)
    {
       if (i==0) b[size - 2]= '\r';
       else      b[size - 1]= '\n';
    }
 
    cmd->size_all = size;
    return cmd->size_all;
}

/*-----------------------------------------------------------
/brief: CMD parse
/param: Pointer to buf
/param: Pointer to CMD
/return: CLI_OK, CLI_CRC_ERROR, CLI_CORRUPT
-----------------------------------------------------------*/
uint32_t console_cmd_parse(const uint8_t* buf, console_cmd_t* cl_cmd)
{
    uint32_t num = 0;//number of parameters
    uint8_t *b = (uint8_t*)&buf[6];
    uint32_t size = console_cmd_get_size(buf);
    uint32_t crc = 0;
    
    if (size != 0)
    {
        if (console_cmd_check_crc32(buf, size, &crc))
        {
            cl_cmd->size_all = size;
            b = get_cmd_comp(&cl_cmd->name, b);
            b = get_cmd_comp(&cl_cmd->type, b);
            b = get_cmd_comp(&cl_cmd->size, b);
    
            num = atoi((const char*)cl_cmd->size.data);
    
            get_cmd_param(cl_cmd, num, b);
            return CLI_OK;
        }
        else return CLI_CRC_ERROR;
    }

    return CLI_CORRUPT;
}

/*-----------------------------------------------------------
/brief: Catch the start of handle whit total bytes of the all package
/param: Pointer to buf with CMD and data
/return: Total package size or 0 if error
-----------------------------------------------------------*/
uint32_t console_cmd_get_size(const uint8_t* buf)
{
    uint32_t size = 0;
    if (check_start_stop_symb(buf))
    {
        size = (uint32_t)buf[1];
        if (size < 8192) return size;
    }
    return 0;
}

/*-----------------------------------------------------------
/brief: Set total bytes of the all package
/param: Pointer to buf with CMD and data
/return: Total package size or 0 if error
-----------------------------------------------------------*/
uint32_t console_cmd_set_size(const uint8_t* buf)
{
    uint32_t size = 0;
    if (check_start_stop_symb(buf))
    {
        size = (uint32_t) buf[1];
        if (size < 8192) return size;
    }
    return 0;
}

/*-----------------------------------------------------------
/brief: Parse CMD's ID
/param: Pointer to a compontent
/param: Pointer to a CMD's list
/return: CMD's ID or EMPTY (0)
-----------------------------------------------------------*/
cmd_id_t get_cmd_id(comp_t* comp, const cmd_t* cmd_list )
{
    for (uint32_t i = 0; i < ID_LAST_CMD; i++)
    {
        if (buffncmp(comp->data, (uint8_t*)cmd_list[i].s, comp->size) == 0)
        {
            return cmd_list[i].id;
        }
    }
    return EMPTY;
}

/*-----------------------------------------------------------
/brief: Formatting the component
/param: Pointer to a compontent
/param: Pointer to a data buf
/return:
-----------------------------------------------------------*/
void console_form_comp(comp_t* comp, const uint8_t *data)
{
    comp->data = (uint8_t*)data;
    comp->size = sizeof(data);
}

/*-----------------------------------------------------------
/brief: Formatting the header ([NAME][TYPE][SIZE])
/param: Pointer to string with NAME
/param: Pointer to string with TYPE
/param: Pointer to string with SIZE (Number of parameters)
/return:
-----------------------------------------------------------*/
void console_form_head(const char* name, const char* type, const char* size , console_cmd_t* cl_cmd)
{
    console_form_comp(&cl_cmd->name, (uint8_t*)name);
    console_form_comp(&cl_cmd->type, (uint8_t*)type);
    console_form_comp(&cl_cmd->size, (uint8_t*)size);
}

/*-----------------------------------------------------------
/brief: Formatting the component
/param: Pointer to a component
/param: Pointer to a component data
/param: Data size
/return:
-----------------------------------------------------------*/
void console_form_comp_bin(comp_t* comp, const uint8_t *data, const uint32_t size)
{
    comp->data = (uint8_t*)data;
    comp->size = size;
}

/*-----------------------------------------------------------
/brief: Formatting a CMD to TX
/param: Pointer to a CMD
/param: Pointer to string with NAME
/param: Pointer to string with TYPE
/param: Pointer to string with SIZE (Number of parameters)
/param: Pointer to param
/param: Pointer to TX buf
/return: Total size or 0 if error
-----------------------------------------------------------*/
uint32_t console_cmd_form(console_cmd_t* cmd,
                          uint8_t *name, uint8_t *type, uint8_t *size,
                          comp_t *param,
                          uint8_t *b)
{

    uint32_t i = 0, num = 0;
    uint8_t *p;
    
    p = &b[CL_START_CMD_NAME_IDX];
    
    p = set_cmd_comp(name, &cmd->name, p);
    p = set_cmd_comp(type, &cmd->type, p);
    p = set_cmd_comp(size, &cmd->size, p);
    
    num = atoi((const char*)cmd->size.data);
    if (strncmp("string", (const char*)type, sizeof("string")) == 0)
    {
        for (i = 0; i < num; i++)
        {
            p = set_cmd_comp(param[i].data, &cmd->param.p[i], p);
        }
    }
    else
    {
        for (i = 0; i < num; i++)
        {
            p = set_cmd_comp_bin(&cmd->param.p[i], param[i].data, param[i].size, p);
        }
    }
    
    p = &b[0];
    console_cmd_set_size_and_end(cmd, p);    
    
    return 0;
}

/*-----------------------------------------------------------
/brief: Formatting a CMD to TX (CMD's parameters allready formatting)
/param: Pointer to a CMD
/param: Pointer to TX buf
/return: Total size or 0 if error
-----------------------------------------------------------*/
uint32_t console_cmd_form_complete(console_cmd_t* cmd, uint8_t *b)
{
    return console_cmd_form(cmd,
                            cmd->name.data, cmd->type.data, cmd->size.data,
                            &cmd->param.p[0], b);
}
