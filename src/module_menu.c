#include "module_menu.h"

#include "lcd.h"
#include "gl.h"

#include <string.h>

void Menu_displayMenuTitle(ucint8_t *str)
{
    uint32_t x, y;
    sFONT *OldFont = CurrentFont;
    LCD_Method OldMethod = CurrentMethod;

    CurrentFont = &Font_6x8;
    CurrentMethod = MET_AND;

    x = (MAX_X - (CurrentFont->Width * strlen((const char *)str))) / 2;
    LCD_PUTS(x, 0, str);

    y = CurrentFont->Height + 1;
    CurrentMethod = MET_OR;
    LCD_Line(0, y, MAX_X, y);

    CurrentFont = OldFont;
    CurrentMethod = OldMethod;
}

void Menu_displayString(uint32_t y, ucint8_t *str)
{
    uint32_t x;

    CurrentMethod = MET_AND;
    x = (MAX_X - (CurrentFont->Width * strlen((const char *)str))) / 2;
    LCD_PUTS(x, y, str);
}

void Menu_displayMenuItemString(uint32_t y, ucint8_t *str)
{
    Menu_displayString(y, "                      ");
    Menu_displayString(y, str);
}

void Menu_highlightMenuItemString(uint32_t y, ucint8_t *str)
{
    uint32_t x;

    x = (MAX_X - (CurrentFont->Width * strlen((const char *)str))) / 2;

    /* Highlight selected item */
    CurrentMethod = MET_NOT_XOR;
    LCD_PUTS(x, y, str);
}

int Menu_isValidMenu(Menu_Menu *menu)
{
    return !(menu == NULL || menu->items == NULL || menu->n_items <= menu->item_index);
}

void Menu_displayMenu(Menu_Menu *menu)
{
    uint32_t y, index;
    Menu_MenuItem *menu_item;

    if (!Menu_isValidMenu(menu)) return;

    LCD_CLS();
    CurrentMethod = MET_AND;

    /* Display menu header */
    Menu_displayMenuTitle(menu->title);
    /* Display menu items */
    for (index = 0, y = CurrentFont->Height + 4;
         index < menu->n_items;
         index++, y += CurrentFont->Height + 2) {
        menu_item = &(menu->items[index]);
        Menu_displayMenuItemString(y, menu_item->title);
    }

    /* Highlight selected item */
    Menu_highlightMenuItemString((menu->item_index * (CurrentFont->Height + 2) + CurrentFont->Height + 4), "                      ");
}

void Menu_selectUpperItem(Menu_Menu *menu)
{
    Menu_MenuItem *menu_item;

    if (!Menu_isValidMenu(menu)) return;

    /* Display current menu item as non-selected */
    menu_item = &menu->items[menu->item_index];
    CurrentMethod = MET_AND;
    Menu_displayMenuItemString((menu->item_index * (CurrentFont->Height + 2) + CurrentFont->Height + 4), menu_item->title);

    /* Determine new menu item (iteratively) */
    if (menu->item_index > 0) {
        --(menu->item_index);
    } else {
        menu->item_index = menu->n_items - 1;
    }

    /* Display new menu item as selected */
    Menu_highlightMenuItemString((menu->item_index * (CurrentFont->Height + 2) + CurrentFont->Height + 4), "                      ");
}

void Menu_selectLowerItem(Menu_Menu *menu)
{
    Menu_MenuItem *menu_item;

    if (!Menu_isValidMenu(menu)) return;

    /* Display current menu item as non-selected */
    menu_item = &menu->items[menu->item_index];
    CurrentMethod = MET_AND;
    Menu_displayMenuItemString((menu->item_index * (CurrentFont->Height + 2) + CurrentFont->Height + 4), menu_item->title);

    /* Determine new menu item (iteratively) */
    if (menu->item_index >= (menu->n_items - 1)) {
        menu->item_index = 0;
    } else {
        ++(menu->item_index);
    }

    /* Display new menu item as selected */
    Menu_highlightMenuItemString((menu->item_index * (CurrentFont->Height + 2) + CurrentFont->Height + 4), "                      ");
}
