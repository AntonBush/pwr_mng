#ifndef MODULE_PWRSTS
#define MODULE_PWRSTS

#include "module_pwr.h"

extern Menu_Menu Pwrsts_StatsMenu;

void Pwrsts_init(Utility_Procedure *return_proc);

uint8_t *Pwrsts_getStats(void);

void Pwrsts_checkPoint(uint32_t time);
void Pwrsts_updateStats(uint32_t time);
void Pwrsts_resetStats(uint32_t time);

#endif // MODULE_PWRSTS
