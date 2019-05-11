#include "io_serial.h"

#include "init_main.h"
#include "stm32f4xx_hal.h"
#include "stdio.h"

#include "console_data_ctrl/console_data_ctrl.h"
#include "console_data_parse/console_data_parse.h"
#include "console_cmd_func/console_cmd_func.h"

//-----------------------Local variables and fucntion-------------------------
extern UART_HandleTypeDef   huart3;
extern DMA_HandleTypeDef    hdma_usart3_rx;
//----------------------------------------------------------------------------

/*-----------------------------------------------------------
/brief: Init serial
/param: Pointer to serial handler
/return:
-----------------------------------------------------------*/
void io_serial_init(io_serial_h *ser)
{
    MX_DMA_Init();
    MX_USART3_UART_Init();
    
    ser->phuart = &huart3;
    ser->phdma  = &hdma_usart3_rx;
}

/*-----------------------------------------------------------
/brief: DeInit serial
/param: Pointer to serial handler
/return:
-----------------------------------------------------------*/
void io_serial_deinit(io_serial_h *ser)
{
    MX_DMA_Deinit();
    MX_USART3_UART_Deinit();
    
    ser->phuart = NULL;
    ser->phdma  = NULL;    
}
