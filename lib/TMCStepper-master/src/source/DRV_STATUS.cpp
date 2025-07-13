#include "TMCStepper.h"
#include "TMC_MACROS.h"

#define GET_REG(NS, SETTING) NS::DRV_STATUS_t r{0}; r.sr = DRV_STATUS(); return r.SETTING

uint32_t TMC2130Stepper::DRV_STATUS() { return read(DRV_STATUS_t::address); }

uint16_t TMC2130Stepper::sg_result(){ GET_REG(TMC2130_n, sg_result); 	}
bool TMC2130Stepper::fsactive()		{ GET_REG(TMC2130_n, fsactive); 	}
uint8_t TMC2130Stepper::cs_actual()	{ GET_REG(TMC2130_n, cs_actual); 	}
bool TMC2130Stepper::stallguard()	{ GET_REG(TMC2130_n, stallGuard); 	}
bool TMC2130Stepper::ot()			{ GET_REG(TMC2130_n, ot); 			}
bool TMC2130Stepper::otpw()			{ GET_REG(TMC2130_n, otpw); 		}
bool TMC2130Stepper::s2ga()			{ GET_REG(TMC2130_n, s2ga); 		}
bool TMC2130Stepper::s2gb()			{ GET_REG(TMC2130_n, s2gb); 		}
bool TMC2130Stepper::ola()			{ GET_REG(TMC2130_n, ola); 			}
bool TMC2130Stepper::olb()			{ GET_REG(TMC2130_n, olb); 			}
bool TMC2130Stepper::stst()			{ GET_REG(TMC2130_n, stst); 		}

