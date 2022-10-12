#ifndef MODULE_INTERRUPT
#define MODULE_INTERRUPT

#include <MDR32Fx.h>
#include "types.h"

/* UART event mask as of interest by CDC PSTN line state notifications */
#define UART_LINE_STATE_EVENTS		(UART_IT_OE | UART_IT_BE | UART_IT_PE | UART_IT_FE | \
																	 UART_IT_DSR | UART_IT_DCD | UART_IT_RI)

typedef void (Interrupt_Procedure)(void);
typedef void (Interrupt_ProcedureUint32)(uint32_t);

/* Pointers to UART send/receive interrupt handlers */
extern Interrupt_Procedure * Interrupt_UartReceiveHandler;
extern Interrupt_Procedure * Interrupt_UartSendHandler;
extern Interrupt_ProcedureUint32 * Interrupt_UartLineStateHandler;

extern Interrupt_Procedure * Interrupt_RtcSecondHandler;

#endif // MODULE_INTERRUPT
