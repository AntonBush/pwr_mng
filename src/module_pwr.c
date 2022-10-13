#include "module_pwr.h"
#include "module_pwrsts.h"
#include "module_pwrmng.h"

#include "module_time.h"
#include "module_utility.h"

// Pwr

// Pwr typedefs

typedef enum {
    Pwr_DeviceState_off = 0,
    Pwr_DeviceState_on = !Pwr_DeviceState_off
} Pwr_DeviceState;

typedef struct
{
    Pwr_DeviceState state;
    unsigned int last_update;
    unsigned int worktime;
    unsigned int previous_worktime;
    unsigned int address;
} Pwr_Device;

// Pwr private function prototypes

static void Pwr_updateGuiStr(void);
static void Pwr_waitFor(unsigned int ticks);

static void Pwr_stdUpProc(void);
static void Pwr_stdSelectProc(void);
static void Pwr_stdDownProc(void);

// Variables

static Pwr_Device Pwr_Devices[PWR__N_DEVICES] = {{Pwr_DeviceState_off, 0, 0, 0, PWR__DEVICE_0},
                                                 {Pwr_DeviceState_off, 0, 0, 0, PWR__DEVICE_1},
                                                 {Pwr_DeviceState_off, 0, 0, 0, PWR__DEVICE_2},
                                                 {Pwr_DeviceState_off, 0, 0, 0, PWR__DEVICE_3},

                                                 {Pwr_DeviceState_off, 0, 0, 0, PWR__DEVICE_4},
                                                 {Pwr_DeviceState_off, 0, 0, 0, PWR__DEVICE_5},
                                                 {Pwr_DeviceState_off, 0, 0, 0, PWR__DEVICE_6},
                                                 {Pwr_DeviceState_off, 0, 0, 0, PWR__DEVICE_7}};
static size_t Pwr_DeviceIndex = 0;

static uint8_t Pwr_DeviceStr[] = {'D', 'e', 'v', 'i', 'c', 'e', ' ', '0', '\0'};
static size_t Pwr_DeviceStrLength = UTILITY__COUNT_OF(Pwr_DeviceStr);

static uint8_t Pwr_StateStr[] = {'S', 't', 'a', 't', 'e', ' ', 'o', 'f', 'f', '\0'};
static size_t Pwr_StateStrLength = UTILITY__COUNT_OF(Pwr_StateStr);

Utility_Procedure *Pwr_StdUpProc = NULL;
Utility_Procedure *Pwr_StdSelectProc = NULL;
Utility_Procedure *Pwr_StdDownProc = NULL;

// Pwr public functions

void Pwr_init(void)
{
    unsigned int device_index;

    PORT_ResetBits(PWR__DATA_PORT, PWR__DATA_PIN);
    PORT_ResetBits(PWR__CMD_PORT, PWR__CMD_PIN);

    for (device_index = 0; device_index < PWR__N_DEVICES; ++device_index) {
        PORT_ResetBits(PWR__ADDR_PORT, PWR__ADDR_PINS);
        PORT_SetBits(PWR__ADDR_PORT, Pwr_Devices[device_index].address);

        Pwr_waitFor(100);

        // Do 0->1 edge
        PORT_SetBits(PWR__CMD_PORT, PWR__CMD_PIN);
        Pwr_waitFor(100);
        PORT_ResetBits(PWR__CMD_PORT, PWR__CMD_PIN);
    }
}

int Pwr_currentDevice(void)
{
    return Pwr_DeviceIndex;
}

void Pwr_setCurrentDevice(int device)
{
    if (device < 0)
        Pwr_DeviceIndex = 0;
    else if (device >= PWR__N_DEVICES)
        Pwr_DeviceIndex = PWR__N_DEVICES - 1;
    else
        Pwr_DeviceIndex = device;
}

// Pwr private functions

void Pwr_updateGuiStr(void)
{
    Pwr_DeviceStr[Pwr_DeviceStrLength - 2] = '0' + Pwr_DeviceIndex + 1;

    Pwr_StateStr[Pwr_StateStrLength - 3] = Pwr_Devices[Pwr_DeviceIndex].state ? 'n' : 'f';
    Pwr_StateStr[Pwr_StateStrLength - 2] = Pwr_Devices[Pwr_DeviceIndex].state ? ' ' : 'f';
}

void Pwr_waitFor(unsigned int ticks)
{
    unsigned int i;
    for (i = 0; i < ticks; ++i) {
        __NOP();
    }
}

