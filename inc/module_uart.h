#ifndef MODULE_UART
#define MODULE_UART

#include "types.h"
#include <stddef.h>

void Uart_init(void);

// buffer operations

// return -1 if nothing recieved
// return <uint8_t> if recieved
int Uart_getChar(void);
// param 'str' must contain '\0' character
// return number of recieved chars
int Uart_getString(uint8_t *str);
bool Uart_isRecieveBufferEmpty(void);

// return TRUE if successfully
bool Uart_putChar(uint8_t ch);
// param 'str' must contain '\0' character
// return number of recieved chars
int Uart_putString(uint8_t *str);
// if it is impossible to put whole string
// buffer will not be changed
bool Uart_putWholeString(uint8_t *str);
bool Uart_isSendBufferFull(void);

// uart buffer operations

void Uart_recieveBytes(void);
void Uart_sendBytes(void);

#endif // MODULE_UART