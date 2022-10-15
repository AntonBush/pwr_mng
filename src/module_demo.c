#include "module_demo.h"

#include "module_pwr.h"

#include "lcd.h"
#include "joystick.h"
#include "leds.h"

#include <MDR32F9Qx_rst_clk.h>
#include <MDR32F9Qx_port.h>
#include <MDR32F9Qx_bkp.h>

#define ALL_PORTS_CLK (RST_CLK_PCLK_PORTA | RST_CLK_PCLK_PORTB | \
                       RST_CLK_PCLK_PORTC | RST_CLK_PCLK_PORTD | \
                       RST_CLK_PCLK_PORTE | RST_CLK_PCLK_PORTF)

static void Demo_configureClock(void);
static void Demo_configureRtc(void);

void Demo_init(void)
{
    PORT_InitTypeDef PortInitStructure;
    PORT_StructInit(&PortInitStructure);

    Demo_configureClock();

    /************************ LCD Initialization *************************/

    /* Configure LCD_DATA_PORT for data transfer to/from LCD */
    PortInitStructure.PORT_Pin = LCD_DATA_BAS_8_0;
    PortInitStructure.PORT_FUNC = PORT_FUNC_PORT;
    PortInitStructure.PORT_OE = PORT_OE_IN;
    PortInitStructure.PORT_SPEED = PORT_SPEED_SLOW;
    PortInitStructure.PORT_MODE = PORT_MODE_DIGITAL;

    PORT_Init(LCD_DATA_PORT_0, &PortInitStructure);
    PortInitStructure.PORT_Pin = LCD_DATA_BAS_8_1;
    PORT_Init(LCD_DATA_PORT_1, &PortInitStructure);

    /* Configure LCD_RD_WR_PORT for read/write control */
    /* Switch LCD into data output mode */
    PORT_SetBits(LCD_RD_WR_PORT, LCD_RD_WR_PIN);

    PortInitStructure.PORT_Pin = LCD_RD_WR_PIN;
    PortInitStructure.PORT_OE = PORT_OE_OUT;

    PORT_Init(LCD_RD_WR_PORT, &PortInitStructure);

    /* Configure LCD_CLOCK_PORT for CLOCK signal control*/
    /* Set LCD CLOCK signal to its initial value (0) */
    ///////////////////////////////// CONFLICT WITH LEDS ON MDR32F9Q2 /////////////////////////////////////////////////////////////////////////////////////////////////
    // PortInitStructure.PORT_Pin = LCD_CLOCK_PIN;
    // PortInitStructure.PORT_FUNC = PORT_FUNC_PORT;
    // PortInitStructure.PORT_OE = PORT_OE_OUT;
    // PortInitStructure.PORT_SPEED = PORT_SPEED_FAST;
    // PortInitStructure.PORT_MODE = PORT_MODE_DIGITAL;
    PORT_ResetBits(LCD_CLOCK_PORT, LCD_CLOCK_PIN);

    PortInitStructure.PORT_Pin = LCD_CLOCK_PIN;
    PortInitStructure.PORT_SPEED = PORT_SPEED_FAST;

    PORT_Init(LCD_CLOCK_PORT, &PortInitStructure);
    ///////////////////////////////// END /////////////////////////////////////////////////////////////////////////////////////////////////

    /* Configure LCD_CRYSTAL_PORT for LCD crystal control */
    /* De-select both LCD crystals*/
    PORT_ResetBits(LCD_CRYSTAL_PORT, LCD_CRYSTAL_PINs);

    PortInitStructure.PORT_Pin = LCD_CRYSTAL_PINs;
    PortInitStructure.PORT_SPEED = PORT_SPEED_SLOW;

    PORT_Init(LCD_CRYSTAL_PORT, &PortInitStructure);

    /* Configure LCD_CMD_DATA_PORT for data/command mode switching */
    ///////////////////////////////// CONFLICT WITH LEDS ON MDR32F9Q2 /////////////////////////////////////////////////////////////////////////////////////////////////
    // PortInitStructure.PORT_Pin = LCD_CMD_DATA_PIN;
    // PortInitStructure.PORT_FUNC = PORT_FUNC_PORT;
    // PortInitStructure.PORT_OE = PORT_OE_OUT;
    // PortInitStructure.PORT_SPEED = PORT_SPEED_SLOW;
    // PortInitStructure.PORT_MODE = PORT_MODE_DIGITAL;
    PortInitStructure.PORT_Pin = LCD_CMD_DATA_PIN;

    PORT_Init(LCD_CMD_DATA_PORT, &PortInitStructure);
    ///////////////////////////////// END /////////////////////////////////////////////////////////////////////////////////////////////////

    /* Configure LCD_RESET_PORT for RESET signal control */
    /* Zeroing LCD RES signal (initial state) */
    PORT_ResetBits(LCD_RESET_PORT, LCD_RESET_PIN);

    PortInitStructure.PORT_Pin = LCD_RESET_PIN;
    PortInitStructure.PORT_SPEED = PORT_SPEED_FAST;

    PORT_Init(LCD_RESET_PORT, &PortInitStructure);

    LCD_INIT();

    /************************ Joystick Initialization *************************/

    /* Configure SEL_PORT for input to handle joystick event SEL */
    PortInitStructure.PORT_Pin = SEL_PIN;
    PortInitStructure.PORT_OE = PORT_OE_IN;
    PortInitStructure.PORT_FUNC = PORT_FUNC_PORT;
    PortInitStructure.PORT_SPEED = PORT_SPEED_SLOW;

    PORT_Init(SEL_PORT, &PortInitStructure);

    /* Configure UP_PORT for input to handle joystick event UP */
    PortInitStructure.PORT_Pin = UP_PIN;

    PORT_Init(UP_PORT, &PortInitStructure);

    /* Configure DOWN_PORT for input to handle joystick event DOWN */
    PortInitStructure.PORT_Pin = DOWN_PIN;

    PORT_Init(DOWN_PORT, &PortInitStructure);

    /* Configure LEFT_PORT for input to handle joystick event LEFT */
    PortInitStructure.PORT_Pin = LEFT_PIN;

    PORT_Init(LEFT_PORT, &PortInitStructure);

    /* Configure RIGHT_PORT for input to handle joystick event RIGHT */
    PortInitStructure.PORT_Pin = RIGHT_PIN;

    PORT_Init(RIGHT_PORT, &PortInitStructure);

    /////////////////////////////////////////////// CONFLICTS WITH LCD ON MDR32F9Q2 //////////////////////////////////////////////////////////
    /////////////////////////////////////////////// SAVE/RESTORE PORT CONFIG IN LEDS MODULE //////////////////////////////////////////////////
    /************************ LEDs Initialization *************************/

    /* Configure LEDs_PORT for output to switch LEDs on/off */
    //  PortInitStructure.PORT_Pin   = LEDs_PINs;
    //  PortInitStructure.PORT_OE    = PORT_OE_OUT;
    //  PortInitStructure.PORT_FUNC  = PORT_FUNC_PORT;

    //  PORT_Init(LEDs_PORT, &PortInitStructure);

    /* All LEDs switch off */
    //  PORT_ResetBits(LEDs_PORT, LEDs_PINs);
    /////////////////////////////////////////////// END //////////////////////////////////////////////////////////

    /************************ PWR Initialization *************************/
    PORT_StructInit(&PortInitStructure);
    PortInitStructure.PORT_FUNC = PORT_FUNC_PORT;
    PortInitStructure.PORT_OE = PORT_OE_OUT;
    PortInitStructure.PORT_SPEED = PORT_SPEED_FAST;
    PortInitStructure.PORT_MODE = PORT_MODE_DIGITAL;

    /* Configure PWR__ADDR_PORT for output to switch between devices */
    PortInitStructure.PORT_Pin = PWR__ADDR_PINS;

    PORT_Init(PWR__ADDR_PORT, &PortInitStructure);

    /* Configure PWR__CMD_PORT for output to enable device power change */
    PortInitStructure.PORT_Pin = PWR__CMD_PIN;

    PORT_Init(PWR__CMD_PORT, &PortInitStructure);

    /* Configure PWR__DATA_PORT for output to define device power change */
    PortInitStructure.PORT_Pin = PWR__DATA_PIN;

    PORT_Init(PWR__DATA_PORT, &PortInitStructure);

    /************************ Time Initialization *************************/
    PORT_StructInit(&PortInitStructure);

    PortInitStructure.PORT_Pin = PORT_Pin_6 | PORT_Pin_7;
    PortInitStructure.PORT_MODE = PORT_MODE_ANALOG;

    PORT_Init(MDR_PORTE, &PortInitStructure);

    /************************* Uart1 Initialization *************************/
    /* Enable the RTCHSE clock on UART1 */
    RST_CLK_PCLKcmd(RST_CLK_PCLK_UART1, ENABLE);

    PORT_StructInit(&PortInitStructure);
    PortInitStructure.PORT_SPEED = PORT_SPEED_MAXFAST;
    PortInitStructure.PORT_MODE = PORT_MODE_DIGITAL;
    PortInitStructure.PORT_FUNC = PORT_FUNC_OVERRID;

    PortInitStructure.PORT_Pin = PORT_Pin_6;
    PortInitStructure.PORT_OE = PORT_OE_IN;
    PORT_Init(MDR_PORTA, &PortInitStructure);

    PortInitStructure.PORT_Pin = PORT_Pin_7;
    PortInitStructure.PORT_OE = PORT_OE_OUT;
    PORT_Init(MDR_PORTA, &PortInitStructure);

    /************************ *************************/
    Demo_configureRtc();
}

