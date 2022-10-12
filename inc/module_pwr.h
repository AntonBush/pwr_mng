#ifndef MODULE_PWR
#define MODULE_PWR

#include "module_menu.h"

#include <MDR32F9Qx_port.h>

#define PWR__ADDR_PORT  MDR_PORTB
#define PWR__ADDR_PIN_0 PORT_Pin_0
#define PWR__ADDR_PIN_1 PORT_Pin_1
#define PWR__ADDR_PIN_2 PORT_Pin_2
#define PWR__ADDR_PINS  (PWR__ADDR_PIN_0 | PWR__ADDR_PIN_1 | PWR__ADDR_PIN_2)

#define PWR__CMD_PORT   MDR_PORTB
#define PWR__CMD_PIN    PORT_Pin_3

#define PWR__DATA_PORT  MDR_PORTB
#define PWR__DATA_PIN   PORT_Pin_4

#define PWR__N_DEVICES  8

#define PWR__DEVICE_0   0
#define PWR__DEVICE_1   PWR__ADDR_PIN_0
#define PWR__DEVICE_2   PWR__ADDR_PIN_1
#define PWR__DEVICE_3   (PWR__ADDR_PIN_0 | PWR__ADDR_PIN_1)

#define PWR__DEVICE_4   PWR__ADDR_PIN_2
#define PWR__DEVICE_5   (PWR__ADDR_PIN_0 | PWR__ADDR_PIN_2)
#define PWR__DEVICE_6   (PWR__ADDR_PIN_1 | PWR__ADDR_PIN_2)
#define PWR__DEVICE_7   (PWR__ADDR_PIN_0 | PWR__ADDR_PIN_1 | PWR__ADDR_PIN_2)

extern Menu_Procedure *Pwr_StdUpProc;
extern Menu_Procedure *Pwr_StdSelectProc;
extern Menu_Procedure *Pwr_StdDownProc;

// Init output register
void Pwr_init(void);

int Pwr_currentDevice(void);
void Pwr_setCurrentDevice(int device);

#endif // MODULE_PWR
