#include "module_eeprom.h"

#include "module_utility.h"
#include "MDR32F9Qx_eeprom.h"

static const uint8_t Eeprom_Storage[64] = {0};

size_t Eeprom_nBytes(void)
{
    (void)Eeprom_Storage;
    return UTILITY__COUNT_OF(Eeprom_Storage);
}

bool Eeprom_writeByte(uint8_t byte, size_t index)
{
    uint32_t old_mask;

    (void)Eeprom_Storage;
    if (Eeprom_nBytes() <= index) {
        return FALSE;
    }

    old_mask = __get_PRIMASK();
    __set_PRIMASK(1);
    EEPROM_ProgramByte((uint32_t)(Eeprom_Storage + index), EEPROM_Main_Bank_Select, byte);
    __set_PRIMASK(old_mask);
    return TRUE;
}

uint8_t Eeprom_readByte(size_t index)
{
    uint32_t old_mask;

    (void)Eeprom_Storage;
    if (Eeprom_nBytes() <= index) {
        return 0;
    }

    old_mask = __get_PRIMASK();
    __set_PRIMASK(1);
    EEPROM_ReadByte((uint32_t)(Eeprom_Storage + index), EEPROM_Main_Bank_Select);
    __set_PRIMASK(old_mask);
    return 0;
}
