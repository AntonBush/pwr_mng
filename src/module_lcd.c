#include "module_lcd.h"

#include "lcd.h"
#include "gl.h"

#include <string.h>

static const Lcd_Line Lcd_LineArray[] = {Lcd_Line_line0,
                                         Lcd_Line_line1,
                                         Lcd_Line_line2,
                                         Lcd_Line_line3,
                                         Lcd_Line_line4,
                                         Lcd_Line_line5};

const Lcd_Line *Lcd_Lines = Lcd_LineArray;

void Lcd_displayString(Lcd_Line line, ucint8_t *str, int style)
{
    uint32_t x, i, tmp;
    bool highlight = FALSE;
    bool override_line = FALSE;
    bool no_override = FALSE;

    x = (MAX_X - (CurrentFont->Width * strlen((const char *)str))) / 2;

    if (style & LCD__STYLE_HIGHLIGHT) {
        highlight = TRUE;
    }

    if (style & LCD__STYLE_OVERRIDE_LINE) {
        override_line = TRUE;
    } else if (style & LCD__STYLE_NO_OVERRIDE) {
        no_override = TRUE;
    }

    if (!no_override) {
        CurrentMethod = MET_AND;
        if (override_line) {
            LCD_PUTS(0, (int8_t)line, "                      ");
        } else {
            for (tmp = x, i = 0; str[i] != '\0'; tmp += CurrentFont->Width, ++i) {
                LCD_PUTC(tmp, (uint8_t)line, ' ');
            }
        }
    }

    CurrentMethod = highlight ? MET_NOT_XOR : MET_OR;
    LCD_PUTS(x, (uint8_t)line, str);
}

void Lcd_displayLine(uint8_t y)
{
    CurrentMethod = MET_OR;
    LCD_Line(0, y, MAX_X, y);
}

void Lcd_clear(void)
{
    LCD_CLS();
}
