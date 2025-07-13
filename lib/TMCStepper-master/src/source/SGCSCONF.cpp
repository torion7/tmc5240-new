#include "TMCStepper.h"
#include "TMC_MACROS.h"

#define SET_REG(SETTING) SGCSCONF_register.SETTING = B; write(SGCSCONF_register.address, SGCSCONF_register.sr);
#define GET_REG(SETTING) return SGCSCONF_register.SETTING;

