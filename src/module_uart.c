#include "module_uart.h"

#include <MDR32F9Qx_rst_clk.h>
#include <MDR32F9Qx_uart.h>

#include <string.h>

#define UART__BUFFER_SIZE 512

// Typedefs

typedef struct
{
    uint8_t data[UART__BUFFER_SIZE];
    size_t begin;
    size_t end;
} Uart_Buffer;

typedef struct
{
    Uart_ReceivedChar data[UART__BUFFER_SIZE];
    size_t begin;
    size_t end;
} Uart_ReceivedCharBuffer;

// Variables

static volatile Uart_ReceivedCharBuffer Uart_ReceiveBuffer = {{{0, 0}}, 0, 0};
static volatile Uart_Buffer Uart_SendBuffer = {{0}, 0, 0};

// Public functions

void Uart_init(void)
{
    UART_InitTypeDef UARTInitStructure;

    Uart_ReceiveBuffer.begin = 0;
    Uart_ReceiveBuffer.end = 0;

    Uart_SendBuffer.begin = 0;
    Uart_SendBuffer.end = 0;

    /* Set the UART1 HCLK division factor */
    UART_BRGInit(MDR_UART1, UART_HCLKdiv1);

    UART_DeInit(MDR_UART1);

    UARTInitStructure.UART_BaudRate = 9600;
    UARTInitStructure.UART_WordLength = UART_WordLength8b;
    UARTInitStructure.UART_StopBits = UART_StopBits1;
    UARTInitStructure.UART_Parity = UART_Parity_No;
    UARTInitStructure.UART_FIFOMode = UART_FIFO_ON;
    /* With LBE bit set, output line of UARTTXD transmitter becomes to be bound to
     * UARTRXD receiver input */
    UARTInitStructure.UART_HardwareFlowControl = UART_HardwareFlowControl_RXE |
                                                 UART_HardwareFlowControl_TXE;

    UART_Init(MDR_UART1, &UARTInitStructure);

    /* Enable sender and receiver interrupts */
    UART_ITConfig(MDR_UART1, UART_IT_TX | UART_IT_RX, ENABLE);

    /* Adjust sender and receiver interrupt levels */
    UART_DMAConfig(MDR_UART1, UART_IT_FIFO_LVL_12words, UART_IT_FIFO_LVL_4words);
    /* Enable DMA interrupts */
    UART_DMACmd(MDR_UART1, UART_DMA_TXE | UART_DMA_RXE, ENABLE);

    UART_Cmd(MDR_UART1, ENABLE);

    /* Enable interrupt on UART1 */
    NVIC_EnableIRQ(UART1_IRQn);
}

// buffer operations

// return -1 if nothing received
// return <uint8_t> if received
Uart_MaybeReceivedChar Uart_getChar(void)
{
    size_t begin = Uart_ReceiveBuffer.begin;
    size_t end = Uart_ReceiveBuffer.end;
    Uart_MaybeReceivedChar maybe;

    if (begin == end) {
        maybe.received = FALSE;
    } else {
        maybe.received = TRUE;
        maybe.received_data = Uart_ReceiveBuffer.data[begin];
        Uart_ReceiveBuffer.begin = (begin + 1) % UART__BUFFER_SIZE;
    }

    return maybe;
}
// param 'str' must contain '\0' character
// return number of received chars
int Uart_getString(Uart_ReceivedChar *str)
{
    size_t i;

    for (i = 0; str[i].ch != '\0'; ++i) {
        Uart_MaybeReceivedChar maybe = Uart_getChar();
        if (!maybe.received) {
            break;
        } else {
            str[i] = maybe.received_data;
        }
    }

    return i;
}

// return TRUE if successfully
bool Uart_putChar(uint8_t ch)
{
    size_t begin = Uart_SendBuffer.begin;
    size_t end = Uart_SendBuffer.end;

    if ((end + 1) % UART__BUFFER_SIZE == begin) {
        return FALSE;
    }

    Uart_SendBuffer.data[end] = ch;
    Uart_SendBuffer.end = (end + 1) % UART__BUFFER_SIZE;
    return TRUE;
}
// param 'str' must contain '\0' character
// return number of received chars
int Uart_putString(uint8_t *str)
{
    size_t i;

    for (i = 0; str[i] != '\0' && Uart_putChar(str[i]); ++i)
        ;

    return i;
}
// if it is impossible to put whole string
// buffer will not be changed
bool Uart_putWholeString(uint8_t *str)
{
    size_t begin = Uart_ReceiveBuffer.begin;
    size_t end = Uart_ReceiveBuffer.end;
    size_t free_space = UART__BUFFER_SIZE - (begin <= end ? end - begin : end + UART__BUFFER_SIZE - begin);

    if (free_space < strlen((char *)str)) {
        return FALSE;
    }

    Uart_putString(str);

    return TRUE;
}

// uart buffer operations

void Uart_receiveData(void)
{
    static volatile int busy = 0;

    ++busy;
    if (1 < busy) {
        --busy;
        return;
    }

    while (UART_GetFlagStatus(MDR_UART1, UART_FLAG_RXFE) == RESET &&
           (Uart_ReceiveBuffer.end + 1) % UART__BUFFER_SIZE != Uart_ReceiveBuffer.begin) {
        volatile Uart_ReceivedChar *received_char = Uart_ReceiveBuffer.data + Uart_ReceiveBuffer.end;
        uint16_t received_data = UART_ReceiveData(MDR_UART1);

        received_char->ch = received_data;
        received_char->error = UART_Flags(received_data);

        ++Uart_ReceiveBuffer.end;
        Uart_ReceiveBuffer.end %= UART__BUFFER_SIZE;
    }

    --busy;
}
void Uart_sendData(void)
{
    static volatile int busy = 0;
    ++busy;
    if (1 < busy) {
        --busy;
        return;
    }

    while (UART_GetFlagStatus(MDR_UART1, UART_FLAG_TXFF) == RESET &&
           Uart_SendBuffer.begin != Uart_SendBuffer.end) {
        UART_SendData(MDR_UART1, Uart_SendBuffer.data[Uart_SendBuffer.begin]);

        ++Uart_SendBuffer.begin;
        Uart_SendBuffer.begin %= UART__BUFFER_SIZE;
    }

    --busy;
}
