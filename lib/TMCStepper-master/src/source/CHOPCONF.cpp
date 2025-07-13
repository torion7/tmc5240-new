#include "TMCStepper.h"
#include "TMC_MACROS.h"

#define SET_REG(SETTING) CHOPCONF_register.SETTING = B; write(CHOPCONF_register.address, CHOPCONF_register.sr)

// CHOPCONF
uint32_t TMC2130Stepper::CHOPCONF() {
	return read(CHOPCONF_register.address);
}
void TMC2130Stepper::CHOPCONF(uint32_t input) {
	CHOPCONF_register.sr = input;
	write(CHOPCONF_register.address, CHOPCONF_register.sr);
}

void TMC2130Stepper::toff(		uint8_t B )	{ SET_REG(toff);	}
void TMC2130Stepper::hstrt(		uint8_t B )	{ SET_REG(hstrt);	}
void TMC2130Stepper::hend(		uint8_t B )	{ SET_REG(hend);	}
//void TMC2130Stepper::fd(		uint8_t B )	{ SET_REG(fd);		}
void TMC2130Stepper::disfdcc(	bool 	B )	{ SET_REG(disfdcc);	}
void TMC2130Stepper::rndtf(		bool 	B )	{ SET_REG(rndtf);	}
void TMC2130Stepper::chm(		bool 	B )	{ SET_REG(chm);		}
void TMC2130Stepper::tbl(		uint8_t B )	{ SET_REG(tbl);		}
void TMC2130Stepper::vsense(	bool 	B )	{ SET_REG(vsense);	}
void TMC2130Stepper::vhighfs(	bool 	B )	{ SET_REG(vhighfs);	}
void TMC2130Stepper::vhighchm(	bool 	B )	{ SET_REG(vhighchm);}
void TMC2130Stepper::sync(		uint8_t B )	{ SET_REG(sync);	}
void TMC2130Stepper::mres(		uint8_t B )	{ SET_REG(mres);	}
void TMC2130Stepper::intpol(	bool 	B )	{ SET_REG(intpol);	}
void TMC2130Stepper::dedge(		bool 	B )	{ SET_REG(dedge);	}
void TMC2130Stepper::diss2g(	bool 	B )	{ SET_REG(diss2g);	}

uint8_t TMC2130Stepper::toff()		{ CHOPCONF_t r{0}; r.sr = CHOPCONF(); return r.toff;	}
uint8_t TMC2130Stepper::hstrt()		{ CHOPCONF_t r{0}; r.sr = CHOPCONF(); return r.hstrt;	}
uint8_t TMC2130Stepper::hend()		{ CHOPCONF_t r{0}; r.sr = CHOPCONF(); return r.hend;	}
//uint8_t TMC2130Stepper::fd()		{ CHOPCONF_t r{0}; r.sr = CHOPCONF(); return r.fd;		}
bool 	TMC2130Stepper::disfdcc()	{ CHOPCONF_t r{0}; r.sr = CHOPCONF(); return r.disfdcc;	}
bool 	TMC2130Stepper::rndtf()		{ CHOPCONF_t r{0}; r.sr = CHOPCONF(); return r.rndtf;	}
bool 	TMC2130Stepper::chm()		{ CHOPCONF_t r{0}; r.sr = CHOPCONF(); return r.chm;		}
uint8_t TMC2130Stepper::tbl()		{ CHOPCONF_t r{0}; r.sr = CHOPCONF(); return r.tbl;		}
bool 	TMC2130Stepper::vsense()	{ CHOPCONF_t r{0}; r.sr = CHOPCONF(); return r.vsense;	}
bool 	TMC2130Stepper::vhighfs()	{ CHOPCONF_t r{0}; r.sr = CHOPCONF(); return r.vhighfs;	}
bool 	TMC2130Stepper::vhighchm()	{ CHOPCONF_t r{0}; r.sr = CHOPCONF(); return r.vhighchm;}
uint8_t TMC2130Stepper::sync()		{ CHOPCONF_t r{0}; r.sr = CHOPCONF(); return r.sync;	}
uint8_t TMC2130Stepper::mres()		{ CHOPCONF_t r{0}; r.sr = CHOPCONF(); return r.mres;	}
bool 	TMC2130Stepper::intpol()	{ CHOPCONF_t r{0}; r.sr = CHOPCONF(); return r.intpol;	}
bool 	TMC2130Stepper::dedge()		{ CHOPCONF_t r{0}; r.sr = CHOPCONF(); return r.dedge;	}
bool 	TMC2130Stepper::diss2g()	{ CHOPCONF_t r{0}; r.sr = CHOPCONF(); return r.diss2g;	}

void TMC5160Stepper::diss2vs(bool B){ SET_REG(diss2vs); }
void TMC5160Stepper::tpfd(uint8_t B){ SET_REG(tpfd);	}
bool TMC5160Stepper::diss2vs()		{ CHOPCONF_t r{0}; r.sr = CHOPCONF(); return r.diss2vs; }
uint8_t TMC5160Stepper::tpfd()		{ CHOPCONF_t r{0}; r.sr = CHOPCONF(); return r.tpfd;	}


#define GET_REG_2660(SETTING) return CHOPCONF_register.SETTING;

