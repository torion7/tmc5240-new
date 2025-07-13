#include "TMCStepper.h"
#include "TMC_MACROS.h"

#define SET_REG(SETTING) SMARTEN_register.SETTING = B; write(SMARTEN_register.address, SMARTEN_register.sr)
#define GET_REG(SETTING) return SMARTEN_register.SETTING

