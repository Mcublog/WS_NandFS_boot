#include "main.h"
#include "cmsis_os.h"
#include <stdio.h>


#include "init_main.h"
#include "test.h"

#include "io_nand.h"
#include "io_fs.h"
#include "io_serial.h"
#include "io_console.h"

//-----------------------Types and definition---------------------------------
// NOTE: Redefine in stm32f4xx_hal_nand.h for Waveshare board
#define CMD_AREA                   ((uint32_t)(1U<<17U))  /* A16 = CLE high */
#define ADDR_AREA                  ((uint32_t)(1U<<16U))  /* A17 = ALE high */
//----------------------------------------------------------------------------

//-----------------------Local variables and fucntion-------------------------
io_serial_h _ser;
static void _serial_idle_handler(io_serial_h *ser)
//----------------------------------------------------------------------------

//-----------------------Project options--------------------------------------
//----------------------------------------------------------------------------

//-----------------------Task list--------------------------------------------
void StartDefaultTask   (void *pvParameters);
void ConsoleMsgTask     (void *pvParameters);
//----------------------------------------------------------------------------

//-----------------------Semaphore list---------------------------------------
xSemaphoreHandle xbConsoleRx;//get cmd from console
//----------------------------------------------------------------------------

//-----------------------Queue list-------------------------------------------
//xQueueHandle xqMsg;
//----------------------------------------------------------------------------

//-----------------------Programm entry point---------------------------------
int main(void)
{
    //-----------------------HW init----------------------------------------------
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    io_serial_init(&_ser, IO_UART);
    io_nand_init();
    //----------------------------------------------------------------------------

    //-----------------------Creating semaphores----------------------------------
    vSemaphoreCreateBinary(xbConsoleRx);
    //----------------------------------------------------------------------------

    //-----------------------Creating queues--------------------------------------
    //xqMsg = xQueueCreate(8, sizeof(msg_t));
    //----------------------------------------------------------------------------

    //-----------------------Creating tasks---------------------------------------
    xTaskCreate(StartDefaultTask, "StartDefaultTask", configMINIMAL_STACK_SIZE * 16, NULL, (tskIDLE_PRIORITY),     NULL);
    xTaskCreate(ConsoleMsgTask,   "ConsoleMsgTask",   configMINIMAL_STACK_SIZE * 16, NULL, (tskIDLE_PRIORITY + 1), NULL);
    //----------------------------------------------------------------------------

    //-----------------------Semaphores takes-------------------------------------
    xSemaphoreTake(xbConsoleRx, portMAX_DELAY);
    //----------------------------------------------------------------------------

    //----------------------------------------------------------------------------
    io_serial_callback_reg(&ser, IDLE_CALLBACK, (io_callback_handler_t) _serial_idle_handler);

    //----------------------------------------------------------------------------
    io_fs_file file;
      
    volatile int32_t err = io_fs_init();
    
    err = io_fs_mount();
    if (err < 0)    
    {
        printf("Format...\r\n");
        err = io_fs_format();
        err = io_fs_mount();
    }
    
    // read current count
    uint32_t boot_count = 0;
    err = io_fs_file_open(&file, "boot_count", IO_FS_O_RDWR | IO_FS_O_CREAT);
    err = io_fs_file_read(&file, &boot_count, sizeof(boot_count));

    // update boot count
    boot_count += 1;
    err = io_fs_file_rewind(&file);
    err = io_fs_file_write(&file, &boot_count, sizeof(boot_count));
    // remember the storage is not updated until the file is closed successfully
    err = io_fs_file_close(&file);
    
    // release any resources we were using
    err = io_fs_unmount();

    // print the boot count
    printf("boot_count: %d\n", boot_count);
    //----------------------------------------------------------------------------
    
    osKernelStart();
    while (1)
    {
    }
}
//------------------------------------------------------------------------------------

//------------------------- StartDefaultTask ---------------------------------
void StartDefaultTask (void *pvParameters)
{
    printf("StartDefaultTask...Start\r\n");
    int err = 0;
    io_fs_file file;
    uint32_t boot_count = 0;
    
    while(1)
    {
        vTaskDelay(5000);
        err = io_fs_mount();
        if (err < 0)    
        {
            printf("Format...\r\n");
            err = io_fs_format();
            err = io_fs_mount();
        }
    
        // read current count
        err = io_fs_file_open(&file, "boot_count", IO_FS_O_RDWR | IO_FS_O_CREAT);
        err = io_fs_file_read(&file, &boot_count, sizeof(boot_count));

//        boot_count += 1;
//        err = io_fs_file_rewind(&file);
//        err = io_fs_file_write(&file, &boot_count, sizeof(boot_count));
        err = io_fs_file_close(&file);
        err = io_fs_unmount();

        printf("boot_count: %d\n", boot_count);
   }
}
//------------------------------------------------------------------------------------

//------------------ Console command Processing --------------------------------------
#define BUFFSIZE        (8192)
uint8_t buf[BUFFSIZE] = {0};

void ConsoleMsgTask (void *pvParameters)
{
    printf("ConsoleMsgkTask...Start\r\n");
    uint32_t data_count=0;

    io_console_init(&_ser, buf, BUFFSIZE);
    uint32_t cli_status=0;  
    
    while (1)
    {
        xSemaphoreTake(xbConsoleRx, 500);
        io_console_process();
    }
}
//------------------------------------------------------------------------------------

/*-----------------------------------------------------------
/brief: IDLE IRQ callback handler
/param: Pointer to serial handler
/return:
-----------------------------------------------------------*/
static void _serial_idle_handler(io_serial_h *ser)
{
    UART_HandleTypeDef *p = ser->phuart;
    p->Instance->CR1 &= (~UART_IT_IDLE);
    xSemaphoreGiveFromISR(xbConsoleRx, NULL);
}
