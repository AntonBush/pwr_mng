#ifndef MODULE_UTILITY
#define MODULE_UTILITY

#include "types.h"

#define UTILITY__COUNT_OF(a) (sizeof(a) / sizeof(*(a)))

typedef void(Utility_Procedure)(void);
typedef void(Utility_ProcedureUint32)(uint32_t);

char Utility_intToChar(int digit);

#endif // MODULE_UTILITY
