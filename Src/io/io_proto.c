#include "io_proto.h"

#include "init_main.h"
#include "stm32f4xx_hal.h"
#include <stdio.h>
#include <string.h>

#include "proto_data_parse/proto_data_parse.h"
#include "proto_cmd_func/proto_cmd_func.h"

//-----------------------Local variables and function-------------------------
typedef enum
{
    PROTO_COMPLETE,
    PROTO_WAIT_BYTES
}proto_rx_state_t;

//uint8_t *_buf = NULL;
io_proto_handler_t    _con = {0};
proto_cmd_t           _cmd_rx = {0};

static void     _proto_start_rx(void);
static uint32_t _proto_data_check(proto_rx_state_t *state);
static uint32_t _proto_try_parse(void);
static void     _proto_continue_woking(void);
//----------------------------------------------------------------------------

/*-----------------------------------------------------------
/brief: Launch proto to receive data
/param:
/return:
-----------------------------------------------------------*/
static void _proto_start_rx(void)
{   
    io_serial_set_dma_rx_and_idle_irq(_con.ser, _con.buf, _con.maxsize);   
}

/*-----------------------------------------------------------
/brief: Check proto status
/param:
/return: 0 if error, number data bytes
-----------------------------------------------------------*/
static uint32_t _proto_data_check(proto_rx_state_t *state)
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
        bytes_rx = proto_cmd_get_size(_con.buf);
        if (i != bytes_rx)
        {
            *state = PROTO_WAIT_BYTES;
            return 0;
        }
        else
        {
            *state = PROTO_COMPLETE;
            //Check header, if broken then restart RX
            if ( check_start_stop_symb(_con.buf)) return bytes_rx;  
            else
            {
                _proto_start_rx();
                return 0;
            }
        }
    }
    else
    {
        *state = PROTO_WAIT_BYTES;
        return 0;
    }
}

/*-----------------------------------------------------------
/brief: Try to parse data
/param:
/return: 0 == CLI_CRC_ERROR, 1 == CLI_OK, 2 == CLI_CORRUPT
-----------------------------------------------------------*/
static uint32_t _proto_try_parse(void)
{
    return proto_cmd_parse(_con.buf, &_cmd_rx);
}

/*-----------------------------------------------------------
/brief: Continue to work the proto
/param:
/return:
-----------------------------------------------------------*/
static void _proto_continue_woking(void)
{
    io_serial_set_idle_irq(_con.ser);
}

/*-----------------------------------------------------------
/brief: Init serial proto
/param: Pointer to serial handler
/param: Pointer to proto data buffer
/param: Maximum buffer size
/return:
-----------------------------------------------------------*/
void io_proto_init(io_serial_h *ser, uint8_t *pbuf, uint32_t size)
{   
    _con.ser       = ser;
    _con.buf       = pbuf;
    _con.maxsize   = size;
    
    _proto_start_rx();
}

/*-----------------------------------------------------------
/brief: Сonsole processing
/param:
/return:
-----------------------------------------------------------*/
void io_proto_process(void)
{
    proto_rx_state_t  state = PROTO_COMPLETE;
    uint32_t data_count = _proto_data_check(&state);
    
    if (data_count)
    {
        uint32_t status = _proto_try_parse();
        
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
        _proto_continue_woking();
    }

    if (state == PROTO_COMPLETE) _proto_start_rx(); //Reset pointers to start index of the buf_rx;
}
