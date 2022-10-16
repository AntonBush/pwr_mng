#ifndef MODULE_LCD
#define MODULE_LCD

#include "text.h"

#define LCD__STYLE_NO_STYLE      0
#define LCD__STYLE_HIGHLIGHT     1
#define LCD__STYLE_OVERRIDE_LINE 2
#define LCD__STYLE_NO_OVERRIDE   4

#define LCD__N_LINES             6

typedef enum {
    Lcd_Line_line0 = 0,
    Lcd_Line_line1 = LineMessage1,
    Lcd_Line_line2 = LineMessage2,
    Lcd_Line_line3 = LineMessage3,
    Lcd_Line_line4 = LineMessage4,
    Lcd_Line_line5 = LineMessage5,
} Lcd_Line;

extern const Lcd_Line *Lcd_Lines;

void Lcd_displayStringX(uint32_t x, Lcd_Line line, ucint8_t *str, int style);
void Lcd_displayStringSh(uint32_t shift, Lcd_Line line, ucint8_t *str, int style);
void Lcd_displayString(Lcd_Line line, ucint8_t *str, int style);
void Lcd_displayLine(uint8_t y);
void Lcd_clear(void);

#endif // MODULE_LCD
