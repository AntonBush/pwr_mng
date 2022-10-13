#include "module_menu.h"

#include "module_lcd.h"

void Menu_highlightLine(Lcd_Line line)
{
    Lcd_displayString(line,
                      "                      ",
                      LCD__STYLE_NO_OVERRIDE | LCD__STYLE_HIGHLIGHT);
}

int Menu_isValidMenu(Menu_Menu *menu)
{
    return !(menu == NULL || menu->items == NULL || menu->n_items <= menu->item_index);
}

void Menu_displayMenu(Menu_Menu *menu)
{
    size_t i;
    Menu_MenuItem *menu_item;

    if (!Menu_isValidMenu(menu)) return;

    Lcd_clear();

    /* Display menu header */
    Lcd_displayString(Lcd_Line_line0, menu->title, LCD__STYLE_NO_OVERRIDE);
    Lcd_displayLine(Lcd_Line_line1 - 3);
    /* Display menu items */
    for (i = 0; i < menu->n_items; ++i) {
        menu_item = menu->items + i;
        Lcd_displayString(Lcd_Lines[i + 1],
                          menu_item->title,
                          LCD__STYLE_NO_OVERRIDE);
        if (i == menu->item_index) {
            Menu_highlightLine(Lcd_Lines[i + 1]);
        }
    }
}

void Menu_selectUpperItem(Menu_Menu *menu)
{
    if (!Menu_isValidMenu(menu)) return;

    /* Display current menu item as non-selected */
    Menu_highlightLine(Lcd_Lines[menu->item_index + 1]);

    /* Determine new menu item (iteratively) */
    if (menu->item_index > 0) {
        --(menu->item_index);
    } else {
        menu->item_index = menu->n_items - 1;
    }

    /* Display new menu item as selected */
    Menu_highlightLine(Lcd_Lines[menu->item_index + 1]);
}

void Menu_selectLowerItem(Menu_Menu *menu)
{
    if (!Menu_isValidMenu(menu)) return;

    /* Display current menu item as non-selected */
    Menu_highlightLine(Lcd_Lines[menu->item_index + 1]);

    /* Determine new menu item (iteratively) */
    if (menu->item_index >= (menu->n_items - 1)) {
        menu->item_index = 0;
    } else {
        ++(menu->item_index);
    }

    /* Display new menu item as selected */
    Menu_highlightLine(Lcd_Lines[menu->item_index + 1]);
}
