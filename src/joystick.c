/**
  ******************************************************************************
  * @file    joystick.c
  * @author  Phyton Application Team
  * @version V3.0.0
  * @date    10.09.2011
  * @brief   This file provides all the Joystick driver functions.
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
#include "MDR32FxQI_port.h"             // Milandr::Drivers:PORT
#include "joystick.h"

/** @addtogroup __MDR32F9Qx_Eval_Demo MDR32F9Qx Demonstration Example
  * @{
  */

/** @addtogroup Joystick_Driver Joystick Driver
  * @{
  */

/** @defgroup Joystick_Private_Types Joystick Private Types
  * @{
  */

/**
  * @brief  Joystick key structure definition
  */
typedef struct
{
  MDR_PORT_TypeDef * PORT;
  PORT_Pin_TypeDef PORT_Pin;
}JoystickKeys_TypeDef;

/** @} */ /* End of group Joystick_Private_Types */

/** @defgroup Joystick_Private_Variables Joystick Private Variables
  * @{
  */

static JoystickKeys_TypeDef JoystickKeys[NUM_KEY_CODES - 2] = {
  {SEL_PORT, SEL_PIN},                 /*!< SEL pressed   */
  {UP_PORT, UP_PIN},                   /*!< UP pressed    */
  {DOWN_PORT, DOWN_PIN},               /*!< DOWN pressed  */
  {LEFT_PORT, LEFT_PIN},               /*!< LEFT pressed  */
  {RIGHT_PORT, RIGHT_PIN}              /*!< RIGHT pressed */
};

/** @} */ /* End of group Joystick_Private_Variables */

/** @defgroup Joystick_Private_Functions Joystick Private Functions
  * @{
  */

/*  */
/*******************************************************************************
* Function Name  : GetKey
* Description    : Determines "code" by keys pressed
* Input          : None
* Output         : None
* Return         : "Code" of key pressed; MULTIPLE if several
*******************************************************************************/
KeyCode GetKey(void)
{
  uint32_t i;
  KeyCode key = NOKEY;

  for (i = 0; i < (NUM_KEY_CODES - 2); i++)
  {
    if ((PORT_ReadInputDataBit(JoystickKeys[i].PORT, JoystickKeys[i].PORT_Pin)) == RESET)
    {
      if (key != NOKEY)
      {
        return MULTIPLE;
      }
      key = ((KeyCode)(i));
    }
  }
  return key;
}

/** @} */ /* End of group Joystick_Private_Functions */

/** @} */ /* End of group Joystick_Driver */

/** @} */ /* End of group __MDR32F9Qx_Eval_Demo */

/******************* (C) COPYRIGHT 2011 Phyton *********************************
*
* END OF FILE joystick.c */

