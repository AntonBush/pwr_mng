#include "module_app.h"

#include "module_menu.h"

#include <stddef.h>
//#include "types.h"
//#include "lcd.h"
//#include "gl.h"
#include "text.h"
//#include "leds.h"
//#include "menu.h"
//#include "time.h"
//#include "lowpower.h"
#include "MDR32F9Qx_port.h"
#include "Demo_Init.h"
//#include "rtc.h"
//#include "ff.h"
//#include "diskio.h"
//#include "systick.h"
//#include <string.h>

#define APP__MAX_MENU_LEVELS 4

static Menu_Menu * App_CurrentMenu;
static Menu_Menu * App_PreviousMenus[APP__MAX_MENU_LEVELS];
static size_t App_MenuLevel;

static void App_upProc(void);
static void App_selectProc(void);
static void App_downProc(void);
static void App_idleProc(void);
static void App_returnProc(void);

static int App_isValidCurrentMenu(void)
{
	size_t i;

	if ( App_CurrentMenu == NULL
			|| APP__MAX_MENU_LEVELS <= App_MenuLevel
			) return 0;
	
	for (i = 0; i <= App_MenuLevel; ++i)
	{
		if ( App_PreviousMenus[i] == NULL
				|| !Menu_isValidMenu(App_PreviousMenus[i])
				) return 0;
	}
	
	return 1;
}

/* Second level menu *//*
struct sMenuItem LCDMenuItems[] = {{"Text", IdleFunc, &TextMenu},
                                   {"Graphics", IdleFunc, &GraphicMenu},
                                   {"LEDs", IdleFunc, &LEDsMenu},
                                   {"Return", ReturnFunc, NULL}};
struct sMenu LCDMenu = {"Display", LCDMenuItems, countof(LCDMenuItems)};

struct sMenuItem DevMenuItems[] = {{"Communication", IdleFunc, &CommMenu},
                                   {"Other", IdleFunc, &OtherMenu},
                                   {"Return", ReturnFunc, NULL}};
struct sMenu DevMenu = {"Devices", DevMenuItems, countof(DevMenuItems)};

struct sMenuItem uSD_FlashMenuItems[] = {{"Show volume status",     ShowVolumeStatusFunc,   NULL},
                                         {"Read file",              ReadFileFunc,           NULL},
                                         {"Write file",             WriteFileFunc,          NULL},
                                         {"Return",                 ReturnFunc,             NULL}};
struct sMenu uSD_FlashMenu = {"uSD FLASH", uSD_FlashMenuItems, countof(uSD_FlashMenuItems)};*/

/* Main menu */
static Menu_MenuItem App_MainMenuItems[] = { { "Stats", 		  NULL, App_idleProc }
																						, { "Manage", 	  NULL, App_idleProc }
																						, { "Force Send", NULL, App_idleProc }
																						};

static Menu_Menu MainMenu = { "Main menu"
														, App_MainMenuItems
														, MENU__COUNT_OF(App_MainMenuItems)
														, 0
														, App_upProc
														, App_selectProc
														, App_downProc
														};

void App_init(void)
{
  Demo_Init();
  //Calendar_Init();

  App_CurrentMenu = &MainMenu;
	App_MenuLevel = 0;
  App_PreviousMenus[App_MenuLevel] = App_CurrentMenu;
  CurrentFont = &Font_6x8;
	
	if (App_isValidCurrentMenu()) Menu_displayMenu(App_CurrentMenu);
}

void App_up(void)
{
	if ( !App_isValidCurrentMenu()
			|| App_CurrentMenu->up == NULL
			) return;
	
	App_CurrentMenu->up();
}

void App_select(void)
{
	if ( !App_isValidCurrentMenu()
			|| App_CurrentMenu->select == NULL
			) return;
	
	App_CurrentMenu->select();
}

