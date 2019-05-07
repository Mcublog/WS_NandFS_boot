#include "console_data_ctrl.h"

#include <string.h>
#include "../console_data_parse/console_data_parse.h"

#define CONSOLE_IDLE_ON(huart)  (huart.Instance->CR1|=UART_IT_IDLE)
#define CONSOLE_IDLE_OFF(huart) (huart.Instance->CR1&=(~UART_IT_IDLE))

/*
//Инициализация хидера управления потом данных на косоль.
*/
void console_init(console_data_ctrl_t *con_data_h, UART_HandleTypeDef *huart, DMA_HandleTypeDef *hdma_uart_rx, uint8_t *buf, uint32_t maxsize)
{
    con_data_h->huart = huart;
    con_data_h->hdma_uart_rx = hdma_uart_rx;
    con_data_h->buf = buf;
    con_data_h->maxsize = maxsize;
}

/*
//Перезапускает ожидание приема данных. 
*/
void console_start_rx(console_data_ctrl_t *con_data_h)
{
  uint8_t dummy[5]={0, 0, 0, 0, 0};//буфер для очистки заголовка пакета
  if (con_data_h->huart!=NULL)
  { 
    HAL_UART_DMAStop(con_data_h->huart); 
    HAL_UART_Receive_DMA(con_data_h->huart, con_data_h->buf, con_data_h->maxsize);  
    con_data_h->huart->Instance->CR1|=UART_IT_IDLE;
  }
  else
  {
      memcpy((void*)con_data_h->buf, (void*)dummy, sizeof(dummy));
  }
}

/*
//Проверяет, что в конце буфера лежит символ '\n'.
//Также идет проверка на размер пакета
//Если он получен, значит пакет полностью получен.
//Пакет не болжен быть больше размера буфера.
//Вовзращает кол-во данных или ноль если ошибка
*/
uint32_t console_check_data(console_data_ctrl_t *con_data_h)
{
  uint32_t bytes_rx=0;
  uint32_t i = 0;
  if (con_data_h->huart != NULL)
  { 
    i = (con_data_h->maxsize) - (con_data_h->hdma_uart_rx->Instance->NDTR);
  }
  else i = 5;
  if ( i >=5 )
  {
    bytes_rx = console_cmd_get_size(con_data_h->buf);
    if (con_data_h->huart == NULL) i = bytes_rx;
    if (i != bytes_rx)
    {
        return 0;
    }
    else
    {
        //проверяю стартовый заголовок, если битый, то пересбрасываю DMA
        if ( check_start_stop_symb(con_data_h->buf))return bytes_rx;  
        else
        {
            console_start_rx(con_data_h);
            return 0;
        }
    }
  }
  else return 0;
}

/*
//Проверяет, что в конце буфера лежит символ '\n'.
//Также идет проверка на размер пакета
//Если он получен, значит пакет полностью получен.
//Пакет не болжен быть больше размера буфера.
//Вовзращает кол-во данных или ноль если ошибка
*/
void console_idle_on(console_data_ctrl_t *con_data_h)
{
    if (con_data_h->huart != NULL)
    {
        con_data_h->huart->Instance->CR1|=UART_IT_IDLE;
    }
}

/*
//Проверяет, что в конце буфера лежит символ '\n'.
//Также идет проверка на размер пакета
//Если он получен, значит пакет полностью получен.
//Пакет не болжен быть больше размера буфера.
//Вовзращает кол-во данных или ноль если ошибка
*/
void console_idle_off(console_data_ctrl_t *con_data_h)
{
    if (con_data_h->huart != NULL)
    {
        con_data_h->huart->Instance->CR1&=(~UART_IT_IDLE);
    }
}

