#ifndef MODULE_PWRSTS
#define MODULE_PWRSTS

#include "module_pwr.h"
#include "module_menu.h"

extern Menu_Menu Pwrsts_StatsMenu;

void Pwrsts_init(Menu_Procedure *return_proc);

uint8_t *Pwrsts_getStats(void);

void Pwrsts_checkPoint(uint32_t time);
void Pwrsts_updateStats(uint32_t time);

void Pwrsts_setDevice0Proc(void);
void Pwrsts_setDevice1Proc(void);
void Pwrsts_setDevice2Proc(void);
void Pwrsts_setDevice3Proc(void);

void Pwrsts_setDevice4Proc(void);
void Pwrsts_setDevice5Proc(void);
void Pwrsts_setDevice6Proc(void);
void Pwrsts_setDevice7Proc(void);

#endif // MODULE_PWRSTS
