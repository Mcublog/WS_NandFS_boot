#include "io_serial.h"

#include "init_main.h"
#include "stm32f4xx_hal.h"
#include "stdio.h"

#include "console_data_parse/console_data_parse.h"
#include "console_cmd_func/console_cmd_func.h"

//-----------------------Local variables and fucntion-------------------------
extern UART_HandleTypeDef   huart3;
extern DMA_HandleTypeDef    hdma_usart3_rx;

// HW Specific function
static void _hw_init(io_serial_h *ser);
static void _hw_deinit(io_serial_h *ser);
static void _hw_set_idle_irq(io_serial_h *ser);
static void _hw_tx_data(io_serial_h *ser, uint8_t *buf, uint32_t size);
static uint32_t _hw_get_dma_bytes_waiting(io_serial_h *ser);
static void _hw_set_usart_dma_rx_and_idle_irq(io_serial_h *ser, uint8_t *buf, uint32_t size);
//----------------------------------------------------------------------------

/*-----------------------------------------------------------
/brief: Init HW
/param: Pointer to serial handler
/return:
-----------------------------------------------------------*/
static void _hw_init(io_serial_h *ser)
{
    if (ser->type == IO_UART)
    {
        // HW Specific        
        MX_DMA_Init();
        MX_USART3_UART_Init();

        ser->phuart = &huart3;
        ser->phdma  = &hdma_usart3_rx;        
    }    
}

/*-----------------------------------------------------------
/brief: DeInit HW
/param: Pointer to serial handler
/return:
-----------------------------------------------------------*/
static void _hw_deinit(io_serial_h *ser)
{
    if (ser->type == IO_UART)
    {        
        ser->type = IO_NONE;
        
        // HW Specific        
        MX_DMA_Deinit();
        MX_USART3_UART_Deinit();
    
        ser->phuart = NULL;
        ser->phdma  = NULL;
    }  
}

/*-----------------------------------------------------------
/brief: Set IDLE Irq
/param: Pointer to serial handler
/return:
-----------------------------------------------------------*/
static void _hw_set_idle_irq(io_serial_h *ser)
{
    if (ser->type == IO_UART)
    {
        // HW Specific
        UART_HandleTypeDef *p = ser->phuart;
        p->Instance->CR1 |= UART_IT_IDLE;
    }
}

/*-----------------------------------------------------------
/brief: Serial TX
/param: Pointer to serial handler
/param: Pointer to TX bufer
/param: Number bytes to TX
/return:
-----------------------------------------------------------*/
static void _hw_tx_data(io_serial_h *ser, uint8_t *buf, uint32_t size)
{
    if (ser->type == IO_UART)
    {   
        // HW Specific
        UART_HandleTypeDef *p = ser->phuart;
        HAL_UART_Transmit(p, buf, size, 1000);
    }
}

/*-----------------------------------------------------------
/brief: Get the number of bytes from DMA
/param: Pointer to serial handler
/return:
-----------------------------------------------------------*/
static uint32_t _hw_get_dma_bytes_waiting(io_serial_h *ser)
{
    if (ser->type == IO_UART)
    {
        // HW Specific
        DMA_HandleTypeDef *p = ser->phdma;
        return p->Instance->NDTR;
    }
    return 0;
}

/*-----------------------------------------------------------
/brief: Set DMA rx and IDLE IRQ
/param: Pointer to serial handler
/param: Pointer to RX bufer
/param: Number bytes to RX
/return:
-----------------------------------------------------------*/
static void _hw_set_usart_dma_rx_and_idle_irq(io_serial_h *ser, uint8_t *buf, uint32_t size)
{
    if (ser->type == IO_UART)
    {
        // HW Specific
        UART_HandleTypeDef *phuart = ser->phuart;
    
        HAL_UART_DMAStop(phuart); 
        HAL_UART_Receive_DMA(phuart, buf, size);  
        _hw_set_idle_irq(ser);    
    }
}

/*-----------------------------------------------------------
/brief: Init serial
/param: Pointer to serial handler
/return:
-----------------------------------------------------------*/
void io_serial_init(io_serial_h *ser)
{   
    ser->type = IO_UART;
    
    _hw_init(ser);
}

/*-----------------------------------------------------------
/brief: DeInit serial
/param: Pointer to serial handler
/return:
-----------------------------------------------------------*/
void io_serial_deinit(io_serial_h *ser)
{
    _hw_deinit(ser);
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
    _hw_set_idle_irq(ser);
}

/*-----------------------------------------------------------
/brief: Get the number of bytes from DMA
/param: Pointer to serial handler
/return:
-----------------------------------------------------------*/
uint32_t io_serial_get_dma_bytes_waiting(io_serial_h *ser)
{
    return _hw_get_dma_bytes_waiting(ser);
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
    _hw_tx_data(ser, buf, size);
}

/*-----------------------------------------------------------
/brief: Set DMA rx and IDLE IRQ
/param: Pointer to serial handler
/param: Pointer to RX bufer
/param: Number bytes to RX
/return:
-----------------------------------------------------------*/
void io_serial_set_dma_rx_and_idle_irq(io_serial_h *ser, uint8_t *buf, uint32_t size)
{
    _hw_set_usart_dma_rx_and_idle_irq(ser, buf, size);
}

//TODO:
void io_serial_callback_reg(io_serial_h *ser, io_callback_id_t id)
{
}

//TODO:
void io_serial_callback_call(io_serial_h *ser)
{
    if (ser->type == IO_UART)
    {
        UART_HandleTypeDef *p = ser->phuart;
        uint32_t isrflags = READ_REG(p->Instance->SR);
        if (READ_BIT(isrflags, USART_SR_IDLE))//if IDLE handler call
        {
            
        }        
    }
}
