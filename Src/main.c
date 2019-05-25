#include "main.h"
#include "cmsis_os.h"
#include <stdio.h>
#include <string.h>

#include "init_main.h"
//#include "test.h"

#include "io/io_nand/io_nand.h"
#include "io/io_serial/io_serial.h"
#include "io/io_fs.h"
#include "io_proto.h"

#include "firmware.h"

//-----------------------Types and definition---------------------------------
// NOTE: Redefine in stm32f4xx_hal_nand.h for Waveshare board
#define CMD_AREA                   ((uint32_t)(1U<<17U))  /* A16 = CLE high */
#define ADDR_AREA                  ((uint32_t)(1U<<16U))  /* A17 = ALE high */

#define MMNGR_BACKUPRAM_OFFSET  (0x0008)
#define BKP_BASE                (0x4000284C)

#define MAIN_PROGRAM_START_ADDRESS  FIRMWARE_ADDR
//----------------------------------------------------------------------------

//-----------------------Local variables and function-------------------------
io_serial_h _ser;
static void _serial_idle_handler(io_serial_h *ser);

void copy_and_jmp_to_app(void);

void set_boot_mark(void);
static void _check_boot_mark(void);


static void _config_write_word32(uint32_t full_adr, uint32_t word);
//----------------------------------------------------------------------------

//-----------------------Project options--------------------------------------
//----------------------------------------------------------------------------

//-----------------------Task list--------------------------------------------
void StartDefaultTask   (void *pvParameters);
void ProtoMsgTask       (void *pvParameters);
//----------------------------------------------------------------------------

//-----------------------Semaphore list---------------------------------------
xSemaphoreHandle xbProtoRx;//get cmd from proto
//----------------------------------------------------------------------------

//-----------------------Queue list-------------------------------------------
//xQueueHandle xqMsg;
//----------------------------------------------------------------------------

//-----------------------Programm entry point---------------------------------
int main(void)
{
    //-----------------------HW init----------------------------------------------
    HAL_Init();

    _check_boot_mark();

    SystemClock_Config();
    MX_GPIO_Init();
    io_serial_init(&_ser, IO_UART);
    io_nand_init();
    //----------------------------------------------------------------------------

    //-----------------------Creating semaphores----------------------------------
    vSemaphoreCreateBinary(xbProtoRx);
    //----------------------------------------------------------------------------

    //-----------------------Creating queues--------------------------------------
    //xqMsg = xQueueCreate(8, sizeof(msg_t));
    //----------------------------------------------------------------------------

    //-----------------------Creating tasks---------------------------------------
    xTaskCreate(StartDefaultTask, "StartDefaultTask", configMINIMAL_STACK_SIZE * 16, NULL, (tskIDLE_PRIORITY),     NULL);
    xTaskCreate(ProtoMsgTask,     "ProtoMsgTask",     configMINIMAL_STACK_SIZE * 16, NULL, (tskIDLE_PRIORITY + 1), NULL);
    //----------------------------------------------------------------------------

    //-----------------------Semaphores takes-------------------------------------
    xSemaphoreTake(xbProtoRx, portMAX_DELAY);
    //----------------------------------------------------------------------------

    //----------------------------------------------------------------------------
    io_serial_callback_reg(&_ser, IDLE_CALLBACK, (io_callback_handler_t) _serial_idle_handler);

    //-----------------------Some preliminary steps-------------------------------
    // Check  internal flash
    uint32_t *check = (uint32_t *)MAIN_PROGRAM_START_ADDRESS;
    if (*check != 0xFFFFFFFF)
    {
        // Erase the firmware in the internal flash
        printf("Erase internal flash...\r\n");
        HAL_FLASH_Unlock();
        FLASH_Erase_Sector(FLASH_SECTOR_5, FLASH_VOLTAGE_RANGE_3);
        HAL_FLASH_Lock();
    }
    
    // Delete firmware file
    io_fs_file firmware;
    int32_t err = io_fs_init();
    err = io_fs_mount();
    if (err < 0)
    {
        printf("Format...\r\n");
        err = io_fs_format();
        err = io_fs_mount();
    }    

    printf("Delete firmware from the FS...\r\n");
    io_fs_file_delete("firmware");

    //----------------------------------------------------------------------------
    

    
    osKernelStart();
    while (1)
    {
    }
}
//------------------------------------------------------------------------------------

/*-----------------------------------------------------------
/brief: Copy the app to the inner flash and boot
/param:
/return:
-----------------------------------------------------------*/
void copy_and_jmp_to_app(void)
{
    printf("firm_array size in WORD: %d\r\n", sizeof(firm_array));

    HAL_FLASH_Unlock();
    FLASH_Erase_Sector(FLASH_SECTOR_5, FLASH_VOLTAGE_RANGE_3);
    HAL_FLASH_Lock();
    uint32_t *pword = (uint32_t*) firm_array;
    for (uint32_t i = 0; i < (sizeof(firm_array)>>2); i++)
    {
        _config_write_word32((MAIN_PROGRAM_START_ADDRESS + i * 4), pword[i]);
    }
    set_boot_mark();
}

