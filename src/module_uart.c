#include "module_uart.h"

#include <MDR32F9Qx_rst_clk.h>
#include <MDR32F9Qx_uart.h>

#define UART__BUFFER_SIZE 256

// Typedefs

typedef struct
{
    uint8_t data[UART__BUFFER_SIZE];
    size_t begin;
    size_t end;
} Uart_Buffer;

// Private variables

static volatile Uart_Buffer Uart_RecieveBuffer;
static volatile Uart_Buffer Uart_SendBuffer;

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
    return -1;
}
// param 'str' must contain '\0' character
// return number of recieved chars
int Uart_getString(uint8_t *str)
{
    return 0;
}
bool Uart_isRecieveBufferEmpty(void)
{
    return TRUE;
}

// return TRUE if successfully
bool Uart_putChar(uint8_t ch)
{
    return FALSE;
}
// param 'str' must contain '\0' character
// return number of recieved chars
int Uart_putString(uint8_t *str)
{
    return 0;
}
// if it is impossible to put whole string
// buffer will not be changed
bool Uart_putWholeString(uint8_t *str)
{
    return FALSE;
}
bool Uart_isSendBufferFull(void)
{
    return TRUE;
}

// uart buffer operations

void Uart_recieveBytes(void)
{
}
void Uart_sendBytes(void)
{
}
