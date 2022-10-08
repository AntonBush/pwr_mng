#include "module_pwr.h"
#include "module_pwrsts.h"
#include "module_pwrmng.h"

// Pwr

// Pwr typedefs

typedef enum { Pwr_DeviceState_off = 0, Pwr_DeviceState_on = !Pwr_DeviceState_off } Pwr_DeviceState;

typedef struct
{
	Pwr_DeviceState state;
  unsigned int last_turn_on_timepoint;
  unsigned int worktime;
	unsigned int previous_worktime;
	unsigned int address;
} Pwr_Device;

// Pwr private function prototypes

static void Pwr_update(void);
static void Pwr_waitFor(unsigned int ticks);

// Pwr private variables

static Pwr_Device Pwr_Devices[PWR__N_DEVICES] = { { Pwr_DeviceState_off, 0, 0, 0, PWR__DEVICE_0 }
																								, { Pwr_DeviceState_off, 0, 0, 0, PWR__DEVICE_1 }
																								, { Pwr_DeviceState_off, 0, 0, 0, PWR__DEVICE_2 }
																								, { Pwr_DeviceState_off, 0, 0, 0, PWR__DEVICE_3 }

																								, { Pwr_DeviceState_off, 0, 0, 0, PWR__DEVICE_4 }
																								, { Pwr_DeviceState_off, 0, 0, 0, PWR__DEVICE_5 }
																								, { Pwr_DeviceState_off, 0, 0, 0, PWR__DEVICE_6 }
																								, { Pwr_DeviceState_off, 0, 0, 0, PWR__DEVICE_7 }
																								};
static size_t Pwr_DeviceIndex = 0;

static uint8_t Pwr_DeviceStr[] = { 'D', 'e', 'v', 'i', 'c', 'e', ' ', '0', '\0' };
static size_t Pwr_DeviceStrLength = MENU__COUNT_OF(Pwr_DeviceStr);

static uint8_t Pwr_StateStr[] = { 'S', 't', 'a', 't', 'e', ' ', 'o', 'f', 'f', '\0' };
static size_t Pwr_StateStrLength = MENU__COUNT_OF(Pwr_StateStr);

// Pwr public functions

void Pwr_init(void)
{
	unsigned int device_index;

	PORT_ResetBits(PWR__DATA_PORT, PWR__DATA_PIN);
	PORT_ResetBits(PWR__CMD_PORT, PWR__CMD_PIN);

	for (device_index = 0; device_index < PWR__N_DEVICES; ++device_index)
	{
		PORT_ResetBits(PWR__ADDR_PORT, PWR__ADDR_PINS);
		PORT_SetBits(PWR__ADDR_PORT, Pwr_Devices[device_index].address);

		Pwr_waitFor(100);

		// Do 0->1 edge
		PORT_SetBits(PWR__CMD_PORT, PWR__CMD_PIN);
		Pwr_waitFor(100);
		PORT_ResetBits(PWR__CMD_PORT, PWR__CMD_PIN);
	}
}

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

void Pwr_waitFor(unsigned int ticks)
{
	unsigned int i;
	for (i = 0; i < ticks; ++i)
	{
		__NOP();
	}
}

// Pwrsts

// Pwrsts private functions prototypes

static void Pwrsts_updateProc(void);

// Pwrsts private variables

static uint8_t Pwrsts_WorktimeStr[] = { 'W', 'o', 'r', 'k', 'T'
																			, ':', ' ', '.', '.', 'h'
																			, '.', '.', 'm', '.', '.'
																			, 's', ' ', '\0'
																			};
// h: 7, 8
// m: 10, 11
// s: 13, 14
static size_t Pwrsts_WorktimeStrLength = MENU__COUNT_OF(Pwrsts_WorktimeStr);

static uint8_t Pwrsts_PreviousWorktimeStr[] = { 'P', 'W', 'o', 'r', 'k'
																							, 'T', ':', ' ', '.', '.'
																							, 'h', '.', '.', 'm', '.'
																							, '.', 's', '\0'
																							};
// h: 8, 9
// m: 11, 12
// s: 14, 15
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
															, NULL
															};

// Pwrsts public functions

void Pwrsts_init(Menu_Procedure * return_proc)
{
	Pwrsts_StatsMenu.select = return_proc;
}

void Pwrsts_update(void)
{
	// Process stats of A_L_L devices
	// Process 12 am and pm
	// Update Worktime :
	// Update Previous worktime
}

void Pwrsts_setDevice0(void) { Pwr_setCurrentDevice(PWR__DEVICE_0); }
void Pwrsts_setDevice1(void) { Pwr_setCurrentDevice(PWR__DEVICE_1); }
void Pwrsts_setDevice2(void) { Pwr_setCurrentDevice(PWR__DEVICE_2); }
void Pwrsts_setDevice3(void) { Pwr_setCurrentDevice(PWR__DEVICE_3); }