static void Pwr_stdUpProc(void)
{
    if (Pwr_StdUpProc != NULL) {
        Pwr_StdUpProc();
    }
}
static void Pwr_stdSelectProc(void)
{
    if (Pwr_StdSelectProc != NULL) {
        Pwr_StdSelectProc();
    }
}
static void Pwr_stdDownProc(void)
{
    if (Pwr_StdDownProc != NULL) {
        Pwr_StdDownProc();
    }
}

// Pwrsts

// Pwrsts private functions prototypes

static void Pwrsts_setDevice0Proc(void);
static void Pwrsts_setDevice1Proc(void);
static void Pwrsts_setDevice2Proc(void);
static void Pwrsts_setDevice3Proc(void);

static void Pwrsts_setDevice4Proc(void);
static void Pwrsts_setDevice5Proc(void);
static void Pwrsts_setDevice6Proc(void);
static void Pwrsts_setDevice7Proc(void);

static void Pwrsts_updateGuiStr(void);
static void Pwrsts_updateProc(void);

// Pwrsts private variables

static uint8_t Pwrsts_WorktimeStr[] = {'W', 'o', 'r', 'k', 'T', ':', ' ', '.', '.', 'h', '.', '.', 'm', '.', '.', 's', ' ', '\0'};
// h: 7, 8
// m: 10, 11
// s: 13, 14
// static size_t Pwrsts_WorktimeStrLength = UTILITY__COUNT_OF(Pwrsts_WorktimeStr);

static uint8_t Pwrsts_PreviousWorktimeStr[] = {'P', 'W', 'o', 'r', 'k', 'T', ':', ' ', '.', '.', 'h', '.', '.', 'm', '.', '.', 's', '\0'};
// h: 8, 9
// m: 11, 12
// s: 14, 15
// static size_t Pwrsts_PreviousWorktimeStrLength = UTILITY__COUNT_OF(Pwrsts_PreviousWorktimeStr);

#define PWRSTS__RETURN_ITEM_INDEX 4
static Menu_MenuItem Pwrsts_MainStatsMenuItems[] = {{Pwr_DeviceStr, NULL, NULL},
                                                    {Pwr_StateStr, NULL, NULL},
                                                    {Pwrsts_WorktimeStr, NULL, NULL},
                                                    {Pwrsts_PreviousWorktimeStr, NULL, NULL},
                                                    {"Up:update|Sel:return", NULL, NULL}};
static Menu_Menu Pwrsts_MainStatsMenu = {"Power stats",
                                         Pwrsts_MainStatsMenuItems,
                                         UTILITY__COUNT_OF(Pwrsts_MainStatsMenuItems),
                                         0,
                                         Pwrsts_updateProc,
                                         NULL,
                                         NULL,
                                         NULL};

static Menu_MenuItem Pwrsts_Stats14MenuItems[] = {{"Device 1", &Pwrsts_MainStatsMenu, Pwrsts_setDevice0Proc},
                                                  {"Device 2", &Pwrsts_MainStatsMenu, Pwrsts_setDevice1Proc},
                                                  {"Device 3", &Pwrsts_MainStatsMenu, Pwrsts_setDevice2Proc},
                                                  {"Device 4", &Pwrsts_MainStatsMenu, Pwrsts_setDevice3Proc},
                                                  {"Return", NULL, NULL}};
static Menu_Menu Pwrsts_Stats14Menu = {"Stats Menu 1-4",
                                       Pwrsts_Stats14MenuItems,
                                       UTILITY__COUNT_OF(Pwrsts_Stats14MenuItems),
                                       0,
                                       Pwr_stdUpProc,
                                       Pwr_stdSelectProc,
                                       Pwr_stdDownProc,
                                       NULL};

static Menu_MenuItem Pwrsts_Stats58MenuItems[] = {{"Device 5", &Pwrsts_MainStatsMenu, Pwrsts_setDevice4Proc},
                                                  {"Device 6", &Pwrsts_MainStatsMenu, Pwrsts_setDevice5Proc},
                                                  {"Device 7", &Pwrsts_MainStatsMenu, Pwrsts_setDevice6Proc},
                                                  {"Device 8", &Pwrsts_MainStatsMenu, Pwrsts_setDevice7Proc},
                                                  {"Return", NULL, NULL}};
