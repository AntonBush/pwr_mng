#include "module_time.h"

#include "MDR32F9Qx_config.h"
#include <MDR32Fx.h>
#include <MDR32F9Qx_port.h>
#include <MDR32F9Qx_bkp.h>
#include <MDR32F9Qx_rst_clk.h>

// Private functions prototypes

static void Time_setRawTime(uint32_t time);

static void Time_upProc(void);
static void Time_selectProc(void);
static void Time_downProc(void);
static void Time_updateProc(void);

static void Time_highlightItem(void);

static void Time_changeTimeProc(void);
static void Time_saveProc(void);
static void Time_returnProc(void);

static void Time_increaseProc(void);
static void Time_reduceProc(void);

static void Time_updateGuiStr(void);

static void Time_configureRtc(void);

// Private variables

static Time_TimeEdit Time_Time = { 0, 0, 0 };

static uint8_t Time_HourStr[] = { '0', '0', 'h', '\0' };
static size_t Time_HourStrLength = MENU__COUNT_OF(Time_HourStr);

static uint8_t Time_MinuteStr[] = { '0', '0', 'm', '\0' };
static size_t Time_MinuteStrLength = MENU__COUNT_OF(Time_MinuteStr);

static uint8_t Time_SecondStr[] = { '0', '0', 's', '\0' };
static size_t Time_SecondStrStrLength = MENU__COUNT_OF(Time_SecondStr);

static uint8_t Time_TimeStr[] = { '.', '.', 'h', '.', '.', 'm', '.', '.', 's', '\0' };
static size_t Time_TimeStrLength = MENU__COUNT_OF(Time_TimeStr);

static uint32_t Time_NPassedDays = 0;

#define TIME__RETURN_ITEM_INDEX 4
static Menu_MenuItem Time_SetTimeMenuItems[] = { { Time_HourStr, NULL, Time_changeTimeProc }
																								, { Time_MinuteStr, NULL, Time_changeTimeProc }
																								, { Time_SecondStr, NULL, Time_changeTimeProc }
																								, { "Save", NULL, Time_saveProc }
																								, { Time_TimeStr, NULL, NULL }
																								};

Menu_Menu Time_SetTimeMenu = { "Set time"
															, Time_SetTimeMenuItems
															, MENU__COUNT_OF(Time_SetTimeMenuItems)
															, 0
															, Time_upProc
															, Time_selectProc
															, Time_downProc
															, Time_updateProc
															};

// Public functions

void Time_init(Menu_Procedure * return_proc)
{
	unsigned int i;
	uint32_t tmp;

	Time_SetTimeMenuItems[TIME__RETURN_ITEM_INDEX].proc = return_proc;

  /* Enables the HSE clock for BKP control */
  RST_CLK_PCLKcmd(RST_CLK_PCLK_BKP, ENABLE);

  Time_configureRtc();

	// if backup was reset (first run)
  if (MDR_BKP->REG_00 != 0x1234)
  {
		Time_setRawTime(0);

		MDR_BKP->REG_00 = 0x1234;
		/* Wait until last write operation on RTC registers has finished */
    BKP_RTC_WaitForUpdate();
  }
  else if (tmp = Time_getRawTime(), tmp / 86400 != 0)
  {
		for (i = 0; i < tmp / 86400; ++i)
		{
			Time_addPassedDay();
		}
		
		Time_setRawTime(tmp % 86400);
  }
}

uint32_t Time_getRawTime(void)
{
	return BKP_RTC_GetCounter();
}

Time_TimeEdit Time_getTime(void)
{
	uint32_t tmp;
	Time_TimeEdit time;
	
	tmp = Time_getRawTime();
	time.hour = tmp / 3600;
	time.minute = tmp / 60 % 60;
	time.second = tmp % 60;
	
	return time;
}

void Time_setTime(Time_TimeEdit time)
{
	if ((23 < time.hour)
			|| (59 < time.minute)
			|| (59 < time.second)) return;
	
	Time_setRawTime(time.hour * 3600 + time.minute * 60 + time.second);
	
	Time_updateGuiStr();
}

uint32_t Time_getNPassedDays(void)
{
	uint32_t tmp = Time_NPassedDays;
	Time_NPassedDays = 0;
	return tmp;
}

void Time_addPassedDay(void)
{
	++Time_NPassedDays;
}

// Private functions

void Time_setRawTime(uint32_t time)
{
  /* Set the RTC counter value */
	BKP_RTC_SetCounter(time);
  /* Wait until last write operation on RTC registers has finished */
  BKP_RTC_WaitForUpdate();
}

void Time_upProc(void)
{
	Menu_selectUpperItem(&Time_SetTimeMenu);
}

void Time_selectProc(void)
{
	size_t index = Time_SetTimeMenu.item_index;
	Menu_Procedure * proc = Time_SetTimeMenu.items[index].proc;
	if (proc != NULL)
	{
		proc();
	}
}

