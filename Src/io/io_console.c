#include "io_console.h"

#include "init_main.h"
#include "stm32f4xx_hal.h"
#include <stdio.h>
#include <string.h>

#include "console_data_parse/console_data_parse.h"
#include "console_cmd_func/console_cmd_func.h"

//-----------------------Local variables and fucntion-------------------------
//uint8_t *_buf = NULL;
console_data_ctrl_t _con = {0};
console_cmd_t _cmd_rx = {0};
//----------------------------------------------------------------------------

/*-----------------------------------------------------------
/brief: Init serial console
/param:
/return:
-----------------------------------------------------------*/
void io_console_init(io_serial_h *ser, uint8_t *pbuf, uint32_t size)
{   
    _con.ser       = ser;
    _con.buf       = pbuf;
    _con.maxsize   = size;
    
    io_console_start_rx();
}

/*-----------------------------------------------------------
/brief: Launch console to receive data
/param:
/return:
-----------------------------------------------------------*/
void io_console_start_rx(void)
{
    //console_start_rx(&_con);
    
    io_serial_type_h type = io_serial_get_type(_con.ser);
    if (type == IO_UART)
    { 
        HAL_UART_DMAStop(_con.ser->phuart); 
        HAL_UART_Receive_DMA(_con.ser->phuart, _con.buf, _con.maxsize);  
        _con.ser->phuart->Instance->CR1|=UART_IT_IDLE;
    }
    else
    {
        uint8_t dummy[5] = {0, 0, 0, 0, 0};//буфер для очистки заголовка пакета
        memcpy((void*)_con.buf, (void*)dummy, sizeof(dummy));
    }    
}

/*-----------------------------------------------------------
/brief: Check console status
/param:
/return: 0 if error, number data bytes
-----------------------------------------------------------*/
uint32_t io_console_data_check(void)
{
    uint32_t bytes_rx = 0;
    uint32_t i = 0;
    io_serial_type_h type = io_serial_get_type(_con.ser);
    
    if (type == IO_UART)
    { 
        i = (_con.maxsize) - (_con.ser->phdma->Instance->NDTR);
    }
    else i = 5;
  
    if (i >= 5)
    {
        bytes_rx = console_cmd_get_size(_con.buf);
        if (type == IO_UART) i = bytes_rx;
        if (i != bytes_rx)
        {
            return 0;
        }
        else
        {
            //проверяю стартовый заголовок, если битый, то пересбрасываю DMA
            if ( check_start_stop_symb(_con.buf)) return bytes_rx;  
            else
            {
                io_console_start_rx();
                return 0;
            }
        }
    }
    else return 0;
}

/*-----------------------------------------------------------
/brief: Try to parse data
/param:
/return: 0 == CLI_CRC_ERROR, 1 == CLI_OK, 2 == CLI_CORRUPT
-----------------------------------------------------------*/
uint32_t io_console_parse(void)
{
    return console_cmd_parse(_con.buf, &_cmd_rx);
}

/*-----------------------------------------------------------
/brief: Process console status
/param:
/return:
-----------------------------------------------------------*/
void io_console_process(uint32_t status)
{
    if  (status == CLI_OK)
    {
        printf("Data rx -- OK\r\n");
        cmd_fuction_caller(&_cmd_rx, &_con);
    }
    else if (status == CLI_CRC_ERROR)
    {
        printf("Data rx -- CRC32 Error\r\n");
        cmd_function_crc32_error(&_cmd_rx, _con.buf);
    }
    else
    {
        printf("Data rx -- Corrupted\r\n");
        cmd_function_pkt_corrupt(&_cmd_rx, _con.buf);
    }
    
    if (io_serial_get_type(_con.ser) == IO_UART)
    {
        HAL_UART_Transmit(_con.ser->phuart, _con.buf, _cmd_rx.size_all, 1000);
    }
    io_console_start_rx();
}


/*-----------------------------------------------------------
/brief: Continue to work the console
/param:
/return:
-----------------------------------------------------------*/
void io_console_continue_woking(void)
{
    if (io_serial_get_type(_con.ser) == IO_UART)
    {
        _con.ser->phuart->Instance->CR1 |= UART_IT_IDLE;
    }    
}
