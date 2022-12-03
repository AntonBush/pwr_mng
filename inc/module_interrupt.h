#ifndef MODULE_INTERRUPT
#define MODULE_INTERRUPT

#include "module_utility.h"

#include "MDR32FxQI_config.h"           // Milandr::Device:Startup

/* UART event mask as of interest by CDC PSTN line state notifications */
#define UART_LINE_STATE_EVENTS (UART_IT_OE | UART_IT_BE | UART_IT_PE | UART_IT_FE | \
                                UART_IT_DSR | UART_IT_DCD | UART_IT_RI)

/* Pointers to UART send/receive interrupt handlers */
extern Utility_Procedure *Interrupt_UartReceiveHandler;
extern Utility_Procedure *Interrupt_UartSendHandler;
extern Utility_ProcedureUint32 *Interrupt_UartLineStateHandler;

extern Utility_Procedure *Interrupt_RtcSecondHandler;

#endif // MODULE_INTERRUPT
