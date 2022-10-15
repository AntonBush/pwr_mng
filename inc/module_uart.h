#ifndef MODULE_UART
#define MODULE_UART

#include "types.h"
#include <stddef.h>

#define UART__RECIEVE_ERROR_NO_ERROR   0
#define UART__RECIEVE_ERROR_FRAME      1
#define UART__RECIEVE_ERROR_PARITY     2
#define UART__RECIEVE_ERROR_BREAK_LINE 4
#define UART__RECIEVE_ERROR_OVERFLOW   8

typedef struct
{
    uint8_t ch;
    uint8_t error;
} Uart_ReceivedChar;

typedef struct
{
    Uart_ReceivedChar received_data;
    bool received;
} Uart_MaybeReceivedChar;

void Uart_init(void);

// buffer operations

Uart_MaybeReceivedChar Uart_getChar(void);
// str[str_length].ch must contain '\0' character
// return number of received chars
int Uart_getString(Uart_ReceivedChar *str);

// return TRUE if successfully
bool Uart_putChar(uint8_t ch);
// param 'str' must contain '\0' character
// return number of received chars
int Uart_putString(uint8_t *str);
// if it is impossible to put whole string
// buffer will not be changed
bool Uart_putWholeString(uint8_t *str);

// uart buffer operations

void Uart_receiveData(void);
void Uart_sendData(void);

#endif // MODULE_UART