/*-----------------------------------------------------------
/brief: Set mark to boot new app and RESET
/param:
/return:
-----------------------------------------------------------*/
void set_boot_mark(void)
{
    io_fs_file firmware;
    int32_t err = io_fs_file_open(&firmware, "firmware", IO_FS_O_RDONLY);
    if (err < 0)
    {
        printf("Firmware's file not found\r\n");
        return;
    }
    uint32_t size = io_fs_file_get_size(&firmware);
    io_fs_file_rewind(&firmware);
    for (uint32_t i = 0; i < (size >> 2); i++)
    {
        uint32_t data;
        io_fs_file_read(&firmware, (void *) &data, 4);
        _config_write_word32(MAIN_PROGRAM_START_ADDRESS + (i * 4), data);        
    }
    io_fs_file_close(&firmware);
    io_fs_file_delete("firmware");
    
    uint32_t *prst = (uint32_t*)(BKP_BASE + MMNGR_BACKUPRAM_OFFSET);

    HAL_PWR_EnableBkUpAccess();
    *prst = 0xFFFFFFFF;
    HAL_PWR_DisableBkUpAccess();
    //RST
    HAL_NVIC_SystemReset();
}

/*-----------------------------------------------------------
/brief: Check boot mark, reset it, and boot new app
/param:
/return:
-----------------------------------------------------------*/
static void _check_boot_mark(void)
{
    volatile uint32_t *prst = (uint32_t*)(BKP_BASE + MMNGR_BACKUPRAM_OFFSET);

    if (*prst == 0xFFFFFFFF)
    {
        typedef  void (*pFunction)(void);

        uint32_t jumpAddress = *((volatile uint32_t*) (MAIN_PROGRAM_START_ADDRESS + 4));
        pFunction Jump_To_Application = (pFunction) jumpAddress;


        HAL_PWR_EnableBkUpAccess();
        *prst = 0; // clear boot mark
        HAL_PWR_DisableBkUpAccess();

        HAL_RCC_DeInit();
        HAL_DeInit();

        __disable_irq();
        SCB->VTOR = MAIN_PROGRAM_START_ADDRESS;
        __enable_irq();

        __set_MSP(*(volatile uint32_t*) MAIN_PROGRAM_START_ADDRESS);
        Jump_To_Application();
    }
}

//------------------------- StartDefaultTask ---------------------------------
void StartDefaultTask (void *pvParameters)
{
    printf("StartDefaultTask...Start\r\n");
    int err = 0;
    io_fs_file file;
    uint32_t boot_count = 0;
    
    err = io_fs_mount();
    if (err < 0)
    {
        printf("Format...\r\n");
        err = io_fs_format();
        err = io_fs_mount();
    }

    while(1)
    {
        vTaskDelay(5000);

        // read current count
        err = io_fs_file_open(&file, "boot_count", IO_FS_O_RDWR | IO_FS_O_CREAT);
        err = io_fs_file_read(&file, &boot_count, sizeof(boot_count));

//        boot_count += 1;
//        err = io_fs_file_rewind(&file);
//        err = io_fs_file_write(&file, &boot_count, sizeof(boot_count));
        err = io_fs_file_close(&file);
        //err = io_fs_unmount();

        //printf("boot_count: %d\n", boot_count);
   }
}
//------------------------------------------------------------------------------------

//------------------ Console command Processing --------------------------------------
#define BUFFSIZE        (8192)
uint8_t buf[BUFFSIZE] = {0};

void ProtoMsgTask (void *pvParameters)
{
    printf("ConsoleMsgkTask...Start\r\n");
    io_proto_init(&_ser, buf, BUFFSIZE);

    HAL_GPIO_TogglePin(Debug_GPIO_Port, Debug_Pin);

    while (1)
    {
        xSemaphoreTake(xbProtoRx, 500);
        io_proto_process();
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
    xSemaphoreGiveFromISR(xbProtoRx, NULL);
}

/*-----------------------------------------------------------
/brief: Write U32 word to FLASH
/param: Flash addr
/param: Data word
/return:
-----------------------------------------------------------*/
static void _config_write_word32(uint32_t full_adr, uint32_t word)
{
    HAL_StatusTypeDef s = HAL_ERROR;
    HAL_FLASH_Unlock();

    while (s==HAL_ERROR)
    {
        __HAL_FLASH_DATA_CACHE_DISABLE();
        __HAL_FLASH_INSTRUCTION_CACHE_DISABLE();

        __HAL_FLASH_DATA_CACHE_RESET();
        __HAL_FLASH_INSTRUCTION_CACHE_RESET();

        __HAL_FLASH_INSTRUCTION_CACHE_ENABLE();
        __HAL_FLASH_DATA_CACHE_ENABLE();
        s = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, full_adr, word);
    }
    HAL_FLASH_Lock();
}
