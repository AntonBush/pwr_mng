#ifndef MODULE_PWRMNG
#define MODULE_PWRMNG

#include "module_pwr.h"

typedef void(Pwrmng_Procedure)(void);

extern Menu_Menu Pwrmng_PowerMenu;

void Pwrmng_init(Menu_Procedure *return_proc, Pwrmng_Procedure *update);

void Pwrmng_turnDeviceOn(void);
void Pwrmng_turnDeviceOff(void);

#endif // MODULE_PWRMNG