static Menu_Menu Pwrsts_Stats58Menu = {"Stats Menu 5-8",
                                       Pwrsts_Stats58MenuItems,
                                       UTILITY__COUNT_OF(Pwrsts_Stats58MenuItems),
                                       0,
                                       Pwr_stdUpProc,
                                       Pwr_stdSelectProc,
                                       Pwr_stdDownProc,
                                       NULL};

static Menu_MenuItem Pwrsts_StatsMenuItems[] = {{"Devices 1-4", &Pwrsts_Stats14Menu, NULL},
                                                {"Devices 5-8", &Pwrsts_Stats58Menu, NULL},
                                                {"Return", NULL, NULL}};
Menu_Menu Pwrsts_StatsMenu = {"Stats Menu",
                              Pwrsts_StatsMenuItems,
                              UTILITY__COUNT_OF(Pwrsts_StatsMenuItems),
                              0,
                              Pwr_stdUpProc,
                              Pwr_stdSelectProc,
                              Pwr_stdDownProc,
                              NULL};

// Pwrsts public functions

void Pwrsts_init(Utility_Procedure *return_proc)
{
    Pwrsts_MainStatsMenu.select = return_proc;
    Pwrsts_Stats14MenuItems[PWRSTS__RETURN_ITEM_INDEX].proc = return_proc;
    Pwrsts_Stats58MenuItems[PWRSTS__RETURN_ITEM_INDEX].proc = return_proc;
    Pwrsts_StatsMenuItems[UTILITY__COUNT_OF(Pwrsts_StatsMenuItems) - 1].proc = return_proc;
}

uint8_t *Pwrsts_getStats(void)
{
    static uint8_t stats[] = {'D', '0', '-', 'f',
                              '\r', '\n',
                              'w', '0', '0', ':', '0', '0', ':', '0', '0',
                              '\r', '\n',
                              'p', '0', '0', ':', '0', '0', ':', '0', '0',
                              '\r', '\n',

                              'D', '1', '-', 'f',
                              '\r', '\n',
                              'w', '0', '0', ':', '0', '0', ':', '0', '0',
                              '\r', '\n',
                              'p', '0', '0', ':', '0', '0', ':', '0', '0',
                              '\r', '\n',

                              'D', '2', '-', 'f',
                              '\r', '\n',
                              'w', '0', '0', ':', '0', '0', ':', '0', '0',
                              '\r', '\n',
                              'p', '0', '0', ':', '0', '0', ':', '0', '0',
                              '\r', '\n',

                              'D', '3', '-', 'f',
                              '\r', '\n',
                              'w', '0', '0', ':', '0', '0', ':', '0', '0',
                              '\r', '\n',
                              'p', '0', '0', ':', '0', '0', ':', '0', '0',
                              '\r', '\n',

                              'D', '4', '-', 'f',
                              '\r', '\n',
                              'w', '0', '0', ':', '0', '0', ':', '0', '0',
                              '\r', '\n',
                              'p', '0', '0', ':', '0', '0', ':', '0', '0',
                              '\r', '\n',

                              'D', '5', '-', 'f',
                              '\r', '\n',
                              'w', '0', '0', ':', '0', '0', ':', '0', '0',
                              '\r', '\n',
                              'p', '0', '0', ':', '0', '0', ':', '0', '0',
                              '\r', '\n',

                              'D', '6', '-', 'f',
                              '\r', '\n',
                              'w', '0', '0', ':', '0', '0', ':', '0', '0',
                              '\r', '\n',
                              'p', '0', '0', ':', '0', '0', ':', '0', '0',
                              '\r', '\n',

                              'D', '7', '-', 'f',
                              '\r', '\n',
                              'w', '0', '0', ':', '0', '0', ':', '0', '0',
                              '\r', '\n',
                              'p', '0', '0', ':', '0', '0', ':', '0', '0',
                              '\r', '\n',
                              '\0'};
    unsigned int i, base_i;
    Time_TimeEdit edit;
    Pwr_Device *device;

    for (i = 0; i < PWR__N_DEVICES; ++i) {
        base_i = i * 28;
        device = Pwr_Devices + i;

        stats[base_i + 3] = (device->state) ? 'f' : 'n';

        edit = Time_timeEdit(device->worktime);

        stats[base_i + 7] = Utility_intToChar(edit.hour / 10);
        stats[base_i + 8] = Utility_intToChar(edit.hour % 10);

        stats[base_i + 10] = Utility_intToChar(edit.minute / 10);
        stats[base_i + 11] = Utility_intToChar(edit.minute % 10);

        stats[base_i + 13] = Utility_intToChar(edit.second / 10);
        stats[base_i + 14] = Utility_intToChar(edit.second % 10);

        edit = Time_timeEdit(device->previous_worktime);

        stats[base_i + 18] = Utility_intToChar(edit.hour / 10);
        stats[base_i + 19] = Utility_intToChar(edit.hour % 10);

        stats[base_i + 21] = Utility_intToChar(edit.minute / 10);
        stats[base_i + 22] = Utility_intToChar(edit.minute % 10);

        stats[base_i + 24] = Utility_intToChar(edit.second / 10);
        stats[base_i + 25] = Utility_intToChar(edit.second % 10);
    }

    return stats;
}