void App_down(void)
{
	if ( !App_isValidCurrentMenu()
			|| App_CurrentMenu->down == NULL
			) return;
	
	App_CurrentMenu->down();
}
	
void App_updateStats(void)
{}

// Common procedures for menu interaction
	
void App_upProc(void)
{
	if (!App_isValidCurrentMenu()) return;
	Menu_selectUpperItem(App_CurrentMenu);
}

void App_selectProc(void)
{
	Menu_MenuItem * menu_item;
	
	if (!App_isValidCurrentMenu()) return;
	
	menu_item = &App_CurrentMenu->items[App_CurrentMenu->item_index];

  if(App_MenuLevel < APP__MAX_MENU_LEVELS - 1 && menu_item->submenu != NULL)
  {
    App_CurrentMenu = menu_item->submenu;
    Menu_displayMenu(App_CurrentMenu);
    ++App_MenuLevel;
    App_PreviousMenus[App_MenuLevel] = App_CurrentMenu;
  }
	
	if (menu_item->proc != NULL)
	{
		menu_item->proc();
	}
}

void App_downProc(void)
{
	if (!App_isValidCurrentMenu()) return;
	Menu_selectLowerItem(App_CurrentMenu);
}

void App_idleProc(void)
{}

void App_returnProc(void)
{
  if(App_MenuLevel == 0) return;

  App_CurrentMenu = App_PreviousMenus[App_MenuLevel - 1];
  --App_MenuLevel;

  Menu_displayMenu(App_CurrentMenu);
}


/* Fourth level menu *//*
struct sMenuItem TimeMenuItems[] = {{"Adjust", TimeAdjustFunc, NULL},
                                    {"Show", TimeShowFunc, NULL},
                                    {"Return", ReturnFunc, NULL}};
struct sMenu TimeMenu = {"Time", TimeMenuItems, countof(TimeMenuItems)};

struct sMenuItem DateMenuItems[] = {{"Adjust", DateAdjustFunc, NULL},
                                    {"Show", DateShowFunc, NULL},
                                    {"Return", ReturnFunc, NULL}};
struct sMenu DateMenu = {"Date", DateMenuItems, countof(DateMenuItems)};

struct sMenuItem AlarmMenuItems[] = {{"Adjust", AlarmAdjustFunc, NULL},
                                     {"Show", AlarmShowFunc, NULL},
                                     {"Return", ReturnFunc, NULL}};
struct sMenu AlarmMenu = {"Alarm", AlarmMenuItems, countof(AlarmMenuItems)};

struct sMenuItem STOPMenuItems[] = {{"Exit on RTC Alarm", STOPMode_RTCAlarm, NULL},
                                    {"Return", ReturnFunc, NULL}};
struct sMenu STOPMenu = {"STOP Mode", STOPMenuItems, countof(STOPMenuItems)};

struct sMenuItem STANDBYMenuItems[] = {
#if !defined (USE_MDR32F9Q3_Rev0) && !defined (USE_MDR32F9Q3_Rev1)
																			 {"Exit on Wakeup", STANDBYMode_WAKEUP, NULL},
#endif
																			 {"Exit on RTC Alarm", STANDBYMode_RTCAlarm, NULL},
                                       {"Return", ReturnFunc, NULL}};
struct sMenu STANDBYMenu = {"STANDBY Mode", STANDBYMenuItems, countof(STANDBYMenuItems)};

struct sMenuItem UARTMenuItems[] = {{"With FIFO", UARTwFIFOFunc, NULL},
                                    {"Without FIFO", UARTFunc, NULL},
                                    {"Return", ReturnFunc, NULL}};
struct sMenu UARTMenu = {"UART", UARTMenuItems, countof(UARTMenuItems)};

struct sMenuItem RTCMenuItems[] = {{"Time", IdleFunc, &TimeMenu},
                                   {"Date", IdleFunc, &DateMenu},
                                   {"Alarm", IdleFunc, &AlarmMenu},
                                   {"Return", ReturnFunc, NULL}};
struct sMenu RTCMenu = {"Real Time Clock", RTCMenuItems, countof(RTCMenuItems)};

struct sMenuItem LowPowerMenuItems[] = {{"STOP", IdleFunc, &STOPMenu},
                                        {"STANDBY", IdleFunc, &STANDBYMenu},
                                        {"Return", ReturnFunc, NULL}};
struct sMenu LowPowerMenu = {"Low Power Mode", LowPowerMenuItems, countof(LowPowerMenuItems)};*/

