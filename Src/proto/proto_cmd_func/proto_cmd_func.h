#ifndef PROTO_CMD_FUNC_H
#define PROTO_CMD_FUNC_H

#include "cmsis_os.h"

#include "../proto_data_parse/proto_data_parse.h"
#include "io_proto.h"

void cmd_fuction_caller(proto_cmd_t* cl_cmd, io_proto_handler_t *con_h);

#endif // PROTO_CMD_FUNC_H
