#include "module_pwr.h"
#include "module_pwrsts.h"
#include "module_pwrmng.h"

// Pwr

// Pwr typedefs

typedef enum { Pwr_DeviceState_off = 0, Pwr_DeviceState_on = !Pwr_DeviceState_off } DeviceState;

typedef struct
{
	DeviceState state;
  unsigned int last_turn_on_timepoint;
  unsigned int worktime;
	unsigned int previous_worktime;
} Pwr_Device;

// Pwr private function prototypes

static void Pwr_update(void);

// Pwr private variables

static Pwr_Device Pwr_Devices[PWR__N_DEVICES] = { { Pwr_DeviceState_off, 0, 0, 0 }
																								, { Pwr_DeviceState_off, 0, 0, 0 }
																								, { Pwr_DeviceState_off, 0, 0, 0 }
																								, { Pwr_DeviceState_off, 0, 0, 0 }
	
  																							, { Pwr_DeviceState_off, 0, 0, 0 }
																								, { Pwr_DeviceState_off, 0, 0, 0 }
																								, { Pwr_DeviceState_off, 0, 0, 0 }
																								, { Pwr_DeviceState_off, 0, 0, 0 }
																								};
static size_t Pwr_DeviceIndex = 0;

static uint8_t Pwr_DeviceStr[] = {'D', 'e', 'v', 'i', 'c', 'e', ' ', '0', '\0'};
static size_t Pwr_DeviceStrLength = MENU__COUNT_OF(Pwr_DeviceStr);

static uint8_t Pwr_StateStr[] = {'S', 't', 'a', 't', 'e', ' ', 'o', 'f', 'f', '\0'};
static size_t Pwr_StateStrLength = MENU__COUNT_OF(Pwr_StateStr);

// Pwr public functions

void Pwr_init(void) {}

void Pwr_setCurrentDevice(int device)
{
	if (device < 0) Pwr_DeviceIndex = 0;
	else if (device >= PWR__N_DEVICES) Pwr_DeviceIndex = PWR__N_DEVICES - 1;
	else Pwr_DeviceIndex = device;
	
	Pwr_update();
}

// Pwr private functions

void Pwr_update(void)
{
	Pwr_DeviceStr[Pwr_DeviceStrLength - 2] = '0' + Pwr_DeviceIndex + 1;

	Pwr_StateStr[Pwr_StateStrLength - 3] = Pwr_Devices[Pwr_DeviceIndex].state ? 'n' : 'f';
	Pwr_StateStr[Pwr_StateStrLength - 2] = Pwr_Devices[Pwr_DeviceIndex].state ? ' ' : 'f';
}

// Pwrsts

// Pwrsts private functions prototypes

static void Pwrsts_update(void);
static void Pwrsts_updateProc(void);

// Pwrsts private variables

// TODO
static uint8_t Pwrsts_WorktimeStr[] = "WorkT: ..h..m..s.";
static size_t Pwrsts_WorktimeStrLength = MENU__COUNT_OF(Pwrsts_WorktimeStr);

// TODO
static uint8_t Pwrsts_PreviousWorktimeStr[] = "PWorkT: ..h..m..s";
static size_t Pwrsts_PreviousWorktimeStrLength = MENU__COUNT_OF(Pwrsts_PreviousWorktimeStr);


static Menu_MenuItem Pwrsts_StatsMenuItems[] = { { Pwr_DeviceStr, 							NULL, NULL }
																								, { Pwr_StateStr, 	  					NULL, NULL }
																								, { Pwrsts_WorktimeStr, 	  		NULL, NULL }
																								, { Pwrsts_PreviousWorktimeStr, NULL, NULL }
																								, { "Up:update|Sel:return", 		NULL, NULL }
																								};

Menu_Menu Pwrsts_StatsMenu = { "Power stats"
														, Pwrsts_StatsMenuItems
														, MENU__COUNT_OF(Pwrsts_StatsMenuItems)
														, 0
														, Pwrsts_updateProc
														, NULL
														, NULL
														};

// Pwrsts public functions

void Pwrsts_init(Menu_Procedure * return_proc)
{
	Pwrsts_StatsMenu.select = return_proc;
}

void Pwrsts_setDevice0(void) { Pwr_setCurrentDevice(PWR__DEVICE_0); Pwrsts_update(); }
void Pwrsts_setDevice1(void) { Pwr_setCurrentDevice(PWR__DEVICE_1); Pwrsts_update(); }
void Pwrsts_setDevice2(void) { Pwr_setCurrentDevice(PWR__DEVICE_2); Pwrsts_update(); }
void Pwrsts_setDevice3(void) { Pwr_setCurrentDevice(PWR__DEVICE_3); Pwrsts_update(); }