/* Third level menu *//*
struct sMenuItem TextMenuItems[] = {{"Font", FontFunc, NULL},
                                    {"Style", StyleFunc, NULL},
                                    {"Book", BookFunc, NULL},
                                    {"Return", ReturnFunc, NULL}};
struct sMenu TextMenu = {"Text", TextMenuItems, countof(TextMenuItems)};

struct sMenuItem GraphicMenuItems[] = {{"Elements", ElementsFunc, NULL},
                                       {"Indicators", IndicatorsFunc, NULL},
                                       {"Return", ReturnFunc, NULL}};
struct sMenu GraphicMenu = {"Graphics", GraphicMenuItems, countof(GraphicMenuItems)};

struct sMenuItem LEDsMenuItems[] = {{"On", LightsOnFunc, NULL},
                                    {"Return", ReturnFunc, NULL}};
struct sMenu LEDsMenu = {"LEDs", LEDsMenuItems, countof(LEDsMenuItems)};

struct sMenuItem CommMenuItems[] = {{"UART", IdleFunc, &UARTMenu},
                                    {"USB. Virtual COM", VCOMFunc, NULL},
                                    {"Return", ReturnFunc, NULL}};
struct sMenu CommMenu = {"Communication", CommMenuItems, countof(CommMenuItems)};

struct sMenuItem OtherMenuItems[] = {{"Thermometer", TSENSORFunc, NULL},
                                     {"Real Time Clock", IdleFunc, &RTCMenu},
                                     {"Low Power", IdleFunc, &LowPowerMenu},
                                     {"Return", ReturnFunc, NULL}};
struct sMenu OtherMenu = {"Other devices", OtherMenuItems, countof(OtherMenuItems)};*/

/* Second level menu *//*
struct sMenuItem LCDMenuItems[] = {{"Text", IdleFunc, &TextMenu},
                                   {"Graphics", IdleFunc, &GraphicMenu},
                                   {"LEDs", IdleFunc, &LEDsMenu},
                                   {"Return", ReturnFunc, NULL}};
struct sMenu LCDMenu = {"Display", LCDMenuItems, countof(LCDMenuItems)};

struct sMenuItem DevMenuItems[] = {{"Communication", IdleFunc, &CommMenu},
                                   {"Other", IdleFunc, &OtherMenu},
                                   {"Return", ReturnFunc, NULL}};
struct sMenu DevMenu = {"Devices", DevMenuItems, countof(DevMenuItems)};

struct sMenuItem uSD_FlashMenuItems[] = {{"Show volume status",     ShowVolumeStatusFunc,   NULL},
                                         {"Read file",              ReadFileFunc,           NULL},
                                         {"Write file",             WriteFileFunc,          NULL},
                                         {"Return",                 ReturnFunc,             NULL}};
struct sMenu uSD_FlashMenu = {"uSD FLASH", uSD_FlashMenuItems, countof(uSD_FlashMenuItems)};*/

/* Main menu *//*
struct sMenuItem MainMenuItems[] = {
  {"Display", IdleFunc, &LCDMenu},
  {"Devices", IdleFunc, &DevMenu},
  {"uSD FLASH", IdleFunc, &uSD_FlashMenu},
  {"About", AboutFunc, NULL}};
struct sMenu MainMenu = {"Main menu", MainMenuItems, countof(MainMenuItems)};
*/

