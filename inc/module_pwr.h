#ifndef MODULE_PWR
#define MODULE_PWR

#include "module_menu.h"
#include "module_time.h"

#include <MDR32F9Qx_port.h>

#define PWR__N_DEVICES     6

#define PWR__DEVICE_1_PORT MDR_PORTB
#define PWR__DEVICE_1_PIN  PORT_Pin_0

#define PWR__DEVICE_2_PORT MDR_PORTB
#define PWR__DEVICE_2_PIN  PORT_Pin_1

#define PWR__DEVICE_3_PORT MDR_PORTB
#define PWR__DEVICE_3_PIN  PORT_Pin_2

#define PWR__DEVICE_4_PORT MDR_PORTB
#define PWR__DEVICE_4_PIN  PORT_Pin_3

#define PWR__DEVICE_5_PORT MDR_PORTB
#define PWR__DEVICE_5_PIN  PORT_Pin_4

#define PWR__DEVICE_6_PORT MDR_PORTD
#define PWR__DEVICE_6_PIN  PORT_Pin_5

extern Utility_Procedure *Pwr_StdUpProc;
extern Utility_Procedure *Pwr_StdSelectProc;
extern Utility_Procedure *Pwr_StdDownProc;

extern Menu_Menu Pwr_DeviceMenu;

// Init output register
void Pwr_init(Utility_Procedure *return_proc, Utility_Procedure *update);

int Pwr_currentDevice(void);
Time_TimeEdit Pwr_currentDeviceTimeEdit(void);
void Pwr_setCurrentDevice(int device);

void Pwr_turnDeviceOn(void);
void Pwr_turnDeviceOff(void);

uint8_t *Pwr_getStats(void);

void Pwr_checkPoint(uint32_t time);
void Pwr_updateStats(uint32_t time);
void Pwr_resetStats(uint32_t time);

void Pwr_toggleTestWaitTicks(void);

#endif // MODULE_PWR
