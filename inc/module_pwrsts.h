#ifndef MODULE_PWRSTS
#define MODULE_PWRSTS

#include "module_pwr.h"
#include "module_menu.h"

extern Menu_Menu Pwrsts_StatsMenu;

void Pwrsts_init(Menu_Procedure * return_proc);

void Pwrsts_setDevice0(void);
void Pwrsts_setDevice1(void);
void Pwrsts_setDevice2(void);
void Pwrsts_setDevice3(void);

void Pwrsts_setDevice4(void);
void Pwrsts_setDevice5(void);
void Pwrsts_setDevice6(void);
void Pwrsts_setDevice7(void);

#endif // MODULE_PWRSTS