void Pwrsts_setDevice4(void) { Pwr_setCurrentDevice(PWR__DEVICE_4); Pwrsts_update(); }
void Pwrsts_setDevice5(void) { Pwr_setCurrentDevice(PWR__DEVICE_5); Pwrsts_update(); }
void Pwrsts_setDevice6(void) { Pwr_setCurrentDevice(PWR__DEVICE_6); Pwrsts_update(); }
void Pwrsts_setDevice7(void) { Pwr_setCurrentDevice(PWR__DEVICE_7); Pwrsts_update(); }

// Pwrsts private functions

void Pwrsts_update(void)
{
	// Update Worktime : 
	// Update Previous worktime
}

void Pwrsts_updateProc(void)
{
	Pwrsts_update();
	Menu_displayMenu(&Pwrsts_StatsMenu);
}

// Pwrmng

// Pwrsts private functions prototypes

static void Pwrmng_turnOnProc(void);
static void Pwrmng_turnOffProc(void);

static void Pwrmng_upProc(void);
static void Pwrmng_selectProc(void);
static void Pwrmng_downProc(void);

// Pwrmng private variables

#define ReturnItemIndex 4
static Menu_MenuItem Pwrmng_PowerMenuItems[] = { { Pwr_DeviceStr, NULL, NULL }
																								, { Pwr_StateStr, NULL, NULL }
																								, { "Turn on", 		NULL, Pwrmng_turnOnProc }
																								, { "Turn off", 	NULL, Pwrmng_turnOffProc }
																								, { "Return", 		NULL, NULL }
																								};

																								
Menu_Menu Pwrmng_PowerMenu = { "Power stats"
														, Pwrmng_PowerMenuItems
														, MENU__COUNT_OF(Pwrmng_PowerMenuItems)
														, 0
														, Pwrmng_upProc
														, Pwrmng_selectProc
														, Pwrmng_downProc
														};

// Pwrmng public functions

void Pwrmng_init(Menu_Procedure * return_proc)
{
	Pwrmng_PowerMenuItems[ReturnItemIndex].proc = return_proc;
}

void Pwrmng_setDevice0(void) { Pwr_setCurrentDevice(PWR__DEVICE_0); }
void Pwrmng_setDevice1(void) { Pwr_setCurrentDevice(PWR__DEVICE_1); }
void Pwrmng_setDevice2(void) { Pwr_setCurrentDevice(PWR__DEVICE_2); }
void Pwrmng_setDevice3(void) { Pwr_setCurrentDevice(PWR__DEVICE_3); }

void Pwrmng_setDevice4(void) { Pwr_setCurrentDevice(PWR__DEVICE_4); }
void Pwrmng_setDevice5(void) { Pwr_setCurrentDevice(PWR__DEVICE_5); }
void Pwrmng_setDevice6(void) { Pwr_setCurrentDevice(PWR__DEVICE_6); }
void Pwrmng_setDevice7(void) { Pwr_setCurrentDevice(PWR__DEVICE_7); }

// Pwrmng private functions

void Pwrmng_turnOffProc(void)
{
	// write to value port
  // write to address ports
	// make 0->1
	// update Pwr_Devices[Pwr_DeviceIndex].worktime
	// update Pwr_Devices[Pwr_DeviceIndex].state
	Pwr_Devices[Pwr_DeviceIndex].state = Pwr_DeviceState_off;
	
	Pwr_update();
	Menu_displayMenu(&Pwrmng_PowerMenu);
}

void Pwrmng_turnOnProc(void)
{
	// write to value port
  // write to address ports
	// make 0->1
	// update Pwr_Devices[Pwr_DeviceIndex].last_turn_on_timepoint
	// update Pwr_Devices[Pwr_DeviceIndex].state
	Pwr_Devices[Pwr_DeviceIndex].state = Pwr_DeviceState_on;
	
	Pwr_update();
	Menu_displayMenu(&Pwrmng_PowerMenu);
}

void Pwrmng_upProc(void)
{
	Menu_selectUpperItem(&Pwrmng_PowerMenu);
}
	
void Pwrmng_selectProc(void)
{
	size_t index = Pwrmng_PowerMenu.item_index;
	Menu_Procedure * proc = Pwrmng_PowerMenu.items[index].proc;
	if (proc != NULL)
	{
		proc();
	}
}
	
void Pwrmng_downProc(void)
{
	Menu_selectLowerItem(&Pwrmng_PowerMenu);
}
