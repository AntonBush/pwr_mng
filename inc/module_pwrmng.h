#ifndef MODULE_PWRMNG
#define MODULE_PWRMNG

#include "module_pwr.h"

extern Menu_Menu Pwrmng_PowerMenu;

void Pwrmng_init(Utility_Procedure *return_proc, Utility_Procedure *update);

void Pwrmng_turnDeviceOn(void);
void Pwrmng_turnDeviceOff(void);

#endif // MODULE_PWRMNG
