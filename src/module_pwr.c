#include "module_pwr.h"
#include "module_pwrsts.h"
#include "module_pwrmng.h"

#include "module_time.h"
#include "module_utility.h"

// Pwr

// Pwr typedefs

typedef enum {
    Pwr_DeviceState_off = 0,
    Pwr_DeviceState_on  = !Pwr_DeviceState_off
} Pwr_DeviceState;

typedef struct
{
    Pwr_DeviceState state;
    unsigned int last_update;
    unsigned int worktime;
    unsigned int previous_worktime;
    MDR_PORT_TypeDef *port;
    uint32_t pin;
} Pwr_Device;

typedef enum {
    Pwr_WaitForTicks_fast = 8000,
    Pwr_WaitForTicks_test = 800000
} Pwr_WaitForTicks;

// Pwr private function prototypes

static void Pwr_updateGuiStr(void);

static void Pwr_stdUpProc(void);
static void Pwr_stdSelectProc(void);
static void Pwr_stdDownProc(void);

static void Pwr_setDevice1Proc(void);
static void Pwr_setDevice2Proc(void);
static void Pwr_setDevice3Proc(void);

static void Pwr_setDevice4Proc(void);
static void Pwr_setDevice5Proc(void);
static void Pwr_setDevice6Proc(void);

static void Pwr_updateGuiProc(void);

static void Pwr_update(void);

static void Pwr_turnDevice(Pwr_DeviceState state);
static void Pwr_toggleProc(void);

// Variables

static Pwr_Device Pwr_Devices[PWR__N_DEVICES] = {{Pwr_DeviceState_off, 0, 0, 0, PWR__DEVICE_1_PORT, PWR__DEVICE_1_PIN},
                                                 {Pwr_DeviceState_off, 0, 0, 0, PWR__DEVICE_2_PORT, PWR__DEVICE_2_PIN},
                                                 {Pwr_DeviceState_off, 0, 0, 0, PWR__DEVICE_3_PORT, PWR__DEVICE_3_PIN},

                                                 {Pwr_DeviceState_off, 0, 0, 0, PWR__DEVICE_4_PORT, PWR__DEVICE_4_PIN},
                                                 {Pwr_DeviceState_off, 0, 0, 0, PWR__DEVICE_5_PORT, PWR__DEVICE_5_PIN},
                                                 {Pwr_DeviceState_off, 0, 0, 0, PWR__DEVICE_6_PORT, PWR__DEVICE_6_PIN}};
static size_t Pwr_DeviceIndex                 = 0;

static uint8_t Pwr_DeviceStr[]    = {'D', 'e', 'v', 'i', 'c', 'e', ' ', '0', '\0'};
static size_t Pwr_DeviceStrLength = UTILITY__COUNT_OF(Pwr_DeviceStr);

static uint8_t Pwr_StateStr[]    = {'S', 't', 'a', 't', 'e', ' ', 'o', 'f', 'f', '\0'};
static size_t Pwr_StateStrLength = UTILITY__COUNT_OF(Pwr_StateStr);

static uint8_t Pwr_WorktimeStr[] = {'W', 'o', 'r', 'k', 'T', ':', ' ', '.', '.', 'h', '.', '.', 'm', '.', '.', 's', ' ', '\0'};
// h: 7, 8
// m: 10, 11
// s: 13, 14
// static size_t Pwrsts_WorktimeStrLength = UTILITY__COUNT_OF(Pwrsts_WorktimeStr);

static uint8_t Pwr_PreviousWorktimeStr[] = {'P', 'W', 'o', 'r', 'k', 'T', ':', ' ', '.', '.', 'h', '.', '.', 'm', '.', '.', 's', '\0'};
// h: 8, 9
// m: 11, 12
// s: 14, 15
// static size_t Pwrsts_PreviousWorktimeStrLength = UTILITY__COUNT_OF(Pwrsts_PreviousWorktimeStr);

Utility_Procedure *Pwr_StdUpProc     = NULL;
Utility_Procedure *Pwr_StdSelectProc = NULL;
Utility_Procedure *Pwr_StdDownProc   = NULL;

static Menu_MenuItem Pwr_MainDeviceMenuItems[] = {{Pwr_DeviceStr, NULL, NULL},
                                                  {Pwr_StateStr, NULL, Pwr_toggleProc},
                                                  {Pwr_WorktimeStr, NULL, NULL},
                                                  {Pwr_PreviousWorktimeStr, NULL, NULL},
                                                  {"Return", NULL, NULL}};
static Menu_Menu Pwr_MainDeviceMenu            = {"Device info",
                                                  Pwr_MainDeviceMenuItems,
                                                  UTILITY__COUNT_OF(Pwr_MainDeviceMenuItems),
                                                  0,
                                                  Pwr_stdUpProc,
                                                  Pwr_stdSelectProc,
                                                  Pwr_stdDownProc,
                                                  Pwr_updateGuiProc};