static void Demo_configureClock(void)
{
    /* Configure CPU_PLL clock */
    RST_CLK_CPU_PLLconfig(RST_CLK_CPU_PLLsrcHSIdiv1, 0);

    /* Enables the RTCHSE clock on all ports */
    RST_CLK_PCLKcmd(ALL_PORTS_CLK, ENABLE);
}

static void Demo_configureRtc(void)
{
    /* Enables the HSE clock for BKP control */
    RST_CLK_PCLKcmd(RST_CLK_PCLK_BKP, ENABLE);

    /* Configure LSE as RTC clock source */
    RST_CLK_LSEconfig(RST_CLK_LSE_ON);
    /* Wait till LSE is ready */
    while (RST_CLK_LSEstatus() != SUCCESS) {
    }

    /* Select the RTC Clock Source */
    BKP_RTCclkSource(BKP_RTC_LSEclk);
    /* Wait until last write operation on RTC registers has finished */
    BKP_RTC_WaitForUpdate();

    /* Sets the RTC prescaler */
    BKP_RTC_SetPrescaler(RTC_PRESCALER_VALUE);
    /* Wait until last write operation on RTC registers has finished */
    BKP_RTC_WaitForUpdate();

    /* Sets the RTC calibrator */
    BKP_RTC_Calibration(RTC_CalibratorValue);
    /* Wait until last write operation on RTC registers has finished */
    BKP_RTC_WaitForUpdate();

    /* Enable the RTC Clock */
    BKP_RTC_Enable(ENABLE);

    /* Enable the Second interrupt */
    BKP_RTC_ITConfig(BKP_RTC_IT_SECF, ENABLE);
    NVIC_EnableIRQ(BACKUP_IRQn);
}
