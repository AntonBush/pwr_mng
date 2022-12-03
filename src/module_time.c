#include "module_time.h"

#include "MDR32FxQI_bkp.h"              // Milandr::Drivers:BKP
#include "MDR32FxQI_rst_clk.h"          // Milandr::Drivers:RST_CLK

// Private functions prototypes

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
static void Time_updateEdit(void);

static void Time_updateGuiStr(void);

// Variables

static Time_TimeEdit Time_Time = {0, 0, 0};

static uint8_t Time_HourStr[] = {'0', '0', 'h', '\0'};
// static size_t Time_HourStrLength = UTILITY__COUNT_OF(Time_HourStr);

static uint8_t Time_MinuteStr[] = {'0', '0', 'm', '\0'};
// static size_t Time_MinuteStrLength = UTILITY__COUNT_OF(Time_MinuteStr);

static uint8_t Time_SecondStr[] = {'0', '0', 's', '\0'};
// static size_t Time_SecondStrStrLength = UTILITY__COUNT_OF(Time_SecondStr);

static uint8_t Time_TimeStr[] = {'.', '.', 'h', '.', '.', 'm', '.', '.', 's', '\0'};
// static size_t Time_TimeStrLength = UTILITY__COUNT_OF(Time_TimeStr);

static uint32_t Time_NPassedDays = 0;

#define TIME__RETURN_ITEM_INDEX 4
static Menu_MenuItem Time_SetTimeMenuItems[] = {{Time_HourStr, NULL, Time_changeTimeProc}, {Time_MinuteStr, NULL, Time_changeTimeProc}, {Time_SecondStr, NULL, Time_changeTimeProc}, {"Save", NULL, Time_saveProc}, {Time_TimeStr, NULL, NULL}};

Menu_Menu Time_SetTimeMenu = {"Set time", Time_SetTimeMenuItems, UTILITY__COUNT_OF(Time_SetTimeMenuItems), 0, Time_upProc, Time_selectProc, Time_downProc, Time_updateProc};
static Utility_ProcedureUint32 *Time_TimeSetCallback = NULL;

// Public functions

Time_TimeEdit Time_timeEdit(uint32_t raw_time)
{
    Time_TimeEdit time;

    time.hour = raw_time / 3600;
    time.minute = raw_time / 60 % 60;
    time.second = raw_time % 60;

    return time;
}

void Time_init(Utility_Procedure *return_proc, Utility_ProcedureUint32 *time_set_callback)
{
    unsigned int i;
    uint32_t tmp;

    Time_SetTimeMenuItems[TIME__RETURN_ITEM_INDEX].proc = return_proc;
    Time_TimeSetCallback = time_set_callback;

    // if backup was reset (first run)
    if (MDR_BKP->REG_00 != 0x1234) {
        Time_setRawTime(0);

        MDR_BKP->REG_00 = 0x1234;
        /* Wait until last write operation on RTC registers has finished */
        BKP_RTC_WaitForUpdate();
    } else if (tmp = Time_getRawTime(), tmp / 86400 != 0) {
        for (i = 0; i < tmp / 86400; ++i) {
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
    return Time_timeEdit(Time_getRawTime());
}

void Time_setRawTime(uint32_t time)
{
    uint32_t old_time = Time_getRawTime();

    /* Wait until last write operation on RTC registers has finished */
    BKP_RTC_WaitForUpdate();
    /* Set the RTC counter value */
    BKP_RTC_SetCounter(time);
    /* Wait until last write operation on RTC registers has finished */
    BKP_RTC_WaitForUpdate();

    if (Time_TimeSetCallback != NULL) {
        Time_TimeSetCallback(old_time);
    }
}

void Time_setTime(Time_TimeEdit time)
{
    if ((23 < time.hour) || (59 < time.minute) || (59 < time.second)) return;

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

void Time_upProc(void)
{
    Menu_selectUpperItem(&Time_SetTimeMenu);
}

void Time_selectProc(void)
{
    size_t index = Time_SetTimeMenu.item_index;
    Utility_Procedure *proc = Time_SetTimeMenu.items[index].proc;
    if (proc != NULL) {
        proc();
    }
}

void Time_downProc(void)
{
    Menu_selectLowerItem(&Time_SetTimeMenu);
}

void Time_updateProc(void)
{
    uint8_t style = LCD__STYLE_NO_STYLE;

    Time_TimeEdit time = Time_getTime();

    Time_TimeStr[0] = '0' + (time.hour / 10 % 10);
    Time_TimeStr[1] = '0' + (time.hour % 10);

    Time_TimeStr[3] = '0' + (time.minute / 10 % 10);
    Time_TimeStr[4] = '0' + (time.minute % 10);

    Time_TimeStr[6] = '0' + (time.second / 10 % 10);
    Time_TimeStr[7] = '0' + (time.second % 10);

    if (Time_SetTimeMenu.item_index == TIME__RETURN_ITEM_INDEX) {
        style |= LCD__STYLE_HIGHLIGHT;
    }

    Lcd_displayString(Lcd_Line_line5, Time_TimeStr, style);
}

void Time_highlightItem(void)
{
    int8_t style = LCD__STYLE_NO_OVERRIDE | LCD__STYLE_HIGHLIGHT;
    size_t index = Time_SetTimeMenu.item_index;

    switch (index) {
        case 0:
        case 1:
        case 2:
            Lcd_displayString(Lcd_Lines[index + 1], "   ", style);
            break;
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

void Time_increaseProc(void)
{
    switch (Time_SetTimeMenu.item_index) {
        case 0:
            Time_Time.hour = (Time_Time.hour + 1) % 24;
            break;

        case 1:
            Time_Time.minute = (Time_Time.minute + 1) % 60;
            break;

        case 2:
            Time_Time.second = (Time_Time.second + 1) % 60;
            break;

        default:
            return;
    }

    Time_updateEdit();
}

void Time_reduceProc(void)
{
    switch (Time_SetTimeMenu.item_index) {
        case 0:
            Time_Time.hour = (Time_Time.hour == 0) ? 23 : Time_Time.hour - 1;
            break;

        case 1:
            Time_Time.minute = (Time_Time.minute == 0) ? 59 : Time_Time.minute - 1;
            break;

        case 2:
            Time_Time.second = (Time_Time.second == 0) ? 59 : Time_Time.second - 1;
            break;
    }

    Time_updateEdit();
}

void Time_updateEdit(void)
{
    Time_updateGuiStr();
    switch (Time_SetTimeMenu.item_index) {
        case 0:
            Lcd_displayString(Lcd_Line_line1, Time_HourStr, LCD__STYLE_NO_STYLE);
            break;

        case 1:
            Lcd_displayString(Lcd_Line_line2, Time_MinuteStr, LCD__STYLE_NO_STYLE);
            break;

        case 2:
            Lcd_displayString(Lcd_Line_line3, Time_SecondStr, LCD__STYLE_NO_STYLE);
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