void Pwrsts_checkPoint(uint32_t time)
{
    unsigned int i;
    for (i = 0; i < PWR__N_DEVICES; ++i) {
        Pwr_Device *device = Pwr_Devices + i;
        device->previous_worktime = device->worktime;
        device->worktime = 0;
        device->last_update = time;
    }
}

void Pwrsts_updateStats(uint32_t time)
{
    unsigned int i;
    for (i = 0; i < PWR__N_DEVICES; ++i) {
        Pwr_Device *device = Pwr_Devices + i;
        if (device->state) {
            device->worktime += time - device->last_update;
        }
        device->last_update = time;
    }
}

void Pwrsts_resetStats(uint32_t time)
{
    unsigned int i;
    for (i = 0; i < PWR__N_DEVICES; ++i) {
        Pwr_Device *device = Pwr_Devices + i;
        device->last_update = time;
    }
}

// Pwrsts private functions

void Pwrsts_setDevice0Proc(void)
{
    Pwr_setCurrentDevice(PWR__DEVICE_0);
    Pwrsts_updateGuiStr();
}
void Pwrsts_setDevice1Proc(void)
{
    Pwr_setCurrentDevice(PWR__DEVICE_1);
    Pwrsts_updateGuiStr();
}
void Pwrsts_setDevice2Proc(void)
{
    Pwr_setCurrentDevice(PWR__DEVICE_2);
    Pwrsts_updateGuiStr();
}
void Pwrsts_setDevice3Proc(void)
{
    Pwr_setCurrentDevice(PWR__DEVICE_3);
    Pwrsts_updateGuiStr();
}

void Pwrsts_setDevice4Proc(void)
{
    Pwr_setCurrentDevice(PWR__DEVICE_4);
    Pwrsts_updateGuiStr();
}
void Pwrsts_setDevice5Proc(void)
{
    Pwr_setCurrentDevice(PWR__DEVICE_5);
    Pwrsts_updateGuiStr();
}
void Pwrsts_setDevice6Proc(void)
{
    Pwr_setCurrentDevice(PWR__DEVICE_6);
    Pwrsts_updateGuiStr();
}
void Pwrsts_setDevice7Proc(void)
{
    Pwr_setCurrentDevice(PWR__DEVICE_7);
    Pwrsts_updateGuiStr();
}

void Pwrsts_updateGuiStr(void)
{
    unsigned int worktime = Pwr_Devices[Pwr_DeviceIndex].worktime;
    unsigned int previous_worktime = Pwr_Devices[Pwr_DeviceIndex].previous_worktime;

    Time_TimeEdit edit;

    Pwr_updateGuiStr();

    edit = Time_timeEdit(worktime);

    // hours
    Pwrsts_WorktimeStr[7] = Utility_intToChar(edit.hour / 10);
    Pwrsts_WorktimeStr[8] = Utility_intToChar(edit.hour % 10);

    // minutes
    Pwrsts_WorktimeStr[10] = Utility_intToChar(edit.minute / 10);
    Pwrsts_WorktimeStr[11] = Utility_intToChar(edit.minute % 10);

    // seconds
    Pwrsts_WorktimeStr[13] = Utility_intToChar(edit.second / 10);
    Pwrsts_WorktimeStr[14] = Utility_intToChar(edit.second % 10);

    edit = Time_timeEdit(previous_worktime);

    // hours
    Pwrsts_PreviousWorktimeStr[8] = Utility_intToChar(edit.hour / 10);
    Pwrsts_PreviousWorktimeStr[9] = Utility_intToChar(edit.hour % 10);

    // minutes
    Pwrsts_PreviousWorktimeStr[11] = Utility_intToChar(edit.minute / 10);
    Pwrsts_PreviousWorktimeStr[12] = Utility_intToChar(edit.minute % 10);

    // seconds
    Pwrsts_PreviousWorktimeStr[14] = Utility_intToChar(edit.second / 10);
    Pwrsts_PreviousWorktimeStr[15] = Utility_intToChar(edit.second % 10);
}

