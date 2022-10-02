#ifndef MODULE_MENU
#define MODULE_MENU

#include "types.h"

#include <stddef.h>

#define MENU__COUNT_OF(a) (sizeof(a) / sizeof(*(a)))

typedef struct Menu_Menu Menu_Menu;
typedef void (Menu_Procedure)(void);
typedef struct
{
	uint8_t * title;
	struct Menu_Menu * submenu;
	Menu_Procedure * proc;
} Menu_MenuItem;

struct Menu_Menu
{
	uint8_t * title;
	Menu_MenuItem * items;
	size_t n_items;
	size_t item_index;
	Menu_Procedure * up;
	Menu_Procedure * select;
	Menu_Procedure * down;
};

//void Menu_displayMenuItemString(uint32_t y, ucint8_t * str);
//void Menu_displayMenuTitle(ucint8_t * str);
void Menu_displayMenu(Menu_Menu * menu);
void Menu_selectUpperItem(Menu_Menu * menu);
void Menu_selectLowerItem(Menu_Menu * menu);
int Menu_isValidMenu(Menu_Menu * menu);

#endif // MODULE_MENU
