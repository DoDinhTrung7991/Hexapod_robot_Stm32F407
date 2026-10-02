#ifndef _UART_H_
#define _UART_H_

#include "init_static.h"
#include "USART_header.h"
#include "DMA.h"
#include "queue.h"

#define UART_STATE_READY 1
#define UART_STATE_BUSY 0

typedef enum
{
    USART1,
    USART2,
    USART3,
    UART4,
    UART5,
    USART6
} UARTx_t;

extern volatile bool UART_init_state[6];
extern volatile queue_t UART_recv_buf[6];
extern volatile bool UART_state_tx[6];
extern volatile bool UART_state_rx[6];
extern volatile uint8_t isUpdated_UART[6];  //Need to be checked before reading UART data

bool UART_init(UARTx_t UARTx, uint32_t baudrate_u32);
bool UART_transmit(UARTx_t UARTx, const uint8_t *buf, uint8_t data_length);
bool UART_Read(UARTx_t UARTx, uint8_t *buf, uint8_t data_length);

#endif
