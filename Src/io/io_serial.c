#include "io_serial.h"

#include "init_main.h"
#include "stm32f4xx_hal.h"
#include "stdio.h"

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
    ser->type = IO_UART;
    
    if (ser->type == IO_UART)
    {
        MX_DMA_Init();
        MX_USART3_UART_Init();
    
        ser->phuart = &huart3;
        ser->phdma  = &hdma_usart3_rx;        
    }
}

/*-----------------------------------------------------------
/brief: DeInit serial
/param: Pointer to serial handler
/return:
-----------------------------------------------------------*/
void io_serial_deinit(io_serial_h *ser)
{
    if (ser->type == IO_UART)
    {
        ser->type = IO_NONE;
        MX_DMA_Deinit();
        MX_USART3_UART_Deinit();
    
        ser->phuart = NULL;
        ser->phdma  = NULL;
    }
}

/*-----------------------------------------------------------
/brief: Get serial type
/param: Pointer to serial handler
/return: Type io_serial
-----------------------------------------------------------*/
io_serial_type_h io_serial_get_type(io_serial_h *ser)
{
    return ser->type;
}

/*-----------------------------------------------------------
/brief: Set Idle IRQ
/param: Pointer to serial handler
/return:
-----------------------------------------------------------*/
void io_serial_set_idle_irq(io_serial_h *ser)
{
    if (ser->type == IO_UART)
    {        
        ser->phuart->Instance->CR1 |= UART_IT_IDLE;
    }
}

/*-----------------------------------------------------------
/brief: Serial TX
/param: Pointer to serial handler
/param: Pointer to TX bufer
/param: Number bytes to TX
/return:
-----------------------------------------------------------*/
void io_serial_tx(io_serial_h *ser, uint8_t *buf, uint32_t size)
{   
    HAL_UART_Transmit(ser->phuart, buf, size, 1000);
}
