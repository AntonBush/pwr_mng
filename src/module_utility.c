#include "module_utility.h"

#include <MDR32Fx.h>

char Utility_intToChar(int digit)
{
    if (digit < 0 || 9 < digit) {
        return '?';
    }

    return '0' + digit;
}

void Utility_waitFor(unsigned int ticks)
{
    unsigned int i;
    for (i = 0; i < ticks; ++i) {
        __NOP();
    }
}
