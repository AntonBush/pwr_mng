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

// Variables

static volatile Uart_Buffer Uart_RecieveBuffer = {{0}, 0, 0};
static volatile Uart_Buffer Uart_SendBuffer = {{0}, 0, 0};

// Public functions

void Uart_init(void)
{
    /* Enable the RTCHSE clock on UART1 */
    RST_CLK_PCLKcmd(RST_CLK_PCLK_UART1, ENABLE);

    /* Set the UART1 HCLK division factor */
    UART_BRGInit(MDR_UART1, UART_HCLKdiv1);

    UART_DeInit(MDR_UART1);

    /* Enable interrupt on UART1 */
    NVIC_EnableIRQ(UART1_IRQn);

    Uart_RecieveBuffer.begin = 0;
    Uart_RecieveBuffer.end = 0;

    Uart_SendBuffer.begin = 0;
    Uart_SendBuffer.end = 0;
}

// buffer operations

// return -1 if nothing recieved
// return <uint8_t> if recieved
int Uart_getChar(void)
{
    size_t begin = Uart_RecieveBuffer.begin;
    size_t end = Uart_RecieveBuffer.end;
    uint8_t ch;

    if (begin == end) {
        return -1;
    }

    ch = Uart_RecieveBuffer.data[begin];
    begin = (begin + 1) % UART__BUFFER_SIZE;
    return ch;
}
// param 'str' must contain '\0' character
// return number of recieved chars
int Uart_getString(uint8_t *str)
{
    size_t i;

    for (i = 0; str[i] != '\0'; ++i) {
        int ch = Uart_getChar();
        if (ch < 0) {
            break;
        } else {
            str[i] = ch;
        }
    }

    return i;
}

bool Uart_isRecieveBufferEmpty(void)
{
    size_t begin = Uart_RecieveBuffer.begin;
    size_t end = Uart_RecieveBuffer.end;

    return (begin == end) ? TRUE : FALSE;
}

// return TRUE if successfully
bool Uart_putChar(uint8_t ch)
{
    size_t begin = Uart_RecieveBuffer.begin;
    size_t end = Uart_RecieveBuffer.end;

    if ((end + 1) % UART__BUFFER_SIZE == begin) {
        return FALSE;
    }

    Uart_SendBuffer.data[end] = ch;
    end = (end + 1) % UART__BUFFER_SIZE;
    return TRUE;
}
// param 'str' must contain '\0' character
// return number of recieved chars
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
    size_t begin = Uart_RecieveBuffer.begin;
    size_t end = Uart_RecieveBuffer.end;
    size_t size = (begin <= end) ? (end - begin) : (end + UART__BUFFER_SIZE - begin);

    if (size < strlen((char *)str)) {
        return FALSE;
    }

    Uart_putString(str);

    return TRUE;
}
bool Uart_isSendBufferFull(void)
{
    size_t begin = Uart_RecieveBuffer.begin;
    size_t end = Uart_RecieveBuffer.end;

    return ((end + 1) % UART__BUFFER_SIZE == begin) ? TRUE : FALSE;
}

// uart buffer operations

void Uart_recieveBytes(void)
{
    static volatile int busy = 0;
    ++busy;
    if (1 < busy) {
        --busy;
        return;
    }

    // todo
}
void Uart_sendBytes(void)
{
    static volatile int busy = 0;
    ++busy;
    if (1 < busy) {
        --busy;
        return;
    }

    while (UART_GetFlagStatus(MDR_UART1, UART_FLAG_TXFF) == RESET && !Uart_isSendBufferFull()) {
        UART_SendData(MDR_UART1, Uart_SendBuffer.data[Uart_SendBuffer.begin]);
        ++Uart_SendBuffer.begin;
        Uart_SendBuffer.begin %= UART__BUFFER_SIZE;
    }
}