static Menu_MenuItem Pwr_Device13MenuItems[] = {{"Device 1", &Pwr_MainDeviceMenu, Pwr_setDevice1Proc},
                                                {"Device 2", &Pwr_MainDeviceMenu, Pwr_setDevice2Proc},
                                                {"Device 3", &Pwr_MainDeviceMenu, Pwr_setDevice3Proc},
                                                {"Return", NULL, NULL}};
static Menu_Menu Pwr_Device13Menu            = {"Devices 1-3",
                                                Pwr_Device13MenuItems,
                                                UTILITY__COUNT_OF(Pwr_Device13MenuItems),
                                                0,
                                                Pwr_stdUpProc,
                                                Pwr_stdSelectProc,
                                                Pwr_stdDownProc,
                                                NULL};

static Menu_MenuItem Pwr_Device46MenuItems[] = {{"Device 4", &Pwr_MainDeviceMenu, Pwr_setDevice4Proc},
                                                {"Device 5", &Pwr_MainDeviceMenu, Pwr_setDevice5Proc},
                                                {"Device 6", &Pwr_MainDeviceMenu, Pwr_setDevice6Proc},
                                                {"Return", NULL, NULL}};
static Menu_Menu Pwr_Device46Menu            = {"Devices 4-6",
                                                Pwr_Device46MenuItems,
                                                UTILITY__COUNT_OF(Pwr_Device46MenuItems),
                                                0,
                                                Pwr_stdUpProc,
                                                Pwr_stdSelectProc,
                                                Pwr_stdDownProc,
                                                NULL};

static Menu_MenuItem Pwr_DeviceMenuItems[] = {{"Devices 1-3", &Pwr_Device13Menu, NULL},
                                              {"Devices 4-6", &Pwr_Device46Menu, NULL},
                                              {"Return", NULL, NULL}};
Menu_Menu Pwr_DeviceMenu                   = {"Devices",
                                              Pwr_DeviceMenuItems,
                                              UTILITY__COUNT_OF(Pwr_DeviceMenuItems),
                                              0,
                                              Pwr_stdUpProc,
                                              Pwr_stdSelectProc,
                                              Pwr_stdDownProc,
                                              NULL};

static Utility_Procedure *Pwr_Update = NULL;

static Pwr_WaitForTicks Pwr_WaitTicks = Pwr_WaitForTicks_fast;

// Pwr public functions

void Pwr_init(Utility_Procedure *return_proc, Utility_Procedure *update)
{
    size_t i;

    for (i = 0; i < PWR__N_DEVICES; ++i) {
        Pwr_Device *device = Pwr_Devices + i;
        PORT_ResetBits(device->port, device->pin);
    }

    Pwr_MainDeviceMenuItems[UTILITY__COUNT_OF(Pwr_MainDeviceMenuItems) - 1].proc = return_proc;
    Pwr_Device13MenuItems[UTILITY__COUNT_OF(Pwr_Device13MenuItems) - 1].proc     = return_proc;
    Pwr_Device46MenuItems[UTILITY__COUNT_OF(Pwr_Device46MenuItems) - 1].proc     = return_proc;
    Pwr_DeviceMenuItems[UTILITY__COUNT_OF(Pwr_DeviceMenuItems) - 1].proc         = return_proc;
    Pwr_Update                                                                   = update;
}

int Pwr_currentDevice(void)
{
    return Pwr_DeviceIndex;
}
Time_TimeEdit Pwr_currentDeviceTimeEdit(void)
{
    return Time_timeEdit((Pwr_Devices + Pwr_DeviceIndex)->worktime);
}
void Pwr_setCurrentDevice(int device)
{
    if (device < 0) {
        Pwr_DeviceIndex = 0;
    } else if (PWR__N_DEVICES <= device) {
        Pwr_DeviceIndex = PWR__N_DEVICES - 1;
    } else {
        Pwr_DeviceIndex = device;
    }
}

void Pwr_turnDeviceOn(void)
{
    Pwr_turnDevice(Pwr_DeviceState_on);
    Pwr_update();
}
void Pwr_turnDeviceOff(void)
{
    Pwr_update();
    Pwr_turnDevice(Pwr_DeviceState_off);
}

uint8_t *Pwr_getStats(void)
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

        stats[base_i + 3] = (device->state) ? 'n' : 'f';

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

void Pwr_checkPoint(uint32_t time)
{
    unsigned int i;
    for (i = 0; i < PWR__N_DEVICES; ++i) {
        Pwr_Device *device        = Pwr_Devices + i;
        device->previous_worktime = device->worktime;
        device->worktime          = 0;
        device->last_update       = time;
    }
}
void Pwr_updateStats(uint32_t time)
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
void Pwr_resetStats(uint32_t time)
{
    unsigned int i;
    for (i = 0; i < PWR__N_DEVICES; ++i) {
        Pwr_Device *device  = Pwr_Devices + i;
        device->last_update = time;
    }
}

void Pwr_toggleTestWaitTicks(void)
{
    if (Pwr_WaitTicks == Pwr_WaitForTicks_fast) {
        Pwr_WaitTicks = Pwr_WaitForTicks_test;
    } else {
        Pwr_WaitTicks = Pwr_WaitForTicks_fast;
    }
}

