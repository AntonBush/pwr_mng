#ifndef MODULE_MENU
#define MODULE_MENU

#include "types.h"
#include "text.h"

#include <stddef.h>

#define MENU__LINE_MESSAGE_1 (LineMessage1)
#define MENU__LINE_MESSAGE_2 (LineMessage2)
#define MENU__LINE_MESSAGE_3 (LineMessage3)
#define MENU__LINE_MESSAGE_4 (LineMessage4
#define MENU__LINE_MESSAGE_5 (LineMessage5)

#define MENU__COUNT_OF(a)    (sizeof(a) / sizeof(*(a)))

typedef struct Menu_Menu Menu_Menu;
typedef void(Menu_Procedure)(void);
typedef struct
{
    uint8_t *title;
    struct Menu_Menu *submenu;
    Menu_Procedure *proc;
} Menu_MenuItem;

struct Menu_Menu {
    uint8_t *title;
    Menu_MenuItem *items;
    size_t n_items;
    size_t item_index;
    Menu_Procedure *up;
    Menu_Procedure *select;
    Menu_Procedure *down;
    Menu_Procedure *update;
};

void Menu_displayMenuTitle(ucint8_t *str);
void Menu_displayString(uint32_t y, ucint8_t *str);
void Menu_displayMenuItemString(uint32_t y, ucint8_t *str);
void Menu_highlightMenuItemString(uint32_t y, ucint8_t *str);

void Menu_displayMenu(Menu_Menu *menu);
void Menu_selectUpperItem(Menu_Menu *menu);
void Menu_selectLowerItem(Menu_Menu *menu);
int Menu_isValidMenu(Menu_Menu *menu);

#endif // MODULE_MENU
