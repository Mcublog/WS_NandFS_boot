#include "io_serial.h"

#include "init_main.h"
#include "stm32f4xx_hal.h"
#include "stdio.h"

#include "console_data_ctrl/console_data_ctrl.h"
#include "console_data_parse/console_data_parse.h"
#include "console_cmd_func/console_cmd_func.h"

//-----------------------Local variables and fucntion-------------------------
extern UART_HandleTypeDef huart3;
extern DMA_HandleTypeDef hdma_usart3_rx;

uint8_t *_buf = NULL;
console_data_ctrl_t _con = {0};
console_cmd_t _cmd_rx = {0};

//----------------------------------------------------------------------------

/*-----------------------------------------------------------
/brief: Init serial
/param:
/return:
-----------------------------------------------------------*/
void io_serial_init(void)
{
    MX_DMA_Init();
    MX_USART3_UART_Init();
}

/*-----------------------------------------------------------
/brief: Init serial console
/param:
/return:
-----------------------------------------------------------*/
void io_serial_console(uint8_t *pbuf, uint32_t size)
{
    _buf = pbuf;
    console_init(&_con, &huart3, &hdma_usart3_rx, _buf, size);
    io_serial_console_start_rx();
}

/*-----------------------------------------------------------
/brief: Launch console to receive data
/param:
/return:
-----------------------------------------------------------*/
void io_serial_console_start_rx(void)
{
    console_start_rx(&_con);
}

/*-----------------------------------------------------------
/brief: Check console status
/param:
/return: 0 if error, number data bytes
-----------------------------------------------------------*/
uint32_t io_serial_console_data_check(void)
{
    return console_check_data(&_con);
}

/*-----------------------------------------------------------
/brief: Try to parse data
/param:
/return: 0 == CLI_CRC_ERROR, 1 == CLI_OK, 2 == CLI_CORRUPT
-----------------------------------------------------------*/
uint32_t io_serial_console_parse(void)
{
    return console_cmd_parse(_buf, &_cmd_rx);
}

/*-----------------------------------------------------------
/brief: Process console status
/param:
/return:
-----------------------------------------------------------*/
void io_serial_console_process(uint32_t status)
{
    if  (status == CLI_OK)
    {
        printf("Data rx -- OK\r\n");
        cmd_fuction_caller(&_cmd_rx, &_con);
    }
    else if (status == CLI_CRC_ERROR)
    {
        printf("Data rx -- CRC32 Error\r\n");
        cmd_function_crc32_error(&_cmd_rx, _buf);
    }
    else
    {
        printf("Data rx -- Corrupted\r\n");
        cmd_function_pkt_corrupt(&_cmd_rx, _buf);
    }
    if (_con.huart != NULL) HAL_UART_Transmit(_con.huart, _buf, _cmd_rx.size_all, 1000);
    io_serial_console_start_rx();
}


/*-----------------------------------------------------------
/brief: Continue to work the console
/param:
/return:
-----------------------------------------------------------*/
void io_serial_console_continue_woking(void)
{
    console_idle_on(&_con);
}