// Pwr private functions

void Pwr_updateGuiStr(void)
{
    unsigned int worktime          = Pwr_Devices[Pwr_DeviceIndex].worktime;
    unsigned int previous_worktime = Pwr_Devices[Pwr_DeviceIndex].previous_worktime;

    Time_TimeEdit edit;

    Pwr_DeviceStr[Pwr_DeviceStrLength - 2] = Utility_intToChar(Pwr_DeviceIndex + 1);

    Pwr_StateStr[Pwr_StateStrLength - 3] = Pwr_Devices[Pwr_DeviceIndex].state ? 'n' : 'f';
    Pwr_StateStr[Pwr_StateStrLength - 2] = Pwr_Devices[Pwr_DeviceIndex].state ? ' ' : 'f';

    edit = Time_timeEdit(worktime);

    // hours
    Pwr_WorktimeStr[7] = Utility_intToChar(edit.hour / 10);
    Pwr_WorktimeStr[8] = Utility_intToChar(edit.hour % 10);

    // minutes
    Pwr_WorktimeStr[10] = Utility_intToChar(edit.minute / 10);
    Pwr_WorktimeStr[11] = Utility_intToChar(edit.minute % 10);

    // seconds
    Pwr_WorktimeStr[13] = Utility_intToChar(edit.second / 10);
    Pwr_WorktimeStr[14] = Utility_intToChar(edit.second % 10);

    edit = Time_timeEdit(previous_worktime);

    // hours
    Pwr_PreviousWorktimeStr[8] = Utility_intToChar(edit.hour / 10);
    Pwr_PreviousWorktimeStr[9] = Utility_intToChar(edit.hour % 10);

    // minutes
    Pwr_PreviousWorktimeStr[11] = Utility_intToChar(edit.minute / 10);
    Pwr_PreviousWorktimeStr[12] = Utility_intToChar(edit.minute % 10);

    // seconds
    Pwr_PreviousWorktimeStr[14] = Utility_intToChar(edit.second / 10);
    Pwr_PreviousWorktimeStr[15] = Utility_intToChar(edit.second % 10);
}

void Pwr_stdUpProc(void)
{
    if (Pwr_StdUpProc != NULL) {
        Pwr_StdUpProc();
    }
}
void Pwr_stdSelectProc(void)
{
    if (Pwr_StdSelectProc != NULL) {
        Pwr_StdSelectProc();
    }
}
void Pwr_stdDownProc(void)
{
    if (Pwr_StdDownProc != NULL) {
        Pwr_StdDownProc();
    }
}

void Pwr_setDevice1Proc(void)
{
    Pwr_setCurrentDevice(0);
    Pwr_updateGuiStr();
}
void Pwr_setDevice2Proc(void)
{
    Pwr_setCurrentDevice(1);
    Pwr_updateGuiStr();
}
void Pwr_setDevice3Proc(void)
{
    Pwr_setCurrentDevice(2);
    Pwr_updateGuiStr();
}

void Pwr_setDevice4Proc(void)
{
    Pwr_setCurrentDevice(3);
    Pwr_updateGuiStr();
}
void Pwr_setDevice5Proc(void)
{
    Pwr_setCurrentDevice(4);
    Pwr_updateGuiStr();
}
void Pwr_setDevice6Proc(void)
{
    Pwr_setCurrentDevice(5);
    Pwr_updateGuiStr();
}

void Pwr_updateGuiProc(void)
{
    size_t index = Pwr_MainDeviceMenu.item_index;
    Pwr_updateGuiStr();

    Lcd_displayStringSh(6, Lcd_Line_line2, Pwr_StateStr, (index == 1 ? LCD__STYLE_HIGHLIGHT : LCD__STYLE_NO_STYLE));
    Lcd_displayStringSh(7, Lcd_Line_line3, Pwr_WorktimeStr, (index == 2 ? LCD__STYLE_HIGHLIGHT : LCD__STYLE_NO_STYLE));
    Lcd_displayStringSh(8, Lcd_Line_line4, Pwr_PreviousWorktimeStr, (index == 3 ? LCD__STYLE_HIGHLIGHT : LCD__STYLE_NO_STYLE));
}

void Pwr_update(void)
{
    if (Pwr_Update != NULL) {
        Pwr_Update();
    }
}

void Pwr_turnDevice(Pwr_DeviceState state)
{
    Pwr_Device *device = Pwr_Devices + Pwr_DeviceIndex;

    if (state) {
        PORT_SetBits(device->port, device->pin);
    } else {
        PORT_ResetBits(device->port, device->pin);
    }

    device->state = state;
}
void Pwr_toggleProc(void)
{
    if (Pwr_Devices[Pwr_DeviceIndex].state) {
        Pwr_turnDeviceOff();
    } else {
        Pwr_turnDeviceOn();
    }

    Pwr_updateGuiStr();
    Lcd_displayStringSh(6, Lcd_Line_line2, Pwr_StateStr, LCD__STYLE_HIGHLIGHT);
}
