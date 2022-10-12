#ifndef MODULE_PWRMNG
#define MODULE_PWRMNG

#include "module_pwr.h"
#include "module_menu.h"

extern Menu_Menu Pwrmng_PowerMenu;

void Pwrmng_init(Menu_Procedure *return_proc);

void Pwrmng_turnDeviceOn(void);
void Pwrmng_turnDeviceOff(void);

void Pwrmng_setDevice0Proc(void);
void Pwrmng_setDevice1Proc(void);
void Pwrmng_setDevice2Proc(void);
void Pwrmng_setDevice3Proc(void);

void Pwrmng_setDevice4Proc(void);
void Pwrmng_setDevice5Proc(void);
void Pwrmng_setDevice6Proc(void);
void Pwrmng_setDevice7Proc(void);

#endif // MODULE_PWRMNG
