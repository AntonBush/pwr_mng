#ifndef MODULE_MENU
#define MODULE_MENU

#include "module_utility.h"

#include "module_lcd.h"

#include <stddef.h>

typedef struct Menu_Menu Menu_Menu;
typedef struct
{
    uint8_t *title;
    struct Menu_Menu *submenu;
    Utility_Procedure *proc;
} Menu_MenuItem;

struct Menu_Menu {
    uint8_t *title;
    Menu_MenuItem *items;
    size_t n_items;
    size_t item_index;
    Utility_Procedure *up;
    Utility_Procedure *select;
    Utility_Procedure *down;
    Utility_Procedure *update;
};

void Menu_highlightLine(Lcd_Line line);

void Menu_displayMenu(Menu_Menu *menu);
void Menu_selectUpperItem(Menu_Menu *menu);
void Menu_selectLowerItem(Menu_Menu *menu);
int Menu_isValidMenu(Menu_Menu *menu);

#endif // MODULE_MENU