void Time_downProc(void)
{
	Menu_selectLowerItem(&Time_SetTimeMenu);
}

void Time_updateProc(void)
{
	Time_TimeEdit time = Time_getTime();
	
	Time_TimeStr[0] = '0' + (time.hour / 10 % 10);
	Time_TimeStr[1] = '0' + (time.hour % 10);

	Time_TimeStr[3] = '0' + (time.minute / 10 % 10);
	Time_TimeStr[4] = '0' + (time.minute % 10);

	Time_TimeStr[6] = '0' + (time.second / 10 % 10);
	Time_TimeStr[7] = '0' + (time.second % 10);
	
	Menu_displayMenuItemString(MENU__LINE_MESSAGE_5, Time_TimeStr);
	if (Time_SetTimeMenu.item_index == TIME__RETURN_ITEM_INDEX)
	{
		Menu_highlightMenuItemString(MENU__LINE_MESSAGE_5, "                                        ");
	}
}

void Time_highlightItem(void)
{
	switch (Time_SetTimeMenu.item_index)
	{
		case 0: Menu_highlightMenuItemString(MENU__LINE_MESSAGE_1, "   "); break;
		case 1: Menu_highlightMenuItemString(MENU__LINE_MESSAGE_2, "   "); break;
		case 2: Menu_highlightMenuItemString(MENU__LINE_MESSAGE_3, "   "); break;
	}
}

void Time_changeTimeProc(void)
{
	Time_SetTimeMenu.up = Time_increaseProc;
	Time_SetTimeMenu.select = Time_returnProc;
	Time_SetTimeMenu.down = Time_reduceProc;
	
	Time_highlightItem();
}

void Time_saveProc(void)
{
	Time_setTime(Time_Time);
}

void Time_returnProc(void)
{
	Time_SetTimeMenu.up = Time_upProc;
	Time_SetTimeMenu.select = Time_selectProc;
	Time_SetTimeMenu.down = Time_downProc;

	Time_highlightItem();
}

static void Time_increaseProc(void)
{
	switch (Time_SetTimeMenu.item_index)
	{
		case 0:
			Time_Time.hour = (Time_Time.hour + 1) % 24;
			Time_updateGuiStr();
			Menu_displayString(MENU__LINE_MESSAGE_1, "   ");
			Menu_displayString(MENU__LINE_MESSAGE_1, Time_HourStr);
			break;

		case 1:
			Time_Time.minute = (Time_Time.minute + 1) % 60;
			Time_updateGuiStr();
			Menu_displayString(MENU__LINE_MESSAGE_2, "   ");
			Menu_displayString(MENU__LINE_MESSAGE_2, Time_MinuteStr);
			break;

		case 2:
			Time_Time.second = (Time_Time.second + 1) % 60;
			Time_updateGuiStr();
			Menu_displayString(MENU__LINE_MESSAGE_3, "   ");
			Menu_displayString(MENU__LINE_MESSAGE_3, Time_SecondStr);
			break;
	}
}

static void Time_reduceProc(void)
{
	switch (Time_SetTimeMenu.item_index)
	{
		case 0:
			Time_Time.hour = (Time_Time.hour == 0) ? 23 : Time_Time.hour - 1;
			Time_updateGuiStr();
			Menu_displayString(MENU__LINE_MESSAGE_1, "   ");
			Menu_displayString(MENU__LINE_MESSAGE_1, Time_HourStr);
			break;

		case 1:
			Time_Time.minute = (Time_Time.minute == 0) ? 59 : Time_Time.minute - 1;
			Time_updateGuiStr();
			Menu_displayString(MENU__LINE_MESSAGE_2, "   ");
			Menu_displayString(MENU__LINE_MESSAGE_2, Time_MinuteStr);
			break;

		case 2:
			Time_Time.second = (Time_Time.second == 0) ? 59 : Time_Time.second - 1;
			Time_updateGuiStr();
			Menu_displayString(MENU__LINE_MESSAGE_3, "   ");
			Menu_displayString(MENU__LINE_MESSAGE_3, Time_SecondStr);
			break;
	}
}

void Time_updateGuiStr(void)
{
	Time_HourStr[0] = '0' + (Time_Time.hour / 10);
	Time_HourStr[1] = '0' + (Time_Time.hour % 10);

	Time_MinuteStr[0] = '0' + (Time_Time.minute / 10);
	Time_MinuteStr[1] = '0' + (Time_Time.minute % 10);

	Time_SecondStr[0] = '0' + (Time_Time.second / 10);
	Time_SecondStr[1] = '0' + (Time_Time.second % 10);
}

void Time_configureRtc(void)
{
  /* Configure LSE as RTC clock source */
  RST_CLK_LSEconfig(RST_CLK_LSE_ON);
  /* Wait till LSE is ready */
  while (RST_CLK_LSEstatus() != SUCCESS)
  {
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
