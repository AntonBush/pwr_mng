#ifndef MODULE_EEPROM
#define MODULE_EEPROM

#include "types.h"
#include "stddef.h"

void Eeprom_init(void);

size_t Eeprom_nBytes(void);
uint8_t Eeprom_readByte(size_t index);
bool Eeprom_writeByte(uint8_t byte, size_t index);

#endif // MODULE_EEPROM
