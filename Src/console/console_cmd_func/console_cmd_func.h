#ifndef CONSOLE_CMD_FUNC_H
#define CONSOLE_CMD_FUNC_H

#include "cmsis_os.h"

#include "console_data_parse/console_data_parse.h"
#include "console_data_ctrl/console_data_ctrl.h"

extern void ReportServiceTask	   (void *pvParameters);//сервисная задача для акс

extern TaskHandle_t xReportServiceHandle;

void cmd_fuction_caller(console_cmd_t* cl_cmd, console_data_ctrl_t *con_h);

#endif
