#include "module_app.h"

#include "module_demo.h"
#include "module_menu.h"
#include "module_pwr.h"
#include "module_time.h"
#include "module_interrupt.h"
#include "module_uart.h"

#include "joystick.h"

#include <stddef.h>
#include <string.h>
//#include "types.h"
//#include "lcd.h"
//#include "gl.h"
//#include "text.h"
//#include "leds.h"
//#include "menu.h"
//#include "time.h"
//#include "lowpower.h"
//#include "MDR32F9Qx_port.h"
//#include "Demo_Init.h"
//#include "rtc.h"
//#include "ff.h"
//#include "diskio.h"
//#include "systick.h"
//#include <string.h>

// Private function prototypes

static void App_up(void);
static void App_select(void);
static void App_down(void);
static void App_update(void);

static void App_updateGui(void);

static void App_upProc(void);
static void App_selectProc(void);
static void App_downProc(void);
static void App_returnProc(void);
static void App_sendStatsProc(void);

static bool App_isValidCurrentMenu(void);

static void App_sendStats(void);
static void App_updateStats(void);

static void App_resetStats(uint32_t old_time);

static void App_testProc(void);

static void App_resolveCommands(void);

// Variables

#define APP__MAX_MENU_LEVELS 4

static Menu_Menu *App_CurrentMenu;
static Menu_Menu *App_PreviousMenus[APP__MAX_MENU_LEVELS];
static size_t App_MenuLevel;
static volatile bool App_UpdateGuiSoon = FALSE;

/* Main menu */
static Menu_MenuItem App_MainMenuItems[] = {{"Devices", &Pwr_DeviceMenu, NULL},
                                            {"SetTime", &Time_SetTimeMenu, NULL},
                                            {"Force Send", NULL, App_sendStatsProc},
                                            {"Test", NULL, App_testProc}};

static Menu_Menu MainMenu = {"Main menu",
                             App_MainMenuItems,
                             UTILITY__COUNT_OF(App_MainMenuItems),
                             0,
                             App_upProc,
                             App_selectProc,
                             App_downProc,
                             NULL};

static int App_SendStatsCounter = 1;

// Public functions

void App_init(void)
{
    Demo_init();
    Time_init(App_returnProc, App_resetStats);

    Pwr_StdUpProc = App_upProc;
    Pwr_StdSelectProc = App_selectProc;
    Pwr_StdDownProc = App_downProc;
    Pwr_init(App_returnProc, App_updateStats);

    Uart_init();

    App_CurrentMenu = &MainMenu;
    App_MenuLevel = 0;
    App_PreviousMenus[App_MenuLevel] = App_CurrentMenu;

    if (App_isValidCurrentMenu()) Menu_displayMenu(App_CurrentMenu);
}

void App_run(void)
{
    uint32_t key = NOKEY;
    uint32_t last_key = NOKEY;

    while (TRUE) {
        switch (key) {
            case SEL:
                App_select();
                break;
            case UP:
                App_up();
                break;
            case DOWN:
                App_down();
                break;
        }

        do {
            App_update();

            last_key = key;
            key = GetKey();
        } while (key == last_key);
    }
}

void App_rtcSecondHandle(void)
{
    uint32_t time;

    App_updateGui();

    /* If counter is equal to 86400: one day was elapsed */
    time = Time_getRawTime();
    if (time == 24 * 60 * 60) {
        Time_setRawTime(0);
        Time_addPassedDay();
    }
}

// Private functions

void App_up(void)
{
    if (!App_isValidCurrentMenu() || App_CurrentMenu->up == NULL) return;

    App_CurrentMenu->up();
}

void App_select(void)
{
    if (!App_isValidCurrentMenu() || App_CurrentMenu->select == NULL) return;

    App_CurrentMenu->select();
}

void App_down(void)
{
    if (!App_isValidCurrentMenu() || App_CurrentMenu->down == NULL) return;

    App_CurrentMenu->down();
}

void App_update(void)
{
    App_resolveCommands();

    if (App_UpdateGuiSoon) {
        App_updateStats();

        if (!App_isValidCurrentMenu() || App_CurrentMenu->update == NULL) return;

        App_CurrentMenu->update();

        App_UpdateGuiSoon = FALSE;
    }
}

void App_updateGui(void)
{
    App_UpdateGuiSoon = TRUE;
}

// Common procedures for menu interaction

void App_upProc(void)
{
    if (!App_isValidCurrentMenu()) return;
    Menu_selectUpperItem(App_CurrentMenu);
}

void App_selectProc(void)
{
    Menu_MenuItem *menu_item;

    if (!App_isValidCurrentMenu()) return;

    menu_item = &App_CurrentMenu->items[App_CurrentMenu->item_index];

    if (menu_item->proc != NULL) {
        menu_item->proc();
    }

    if (App_MenuLevel < APP__MAX_MENU_LEVELS - 1 && menu_item->submenu != NULL) {
        App_CurrentMenu = menu_item->submenu;
        Menu_displayMenu(App_CurrentMenu);
        ++App_MenuLevel;
        App_PreviousMenus[App_MenuLevel] = App_CurrentMenu;
    }
}

