#ifndef PROTO_CMD_LIST_H
#define PROTO_CMD_LIST_H

typedef enum
{
    EMPTY = 0,
//-------------------------
    Get_Name,
    Jmp_App,
    Write_App,
    Get_Fw_Ver,
//-------------------------
    ID_LAST_CMD
}cmd_id_t;//CMD's table, Index matches command

#endif // PROTO_CMD_LIST_H
