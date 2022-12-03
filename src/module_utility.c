#include "module_utility.h"

char Utility_intToChar(int digit)
{
    if (digit < 0 || 9 < digit) {
        return '?';
    }

    return '0' + digit;
}

char Utility_uintToChar(unsigned int digit)
{
    if (9 < digit) {
        return '?';
    }

    return '0' + digit;
}
