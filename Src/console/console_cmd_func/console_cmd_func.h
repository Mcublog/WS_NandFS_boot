#ifndef CONSOLE_CMD_FUNC_H
#define CONSOLE_CMD_FUNC_H

#include "cmsis_os.h"

#include "console_data_parse/console_data_parse.h"
#include "io_console.h"

void cmd_fuction_caller(console_cmd_t* cl_cmd, io_console_handler_t *con_h);

#endif
