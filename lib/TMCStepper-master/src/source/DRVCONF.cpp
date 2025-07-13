#include "TMCStepper.h"
#include "TMC_MACROS.h"

#define SET_REG(SETTING) DRVCONF_register.SETTING = B; write(DRVCONF_register.address, DRVCONF_register.sr)
#define GET_REG(SETTING) return DRVCONF_register.SETTING;

