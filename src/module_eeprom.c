#include "module_eeprom.h"

#include "module_utility.h"
#include "MDR32FxQI_eeprom.h"           // Milandr::Drivers:EEPROM
#include "MDR32FxQI_rst_clk.h"          // Milandr::Drivers:RST_CLK

static uint32_t Eeprom_Storage[64] __attribute__((section("READONLY_MEMORY_SECTION"))) = {
'0', '1', '2', '3', '4', '5', '6', '7',
'0', '1', '2', '3', '4', '5', '6', '7',
'0', '1', '2', '3', '4', '5', '6', '7',
'0', '1', '2', '3', '4', '5', '6', '7',

'0', '1', '2', '3', '4', '5', '6', '7',
'0', '1', '2', '3', '4', '5', '6', '7',
'0', '1', '2', '3', '4', '5', '6', '7',
'0', '1', '2', '3', '4', '5', '6', '7',
};

void Eeprom_init(void)
{
	EEPROM_SetLatency(EEPROM_Latency_1);
}

size_t Eeprom_nBytes(void)
{
    return UTILITY__COUNT_OF(Eeprom_Storage);
}

uint8_t Eeprom_readByte(size_t index)
{
    uint32_t old_mask;
    uint8_t byte;

    if (Eeprom_nBytes() <= index) {
        return 0;
    }

    old_mask = __get_PRIMASK();
    __set_PRIMASK(1);
    byte = EEPROM_ReadWord((uint32_t)(Eeprom_Storage + index), EEPROM_Main_Bank_Select);
    __set_PRIMASK(old_mask);

    return byte;
}

bool Eeprom_writeByte(uint8_t byte, size_t index)
{
    uint32_t old_mask;

    if (Eeprom_nBytes() <= index) {
        return FALSE;
    }

    old_mask = __get_PRIMASK();
    __set_PRIMASK(1);
    EEPROM_ProgramWord((uint32_t)(Eeprom_Storage + index), EEPROM_Main_Bank_Select, (uint32_t)byte);
    __set_PRIMASK(old_mask);
    return TRUE;
}
