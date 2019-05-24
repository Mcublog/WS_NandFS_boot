#include "io_console.h"

#include "init_main.h"
#include "stm32f4xx_hal.h"
#include <stdio.h>
#include <string.h>

#include "console_data_parse/console_data_parse.h"
#include "console_cmd_func/console_cmd_func.h"

//-----------------------Local variables and function-------------------------
typedef enum
{
    CON_COMPLETE,
    CON_WAIT_BYTES
}console_rx_state_t;

//uint8_t *_buf = NULL;
io_console_handler_t    _con = {0};
console_cmd_t           _cmd_rx = {0};

static void     _console_start_rx(void);
static uint32_t _console_data_check(console_rx_state_t *state);
static uint32_t _console_try_parse(void);
static void     _console_continue_woking(void);
//----------------------------------------------------------------------------

/*-----------------------------------------------------------
/brief: Launch console to receive data
/param:
/return:
-----------------------------------------------------------*/
static void _console_start_rx(void)
{   
    io_serial_set_dma_rx_and_idle_irq(_con.ser, _con.buf, _con.maxsize);   
}

/*-----------------------------------------------------------
/brief: Check console status
/param:
/return: 0 if error, number data bytes
-----------------------------------------------------------*/
static uint32_t _console_data_check(console_rx_state_t *state)
{
    uint32_t bytes_rx = 0, i = 0;
    io_serial_type_h type = io_serial_get_type(_con.ser);
    
    if (type == IO_UART)
    { 
        i = (_con.maxsize) - (io_serial_get_dma_bytes_waiting(_con.ser));
    }
    else i = 5;
  
    if (i >= 5)
    {
        bytes_rx = console_cmd_get_size(_con.buf);
        if (i != bytes_rx)
        {
            *state = CON_WAIT_BYTES;
            return 0;
        }
        else
        {
            *state = CON_COMPLETE;
            //Check header, if broken then restart RX
            if ( check_start_stop_symb(_con.buf)) return bytes_rx;  
            else
            {
                _console_start_rx();
                return 0;
            }
        }
    }
    else
    {
        *state = CON_WAIT_BYTES;
        return 0;
    }
}

/*-----------------------------------------------------------
/brief: Try to parse data
/param:
/return: 0 == CLI_CRC_ERROR, 1 == CLI_OK, 2 == CLI_CORRUPT
-----------------------------------------------------------*/
static uint32_t _console_try_parse(void)
{
    return console_cmd_parse(_con.buf, &_cmd_rx);
}

/*-----------------------------------------------------------
/brief: Continue to work the console
/param:
/return:
-----------------------------------------------------------*/
static void _console_continue_woking(void)
{
    io_serial_set_idle_irq(_con.ser);
}

/*-----------------------------------------------------------
/brief: Init serial console
/param: Pointer to serial handler
/param: Pointer to console data buffer
/param: Maximum buffer size
/return:
-----------------------------------------------------------*/
void io_console_init(io_serial_h *ser, uint8_t *pbuf, uint32_t size)
{   
    _con.ser       = ser;
    _con.buf       = pbuf;
    _con.maxsize   = size;
    
    _console_start_rx();
}

/*-----------------------------------------------------------
/brief: Сonsole processing
/param:
/return:
-----------------------------------------------------------*/
void io_console_process(void)
{
    console_rx_state_t  state = CON_COMPLETE;
    uint32_t data_count = _console_data_check(&state);
    
    if (data_count)
    {
        uint32_t status = _console_try_parse();
        
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
        io_serial_tx(_con.ser, _con.buf, _cmd_rx.size_all);
    }
    else
    {
        _console_continue_woking();
    }

    if (state == CON_COMPLETE) _console_start_rx(); //Reset pointers to start index of the buf_rx;
}