void Pwrsts_updateProc(void)
{
    Pwrsts_updateGuiStr();
    Menu_displayMenu(&Pwrsts_MainStatsMenu);
}

// Pwrmng

// Pwrsts private functions prototypes

static void Pwrmng_setDevice0Proc(void);
static void Pwrmng_setDevice1Proc(void);
static void Pwrmng_setDevice2Proc(void);
static void Pwrmng_setDevice3Proc(void);

static void Pwrmng_setDevice4Proc(void);
static void Pwrmng_setDevice5Proc(void);
static void Pwrmng_setDevice6Proc(void);
static void Pwrmng_setDevice7Proc(void);

static void Pwrmng_update(void);

static void Pwrmng_turnDevice(Pwr_DeviceState state);
static void Pwrmng_turnOnProc(void);
static void Pwrmng_turnOffProc(void);

static void Pwrmng_upProc(void);
static void Pwrmng_selectProc(void);
static void Pwrmng_downProc(void);

// Pwrmng private variables

#define PWRMNG__RETURN_ITEM_INDEX 4
static Menu_MenuItem Pwrmng_MainPowerMenuItems[] = {{Pwr_DeviceStr, NULL, NULL},
                                                    {Pwr_StateStr, NULL, NULL},
                                                    {"Turn on", NULL, Pwrmng_turnOnProc},
                                                    {"Turn off", NULL, Pwrmng_turnOffProc},
                                                    {"Return", NULL, NULL}};

static Menu_Menu Pwrmng_MainPowerMenu = {"Power stats",
                                         Pwrmng_MainPowerMenuItems,
                                         UTILITY__COUNT_OF(Pwrmng_MainPowerMenuItems),
                                         0,
                                         Pwrmng_upProc,
                                         Pwrmng_selectProc,
                                         Pwrmng_downProc,
                                         NULL};
static Utility_Procedure *Pwrmng_Update = NULL;

static Menu_MenuItem Pwrmng_Power14MenuItems[] = {{"Device 1", &Pwrmng_MainPowerMenu, Pwrmng_setDevice0Proc},
                                                  {"Device 2", &Pwrmng_MainPowerMenu, Pwrmng_setDevice1Proc},
                                                  {"Device 3", &Pwrmng_MainPowerMenu, Pwrmng_setDevice2Proc},
                                                  {"Device 4", &Pwrmng_MainPowerMenu, Pwrmng_setDevice3Proc},
                                                  {"Return", NULL, NULL}};
static Menu_Menu Pwrmng_Power14Menu = {"Power Menu 1-4",
                                       Pwrmng_Power14MenuItems,
                                       UTILITY__COUNT_OF(Pwrmng_Power14MenuItems),
                                       0,
                                       Pwr_stdUpProc,
                                       Pwr_stdSelectProc,
                                       Pwr_stdDownProc,
                                       NULL};

static Menu_MenuItem Pwrmng_Power58MenuItems[] = {{"Device 5", &Pwrmng_MainPowerMenu, Pwrmng_setDevice4Proc},
                                                  {"Device 6", &Pwrmng_MainPowerMenu, Pwrmng_setDevice5Proc},
                                                  {"Device 7", &Pwrmng_MainPowerMenu, Pwrmng_setDevice6Proc},
                                                  {"Device 8", &Pwrmng_MainPowerMenu, Pwrmng_setDevice7Proc},
                                                  {"Return", NULL, NULL}};
static Menu_Menu Pwrmng_Power58Menu = {"Power Menu 5-8",
                                       Pwrmng_Power58MenuItems,
                                       UTILITY__COUNT_OF(Pwrmng_Power58MenuItems),
                                       0,
                                       Pwr_stdUpProc,
                                       Pwr_stdSelectProc,
                                       Pwr_stdDownProc,
                                       NULL};

static Menu_MenuItem Pwrmng_PowerMenuItems[] = {{"Devices 1-4", &Pwrmng_Power14Menu, NULL},
                                                {"Devices 5-8", &Pwrmng_Power58Menu, NULL},
                                                {"Return", NULL, NULL}};