void Pwrsts_setDevice4(void) { Pwr_setCurrentDevice(PWR__DEVICE_4); }
void Pwrsts_setDevice5(void) { Pwr_setCurrentDevice(PWR__DEVICE_5); }
void Pwrsts_setDevice6(void) { Pwr_setCurrentDevice(PWR__DEVICE_6); }
void Pwrsts_setDevice7(void) { Pwr_setCurrentDevice(PWR__DEVICE_7); }

// Pwrsts private functions

void Pwrsts_updateProc(void)
{
	unsigned int worktime = Pwr_Devices[Pwr_DeviceIndex].worktime;
	unsigned int previous_worktime = Pwr_Devices[Pwr_DeviceIndex].previous_worktime;
	
	unsigned int worktime_hours;
	unsigned int worktime_minutes;
	unsigned int worktime_seconds;

	unsigned int previous_worktime_hours;
	unsigned int previous_worktime_minutes;
	unsigned int previous_worktime_seconds;

	worktime_hours = worktime / (60 * 60);
	worktime %= 60 * 60;
	worktime_minutes = worktime / 60;
	worktime_seconds = worktime % 60;

	previous_worktime_hours = worktime / (60 * 60);
	previous_worktime %= 60 * 60;
	previous_worktime_minutes = worktime / 60;
	previous_worktime_seconds = worktime % 60;

	// hours
	Pwrsts_WorktimeStr[13] = '0' + (worktime_hours / 10 % 10);
	Pwrsts_WorktimeStr[14] = '0' + (worktime_hours % 10);

	// minutes
	Pwrsts_WorktimeStr[10] = '0' + (worktime_minutes / 10 % 10);
	Pwrsts_WorktimeStr[11] = '0' + (worktime_minutes % 10);

	// seconds
	Pwrsts_WorktimeStr[7] = '0' + (worktime_seconds / 10 % 10);
	Pwrsts_WorktimeStr[8] = '0' + (worktime_seconds % 10);

	// hours
	Pwrsts_PreviousWorktimeStr[14] = '0' + (previous_worktime_hours / 10 % 10);
	Pwrsts_PreviousWorktimeStr[15] = '0' + (previous_worktime_hours % 10);

	// minutes
	Pwrsts_PreviousWorktimeStr[11] = '0' + (previous_worktime_minutes / 10 % 10);
	Pwrsts_PreviousWorktimeStr[12] = '0' + (previous_worktime_minutes % 10);

	// seconds
	Pwrsts_PreviousWorktimeStr[8] = '0' + (previous_worktime_seconds / 10 % 10);
	Pwrsts_PreviousWorktimeStr[9] = '0' + (previous_worktime_seconds % 10);

	Menu_displayMenu(&Pwrsts_StatsMenu);
}

// Pwrmng

// Pwrsts private functions prototypes

static void Pwrmng_turnDevice(Pwr_DeviceState state);
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
															, NULL
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

void Pwrmng_turnDevice(Pwr_DeviceState state)
{
	// CMD = 0
	PORT_ResetBits(PWR__CMD_PORT, PWR__CMD_PIN);
	Pwr_waitFor(100);

	// ADDR = current device
	PORT_ResetBits(PWR__ADDR_PORT, PWR__ADDR_PINS);
	PORT_SetBits(PWR__ADDR_PORT, Pwr_Devices[Pwr_DeviceIndex].address);

	if (state)
	{
		// DATA = 1
		PORT_SetBits(PWR__DATA_PORT, PWR__DATA_PIN);
	} else {
		// DATA = 0
		PORT_ResetBits(PWR__DATA_PORT, PWR__DATA_PIN);
	}

	// C = 0->1
	PORT_SetBits(PWR__CMD_PORT, PWR__CMD_PIN);
	Pwr_waitFor(100);

	// C = 0
	PORT_ResetBits(PWR__CMD_PORT, PWR__CMD_PIN);
}

void Pwrmng_turnOffProc(void)
{
	Pwrmng_turnDevice(Pwr_DeviceState_off);

	// update Pwr_Devices[Pwr_DeviceIndex].worktime
	// update Pwr_Devices[Pwr_DeviceIndex].last_turn_on_timepoint
	Pwr_Devices[Pwr_DeviceIndex].state = Pwr_DeviceState_off;

	Pwr_update();
	Menu_displayMenu(&Pwrmng_PowerMenu);
}

void Pwrmng_turnOnProc(void)
{
	Pwrmng_turnDevice(Pwr_DeviceState_on);

	// update Pwr_Devices[Pwr_DeviceIndex].worktime
	// update Pwr_Devices[Pwr_DeviceIndex].last_turn_on_timepoint
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
