/**
 ******************************************************************************
 * @file    Main.c
 * @author  Phyton Application Team
 * @version V3.0.0
 * @date    10.09.2011
 * @brief   Main program body
 ******************************************************************************
 * <br><br>
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
 * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
 * TIME. AS A RESULT, PHYTON SHALL NOT BE HELD LIABLE FOR ANY
 * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
 * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
 * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * <h2><center>&copy; COPYRIGHT 2011 Phyton</center></h2>
 */

/* Includes ------------------------------------------------------------------*/
#include "joystick.h"
#include "module_app.h"
#include "module_uart.h"
#include "module_interrupt.h"

/** @addtogroup __MDR32F9Qx_Eval_Demo MDR32F9Qx Demonstration Example
 * @{
 */

/** @addtogroup Main Main
 * @{
 */

/** @defgroup Main_Functions Main Functions
 * @{
 */

/*******************************************************************************
 * Function Name  : main
 * Description    : Main program.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
#ifdef __CC_ARM
int main(void)
#else
void main(void)
#endif
{
    Interrupt_UartReceiveHandler = Uart_receiveData;
    Interrupt_UartSendHandler = Uart_sendData;
    Interrupt_UartLineStateHandler = NULL;
    Interrupt_RtcSecondHandler = App_rtcSecondHandle;

    App_init();
    App_run();
}

/*******************************************************************************
 * Function Name  : assert_failed
 * Description    : Reports the source file ID, the source line number
 *                : and (if USE_ASSERT_INFO == 2) expression text where
 *                : the assert_param error has occurred.
 * Input          : file_id - pointer to the source file name
 *                : line - assert_param error line source number
 *                : expr - expression text
 * Output         : None
 * Return         : None
 *******************************************************************************/

#if (USE_ASSERT_INFO == 1)
void assert_failed(uint32_t file_id, uint32_t line)
{
    /* User can add his own implementation to report the source file ID and line number.
       Ex: printf("Wrong parameters value: file Id %d on line %d\r\n", file_id, line) */

    /* Infinite loop */
    while (1) {
    }
}
#elif (USE_ASSERT_INFO == 2)
void assert_failed(uint32_t file_id, uint32_t line, const uint8_t *expr)
{
    /* User can add his own implementation to report the source file ID, line number and
       expression text.
       Ex: printf("Wrong parameters value (%s): file Id %d on line %d\r\n", expr, file_id, line) */

    /* Infinite loop */
    while (1) {
    }
}
#endif /* USE_ASSERT_INFO */

/** @} */ /* End of group Main_Functions */

/** @} */ /* End of group Main */

/** @} */ /* End of group __MDR32F9Qx_Eval_Demo */

/******************* (C) COPYRIGHT 2011 Phyton *********************************
 *
 * END OF FILE Main.c */
