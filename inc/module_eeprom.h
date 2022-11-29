#ifndef MODULE_EEPROM
#define MODULE_EEPROM

#include "types.h"
#include "stddef.h"

size_t Eeprom_nBytes(void);
bool Eeprom_writeByte(uint8_t byte, size_t index);
uint8_t Eeprom_readByte(size_t index);

#endif // MODULE_EEPROM
