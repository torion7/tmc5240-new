#include "TMCStepper.h"
#include "TMC_MACROS.h"

#define GET_REG00(SETTING) DRVSTATUS(); return READ_RDSEL00_register.SETTING
#define GET_REG01(SETTING) DRVSTATUS(); return READ_RDSEL01_register.SETTING
#define GET_REG10(SETTING) DRVSTATUS(); return READ_RDSEL10_register.SETTING


