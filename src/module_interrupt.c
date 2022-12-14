#include "module_interrupt.h"

#include "MDR32FxQI_bkp.h"              // Milandr::Drivers:BKP
#include "MDR32FxQI_uart.h"             // Milandr::Drivers:UART

#include <stddef.h>

// Private function prototypes

static void Interrupt_UartXHandler(MDR_UART_TypeDef *uart_x);

// Variables

Utility_Procedure *Interrupt_UartReceiveHandler = NULL;
Utility_Procedure *Interrupt_UartSendHandler = NULL;
Utility_ProcedureUint32 *Interrupt_UartLineStateHandler = NULL;

Utility_Procedure *Interrupt_RtcSecondHandler = NULL;

// Public functions

void UART1_IRQHandler(void)
{
    Interrupt_UartXHandler(MDR_UART1);
}

void UART2_IRQHandler(void)
{
    Interrupt_UartXHandler(MDR_UART2);
}

void BACKUP_IRQHandler(void)
{
    if (BKP_RTC_GetFlagStatus(BKP_RTC_FLAG_SECF) == SET) {
        BKP_RTC_ITConfig(BKP_RTC_IT_SECF, DISABLE);

        if (Interrupt_RtcSecondHandler != NULL) {
            Interrupt_RtcSecondHandler();
        }

        BKP_RTC_ITConfig(BKP_RTC_IT_SECF, ENABLE);
    }
}

// Private functions

void Interrupt_UartXHandler(MDR_UART_TypeDef *uart_x)
{
    uint32_t tmp_ris = uart_x->RIS;

    /* Clear all pending bits except for UART_IT_RX */
    uart_x->ICR |= (tmp_ris & (~UART_IT_RX));

    /* Receive handler */
    if ((tmp_ris & UART_IT_RX) && Interrupt_UartReceiveHandler != NULL) {
        Interrupt_UartReceiveHandler();
    }

    /* Transmit handler */
    if ((tmp_ris & UART_IT_TX) && Interrupt_UartSendHandler != NULL) {
        Interrupt_UartSendHandler();
    }

    /* Break/error handler */
    if (Interrupt_UartLineStateHandler != 0) {
        tmp_ris &= UART_LINE_STATE_EVENTS;
        if (tmp_ris) {
            Interrupt_UartLineStateHandler(tmp_ris);
        }
    }
}
