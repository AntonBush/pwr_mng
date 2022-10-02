#ifndef MODULE_PWRMNG
#define MODULE_PWRMNG

#include "module_pwr.h"
#include "module_menu.h"

extern Menu_Menu Pwrmng_PowerMenu;

void Pwrmng_init(Menu_Procedure * return_proc);

void Pwrmng_setDevice0(void);
void Pwrmng_setDevice1(void);
void Pwrmng_setDevice2(void);
void Pwrmng_setDevice3(void);

void Pwrmng_setDevice4(void);
void Pwrmng_setDevice5(void);
void Pwrmng_setDevice6(void);
void Pwrmng_setDevice7(void);

#endif // MODULE_PWRMNG