Menu_Menu Pwrmng_PowerMenu = {"Power Menu",
                              Pwrmng_PowerMenuItems,
                              UTILITY__COUNT_OF(Pwrmng_PowerMenuItems),
                              0,
                              Pwr_stdUpProc,
                              Pwr_stdSelectProc,
                              Pwr_stdDownProc,
                              NULL};

// Pwrmng public functions

void Pwrmng_init(Utility_Procedure *return_proc, Utility_Procedure *update)
{
    Pwrmng_MainPowerMenuItems[PWRMNG__RETURN_ITEM_INDEX].proc = return_proc;
    Pwrmng_Power14MenuItems[PWRMNG__RETURN_ITEM_INDEX].proc = return_proc;
    Pwrmng_Power58MenuItems[PWRMNG__RETURN_ITEM_INDEX].proc = return_proc;
    Pwrmng_PowerMenuItems[UTILITY__COUNT_OF(Pwrmng_PowerMenuItems) - 1].proc = return_proc;
    Pwrmng_Update = update;
}

void Pwrmng_turnDeviceOn(void)
{
    Pwrmng_turnDevice(Pwr_DeviceState_on);
    Pwrmng_update();
}

void Pwrmng_turnDeviceOff(void)
{
    Pwrmng_update();
    Pwrmng_turnDevice(Pwr_DeviceState_off);
}

// Pwrmng private functions

void Pwrmng_setDevice0Proc(void)
{
    Pwr_setCurrentDevice(PWR__DEVICE_0);
    Pwr_updateGuiStr();
}
void Pwrmng_setDevice1Proc(void)
{
    Pwr_setCurrentDevice(PWR__DEVICE_1);
    Pwr_updateGuiStr();
}
void Pwrmng_setDevice2Proc(void)
{
    Pwr_setCurrentDevice(PWR__DEVICE_2);
    Pwr_updateGuiStr();
}
void Pwrmng_setDevice3Proc(void)
{
    Pwr_setCurrentDevice(PWR__DEVICE_3);
    Pwr_updateGuiStr();
}

void Pwrmng_setDevice4Proc(void)
{
    Pwr_setCurrentDevice(PWR__DEVICE_4);
    Pwr_updateGuiStr();
}
void Pwrmng_setDevice5Proc(void)
{
    Pwr_setCurrentDevice(PWR__DEVICE_5);
    Pwr_updateGuiStr();
}
void Pwrmng_setDevice6Proc(void)
{
    Pwr_setCurrentDevice(PWR__DEVICE_6);
    Pwr_updateGuiStr();
}
void Pwrmng_setDevice7Proc(void)
{
    Pwr_setCurrentDevice(PWR__DEVICE_7);
    Pwr_updateGuiStr();
}

static void Pwrmng_update(void)
{
    if (Pwrmng_Update != NULL) {
        Pwrmng_Update();
    }
}

void Pwrmng_turnDevice(Pwr_DeviceState state)
{
    // CMD = 0
    PORT_ResetBits(PWR__CMD_PORT, PWR__CMD_PIN);
    Pwr_waitFor(100);

    // ADDR = current device
    PORT_ResetBits(PWR__ADDR_PORT, PWR__ADDR_PINS);
    PORT_SetBits(PWR__ADDR_PORT, Pwr_Devices[Pwr_DeviceIndex].address);

    if (state) {
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

    Pwr_Devices[Pwr_DeviceIndex].state = state;
}

void Pwrmng_turnOnProc(void)
{
    Pwrmng_turnDeviceOn();

    Pwr_updateGuiStr();

    Lcd_displayString(Lcd_Line_line2, Pwr_StateStr, LCD__STYLE_NO_STYLE);
}

void Pwrmng_turnOffProc(void)
{
    Pwrmng_turnDeviceOff();

    Pwr_updateGuiStr();
    Lcd_displayString(Lcd_Line_line2, Pwr_StateStr, LCD__STYLE_NO_STYLE);
}

void Pwrmng_upProc(void)
{
    Menu_selectUpperItem(&Pwrmng_MainPowerMenu);
}

void Pwrmng_selectProc(void)
{
    size_t index = Pwrmng_MainPowerMenu.item_index;
    Utility_Procedure *proc = Pwrmng_MainPowerMenu.items[index].proc;
    if (proc != NULL) {
        proc();
    }
}

void Pwrmng_downProc(void)
{
    Menu_selectLowerItem(&Pwrmng_MainPowerMenu);
}
