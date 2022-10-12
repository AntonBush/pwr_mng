#ifndef MODULE_TIME
#define MODULE_TIME

#include "module_menu.h"

typedef struct
{
	unsigned int hour;
	unsigned int minute;
	unsigned int second;
} Time_TimeEdit;

extern Menu_Menu Time_SetTimeMenu;

void Time_init(Menu_Procedure * return_proc);

uint32_t Time_getRawTime(void);
Time_TimeEdit Time_getTime(void);

void Time_setRawTime(uint32_t time);
void Time_setTime(Time_TimeEdit time);

uint32_t Time_getNPassedDays(void);
void Time_addPassedDay(void);

//int getTimeSeconds()

#endif // MODULE_TIME