void App_downProc(void)
{
    if (!App_isValidCurrentMenu()) return;
    Menu_selectLowerItem(App_CurrentMenu);
}

void App_returnProc(void)
{
    if (App_MenuLevel == 0) return;

    App_CurrentMenu = App_PreviousMenus[App_MenuLevel - 1];
    --App_MenuLevel;

    Menu_displayMenu(App_CurrentMenu);
}

void App_sendStatsProc(void)
{
    Uart_putString(Pwr_getStats());
    Uart_sendData();
}

bool App_isValidCurrentMenu(void)
{
    size_t i;

    if (App_CurrentMenu == NULL || APP__MAX_MENU_LEVELS <= App_MenuLevel) return FALSE;

    for (i = 0; i <= App_MenuLevel; ++i) {
        if (App_PreviousMenus[i] == NULL || !Menu_isValidMenu(App_PreviousMenus[i])) return FALSE;
    }

    return TRUE;
}

void App_sendStats(void)
{
    if (Uart_putWholeString(Pwr_getStats())) {
        if (App_SendStatsCounter < 0) {
            App_SendStatsCounter = -App_SendStatsCounter + 1;
        } else {
            ++App_SendStatsCounter;
        }
        Uart_sendData();
    } else if (0 < App_SendStatsCounter) {
        App_SendStatsCounter = -App_SendStatsCounter;
    }
}

#define APP__TWELVE_HOURS (12 * 60 * 60)
void App_updateStats(void)
{
    uint32_t time, n_passed_days;

    time = Time_getRawTime();
    n_passed_days = Time_getNPassedDays();

    if (0 < n_passed_days) {
        if (APP__TWELVE_HOURS <= time) {
            App_SendStatsCounter = 2;
        } else {
            App_SendStatsCounter = 1;
        }
    }

    if (App_SendStatsCounter < 0) {
        App_sendStats();
    }

    if ((App_SendStatsCounter == 1) || (App_SendStatsCounter == 2 && APP__TWELVE_HOURS <= time)) {
        App_sendStats();
        Pwr_checkPoint(time);
    } else {
        Pwr_updateStats(time);
    }
}

void App_resetStats(uint32_t old_time)
{
    uint32_t new_time = Time_getRawTime();

    if (APP__TWELVE_HOURS <= new_time) {
        App_SendStatsCounter = 2;
    } else {
        App_SendStatsCounter = 1;
    }

    Pwr_updateStats(old_time);
    Pwr_resetStats(new_time);
}

void App_testProc(void)
{
    Pwr_toggleTestWaitTicks();
}

typedef enum {
    App_ResolveCommandState_init,
    App_ResolveCommandState_device,
    App_ResolveCommandState_device_x
} App_ResolveCommandState;

// S - send stats
// D0n - turn Device 0 oN
// other - clear buffer
void App_resolveCommands(void)
{
    static App_ResolveCommandState state = App_ResolveCommandState_init;
    static unsigned int chosen_device;
    Uart_MaybeReceivedChar maybe;
	
    Uart_receiveData();
    while (maybe = Uart_getChar(), maybe.received) {
        uint8_t ch = maybe.received_data.ch;

        if (maybe.received_data.error != UART__RECIEVE_ERROR_NO_ERROR) {
            continue;
        }

        if (state == App_ResolveCommandState_init) {
            if (ch == 'S') {
                App_sendStatsProc();
            } else if (ch == 'D') {
                state = App_ResolveCommandState_device;
            } else {
                Uart_putString("Unknown command: ");
                Uart_putChar(ch);
                Uart_putString("\r\nAvailable: 'S' - send stats, 'D' - choose device\r\n");
            }
        } else if (state == App_ResolveCommandState_device) {
            if ('0' < ch && ch < '9') {
                chosen_device = ch - '1';
                state = App_ResolveCommandState_device_x;
            } else {
                state = App_ResolveCommandState_init;
                Uart_putString("Unknown device number: ");
                Uart_putChar(ch);
                Uart_putString("\r\nAvailable: [1, 8]\r\n");
            }
        } else if (state == App_ResolveCommandState_device_x) {
            int old_device = Pwr_currentDevice();
            Pwr_setCurrentDevice(chosen_device);

            if (ch == 'n') {
                Pwr_turnDeviceOn();
            } else if (ch == 'f') {
                Pwr_turnDeviceOff();
            } else {
                Uart_putString("Unknown turn command: ");
                Uart_putChar(ch);
                Uart_putString("\r\nAvailable: 'n' - on, 'f' - off\r\n");
            }

            state = App_ResolveCommandState_init;
            Pwr_setCurrentDevice(old_device);
        }
    }
    Uart_sendData();
}
