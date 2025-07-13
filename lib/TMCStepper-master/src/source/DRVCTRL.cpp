#include "TMCStepper.h"
#include "TMC_MACROS.h"

#define SET_REG0(SETTING) DRVCTRL_0_register.SETTING = B; DRVCTRL(DRVCTRL_0_register.sr)
#define SET_REG1(SETTING) DRVCTRL_1_register.SETTING = B; DRVCTRL(DRVCTRL_1_register.sr)
#define GET_REG0(SETTING) return DRVCTRL_0_register.SETTING
#define GET_REG1(SETTING) return DRVCTRL_1_register.SETTING

