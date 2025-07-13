#pragma once

//#define TMCDEBUG

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wunused-variable"

#if defined(ARDUINO) && ARDUINO >= 100
	#include <Arduino.h>
	#include <SPI.h>
	#include <Stream.h>
#elif defined(bcm2835)
	#include <bcm2835.h>
	#include "source/bcm2835_spi.h"
	#include "source/bcm2835_stream.h"
#elif __cplusplus >= 201703L
	#if __has_include(<Arduino.h>)
		#include <Arduino.h>
	#endif
	#if __has_include(<SPI.h>)
		#include <SPI.h>
	#endif
	#if __has_include(<Stream.h>)
		#include <Stream.h>
	#endif
#endif

#if (__cplusplus == 201703L) && defined(__has_include)
	#define SW_CAPABLE_PLATFORM __has_include(<SoftwareSerial.h>)
#elif defined(__AVR__) || defined(TARGET_LPC1768) || defined(ARDUINO_ARCH_STM32)
	#define SW_CAPABLE_PLATFORM true
#else
	#define SW_CAPABLE_PLATFORM false
#endif

#if SW_CAPABLE_PLATFORM
	#include <SoftwareSerial.h>
#endif

#include "source/SERIAL_SWITCH.h"
#include "source/SW_SPI.h"

#pragma GCC diagnostic pop

#include "source/TMC2130_bitfields.h"
#include "source/TMC2160_bitfields.h"
#include "source/TMC5130_bitfields.h"
#include "source/TMC5160_bitfields.h"
#include "source/TMC5240_bitfields.h"

#define INIT_REGISTER(REG) REG##_t REG##_register = REG##_t
#define INIT2130_REGISTER(REG) TMC2130_n::REG##_t REG##_register = TMC2130_n::REG##_t
#define INIT2160_REGISTER(REG) TMC2160_n::REG##_t REG##_register = TMC2160_n::REG##_t
#define INIT5130_REGISTER(REG) TMC5130_n::REG##_t REG##_register = TMC5130_n::REG##_t
#define INIT5160_REGISTER(REG) TMC5160_n::REG##_t REG##_register = TMC5160_n::REG##_t
#define INIT5240_REGISTER(REG) TMC5240_n::REG##_t REG##_register = TMC5240_n::REG##_t
#define SET_ALIAS(TYPE, DRIVER, NEW, ARG, OLD) TYPE (DRIVER::*NEW)(ARG) = &DRIVER::OLD

#define TMCSTEPPER_VERSION 0x000703 // v0.7.3

class TMCStepper {
	public:
		uint16_t cs2rms(uint8_t CS);
		void rms_current(uint16_t mA);
		void rms_current(uint16_t mA, float mult);
		uint16_t rms_current();
		void hold_multiplier(float val) { holdMultiplier = val; }
		float hold_multiplier() { return holdMultiplier; }
		uint8_t test_connection();

		// Helper functions
		void microsteps(uint16_t ms);
		uint16_t microsteps();
		void blank_time(uint8_t value);
		uint8_t blank_time();
		void hysteresis_end(int8_t value);
		int8_t hysteresis_end();
		void hysteresis_start(uint8_t value);
		uint8_t hysteresis_start();

		// R+WC: GSTAT
		void 	GSTAT(							uint8_t input);
		uint8_t GSTAT();
		bool 	reset();
		bool 	drv_err();
		bool 	uv_cp();

		// W: IHOLD_IRUN
		void IHOLD_IRUN(					uint32_t input);
		uint32_t IHOLD_IRUN();
		void 	ihold(							uint8_t B);
		void 	irun(								uint8_t B);
		void 	iholddelay(					uint8_t B);
		uint8_t ihold();
		uint8_t irun();
		uint8_t iholddelay();

		// W: TPOWERDOWN
		uint8_t TPOWERDOWN();
		void TPOWERDOWN(					uint8_t input);

		// R: TSTEP
		uint32_t TSTEP();

		// W: TPWMTHRS
		uint32_t TPWMTHRS();
		void TPWMTHRS(						uint32_t input);

		// R: MSCNT
		uint16_t MSCNT();

		// R: MSCURACT
		uint32_t MSCURACT();
		int16_t cur_a();
		int16_t cur_b();

	protected:
		TMCStepper(float RS) : Rsense(RS) {};
		INIT_REGISTER(IHOLD_IRUN){{.sr=0}};	// 32b
		INIT_REGISTER(TPOWERDOWN){.sr=0};		// 8b
		INIT_REGISTER(TPWMTHRS){.sr=0};			// 32b

		static constexpr uint8_t TMC_READ = 0x00,
														TMC_WRITE = 0x80;

		struct TSTEP_t { constexpr static uint8_t address = 0x12; };
                struct MSCNT_t { constexpr static uint8_t address = 0x6A; };
                struct MSCURACT_t {
                        union {
                                uint32_t sr;
                                struct {
                                        uint16_t cur_a:9;
                                        uint16_t cur_b:9;
                                        uint16_t reserved:14;
                                };
                        };
                        constexpr static uint8_t address = 0x6B;
                };

		virtual void write(uint8_t, uint32_t) = 0;
		virtual uint32_t read(uint8_t) = 0;
		virtual void vsense(bool) = 0;
		virtual bool vsense(void) = 0;
		virtual uint32_t DRV_STATUS() = 0;
		virtual void hend(uint8_t) = 0;
		virtual uint8_t hend() = 0;
		virtual void hstrt(uint8_t) = 0;
		virtual uint8_t hstrt() = 0;
		virtual void mres(uint8_t) = 0;
		virtual uint8_t mres() = 0;
		virtual void tbl(uint8_t) = 0;
		virtual uint8_t tbl() = 0;

		const float Rsense;
		float holdMultiplier = 0.5;
};

class TMC2130Stepper : public TMCStepper {
	public:
		TMC2130Stepper(uint16_t pinCS, float RS = default_RS, int8_t link_index = -1);
		TMC2130Stepper(uint16_t pinCS, uint16_t pinMOSI, uint16_t pinMISO, uint16_t pinSCK, int8_t link_index = -1);
		TMC2130Stepper(uint16_t pinCS, float RS, uint16_t pinMOSI, uint16_t pinMISO, uint16_t pinSCK, int8_t link_index = -1);
		void begin();
		void defaults();
		void setSPISpeed(uint32_t speed);
		void switchCSpin(bool state);
		bool isEnabled();
		void push();

		// Helper functions
		void sg_current_decrease(uint8_t value);
		uint8_t sg_current_decrease();

		// RW: GCONF
		uint32_t GCONF();
		void GCONF(								uint32_t value);
		void I_scale_analog(			bool B);
		void internal_Rsense(			bool B);
		void en_pwm_mode(					bool B);
		void enc_commutation(			bool B);
		void shaft(								bool B);
		void diag0_error(					bool B);
		void diag0_otpw(					bool B);
		void diag0_stall(					bool B);
		void diag1_stall(					bool B);
		void diag1_index(					bool B);
		void diag1_onstate(				bool B);
		void diag1_steps_skipped(	bool B);
		void diag0_int_pushpull(	bool B);
		void diag1_pushpull(			bool B);
		void small_hysteresis(		bool B);
		void stop_enable(					bool B);
		void direct_mode(					bool B);
		bool I_scale_analog();
		bool internal_Rsense();
		bool en_pwm_mode();
		bool enc_commutation();
		bool shaft();
		bool diag0_error();
		bool diag0_otpw();
		bool diag0_stall();
		bool diag1_stall();
		bool diag1_index();
		bool diag1_onstate();
		bool diag1_steps_skipped();
		bool diag0_int_pushpull();
		bool diag1_pushpull();
		bool small_hysteresis();
		bool stop_enable();
		bool direct_mode();

		// R: IOIN
		uint32_t IOIN();
		bool step();
		bool dir();
		bool dcen_cfg4();
		bool dcin_cfg5();
		bool drv_enn_cfg6();
		bool dco();
		uint8_t version();

		// W: TCOOLTHRS
		uint32_t TCOOLTHRS();
		void TCOOLTHRS(						uint32_t input);

		// W: THIGH
		uint32_t THIGH();
		void THIGH(								uint32_t input);

		// RW: XDRIRECT
		uint32_t XDIRECT();
		void XDIRECT(							uint32_t input);
		void coil_A(							int16_t 	B);
		void coil_B(							int16_t 	B);
		int16_t coil_A();
		int16_t coil_B();

		// W: VDCMIN
		uint32_t VDCMIN();
		void VDCMIN(							uint32_t input);

		// RW: CHOPCONF
		uint32_t CHOPCONF();
		void CHOPCONF(						uint32_t value);
		void toff(								uint8_t B);
		void hstrt(								uint8_t B);
		void hend(								uint8_t B);
		//void fd(									uint8_t B);
		void disfdcc(							bool 		B);
		void rndtf(								bool 		B);
		void chm(									bool 		B);
		void tbl(									uint8_t B);
		void vsense(							bool 		B);
		void vhighfs(							bool 		B);
		void vhighchm(						bool 		B);
		void sync(								uint8_t B);
		void mres(								uint8_t B);
		void intpol(							bool 		B);
		void dedge(								bool 		B);
		void diss2g(							bool 		B);
		uint8_t toff();
		uint8_t hstrt();
		uint8_t hend();
		//uint8_t fd();
		bool 	disfdcc();
		bool 	rndtf();
		bool 	chm();
		uint8_t tbl();
		bool 	vsense();
		bool 	vhighfs();
		bool 	vhighchm();
		uint8_t sync();
		uint8_t mres();
		bool 	intpol();
		bool 	dedge();
		bool 	diss2g();

		// W: COOLCONF
		void COOLCONF(uint32_t value);
		uint32_t COOLCONF();
		void semin(								uint8_t B);
		void seup(								uint8_t B);
		void semax(								uint8_t B);
		void sedn(								uint8_t B);
		void seimin(							bool 		B);
		void sgt(									int8_t  B);
		void sfilt(								bool 		B);
		uint8_t semin();
		uint8_t seup();
		uint8_t semax();
		uint8_t sedn();
		bool seimin();
		int8_t sgt();
		bool sfilt();

		// W: DCCTRL
		void DCCTRL(uint32_t input);
		void dc_time(uint16_t input);
		void dc_sg(uint8_t input);
		uint32_t DCCTRL();
		uint16_t dc_time();
		uint8_t dc_sg();

		// R: DRV_STATUS
		uint32_t DRV_STATUS();
		uint16_t sg_result();
		bool fsactive();
		uint8_t cs_actual();
		bool stallguard();
		bool ot();
		bool otpw();
		bool s2ga();
		bool s2gb();
		bool ola();
		bool olb();
		bool stst();

		// W: PWMCONF
		void PWMCONF(							uint32_t value);
		uint32_t PWMCONF();
		void pwm_ampl(						uint8_t B);
		void pwm_grad(						uint8_t B);
		void pwm_freq(						uint8_t B);
		void pwm_autoscale(				bool		B);
		void pwm_symmetric(				bool		B);
		void freewheel(						uint8_t B);
		uint8_t pwm_ampl();
		uint8_t pwm_grad();
		uint8_t pwm_freq();
		bool 	pwm_autoscale();
		bool 	pwm_symmetric();
		uint8_t freewheel();

		// R: PWM_SCALE
		uint8_t PWM_SCALE();

		// W: ENCM_CTRL
		uint8_t ENCM_CTRL();
		void ENCM_CTRL(						uint8_t input);
		void inv(									bool B);
		void maxspeed(						bool B);
		bool inv();
		bool maxspeed();

		// R: LOST_STEPS
		uint32_t LOST_STEPS();

		// Function aliases

		uint8_t status_response;

	protected:
		void beginTransaction();
		void endTransaction();
		uint8_t transfer(const uint8_t data);
		void transferEmptyBytes(const uint8_t n);
		void write(uint8_t addressByte, uint32_t config);
		uint32_t read(uint8_t addressByte);

		INIT_REGISTER(GCONF){{.sr=0}};		// 32b
		INIT_REGISTER(TCOOLTHRS){.sr=0};	// 32b
		INIT_REGISTER(THIGH){.sr=0};			// 32b
		INIT_REGISTER(XDIRECT){{.sr=0}};	// 32b
		INIT_REGISTER(VDCMIN){.sr=0};			// 32b
		INIT_REGISTER(CHOPCONF){{.sr=0}};	// 32b
		INIT_REGISTER(COOLCONF){{.sr=0}};	// 32b
		INIT_REGISTER(DCCTRL){{.sr = 0}};	// 32b
		INIT_REGISTER(PWMCONF){{.sr=0}};	// 32b
		INIT_REGISTER(ENCM_CTRL){{.sr=0}};//  8b

		struct IOINT_t 		{ constexpr static uint8_t address = 0x04; };
		struct PWM_SCALE_t 	{ constexpr static uint8_t address = 0x71; };
		struct LOST_STEPS_t { constexpr static uint8_t address = 0x73; };
		struct DRV_STATUS_t { constexpr static uint8_t address = 0X6F; };

		static uint32_t spi_speed; // Default 2MHz
		const uint16_t _pinCS;
		SW_SPIClass * TMC_SW_SPI = nullptr;
		static constexpr float default_RS = 0.11;

		int8_t link_index;
		static int8_t chain_length;
};

class TMC2160Stepper : public TMC2130Stepper {
	public:
		TMC2160Stepper(uint16_t pinCS, float RS = default_RS, int8_t link_index = -1);
		TMC2160Stepper(uint16_t pinCS, uint16_t pinMOSI, uint16_t pinMISO, uint16_t pinSCK, int8_t link_index = -1);
		TMC2160Stepper(uint16_t pinCS, float RS, uint16_t pinMOSI, uint16_t pinMISO, uint16_t pinSCK, int8_t link_index = -1);
		void begin();
		void defaults();
		void push();

		uint16_t cs2rms(uint8_t CS);
		void rms_current(uint16_t mA);
		void rms_current(uint16_t mA, float mult);
		uint16_t rms_current();

		// IOIN
		uint32_t 	IOIN();
		bool 			refl_step();
		bool 			refr_dir();
		bool 			encb_dcen_cfg4();
		bool 			enca_dcin_cfg5();
		bool 			drv_enn();
		bool 			dco_cfg6();
		uint8_t 	version();

		// W: OTP_PROG
		// R: OTP_READ
		// RW: FACTORY_CONF

		// W: SHORT_CONF
		void SHORT_CONF(uint32_t);
		void s2vs_level(uint8_t);
		void s2g_level(uint8_t);
		void shortfilter(uint8_t);
		void shortdelay(bool);
		uint32_t SHORT_CONF();
		uint8_t s2vs_level();
		uint8_t s2g_level();
		uint8_t shortfilter();
		bool shortdelay();

		// W: DRV_CONF
		void DRV_CONF(uint32_t);
		void bbmtime(uint8_t);
		void bbmclks(uint8_t);
		void otselect(uint8_t);
		void drvstrength(uint8_t);
		void filt_isense(uint8_t);
		uint32_t DRV_CONF();
		uint8_t bbmtime();
		uint8_t bbmclks();
		uint8_t otselect();
		uint8_t drvstrength();
		uint8_t filt_isense();

		// W: GLOBAL_SCALER
		void GLOBAL_SCALER(uint8_t);
		uint8_t GLOBAL_SCALER();

		// R: OFFSET_READ
		uint16_t OFFSET_READ();

		// W: PWMCONF
		void PWMCONF(uint32_t input);
		void pwm_ofs(uint8_t B);
		void pwm_grad(uint8_t B);
		void pwm_freq(uint8_t B);
		void pwm_autoscale(bool B);
		void pwm_autograd(bool B);
		void freewheel(uint8_t B);
		void pwm_reg(uint8_t B);
		void pwm_lim(uint8_t B);
		uint32_t PWMCONF();
		uint8_t pwm_ofs();
		uint8_t pwm_grad();
		uint8_t pwm_freq();
		bool pwm_autoscale();
		bool pwm_autograd();
		uint8_t freewheel();
		uint8_t pwm_reg();
		uint8_t pwm_lim();

		// R: PWM_SCALE
		uint32_t PWM_SCALE();
		uint8_t pwm_scale_sum();
		uint16_t pwm_scale_auto();

	protected:
		using TMC2130Stepper::ENCM_CTRL;
		using TMC2130Stepper::pwm_ampl;
		using TMC2130Stepper::pwm_symmetric;

		INIT_REGISTER(SHORT_CONF){{.sr=0}};
		INIT_REGISTER(DRV_CONF){{.sr=0}};
		INIT_REGISTER(GLOBAL_SCALER){.sr=0};
		INIT2160_REGISTER(PWMCONF){{.sr=0}};

		static constexpr float default_RS = 0.075;
};

class TMC5130Stepper : public TMC2160Stepper {
	public:
		TMC5130Stepper(uint16_t pinCS, float RS = default_RS, int8_t link_index = -1);
		TMC5130Stepper(uint16_t pinCS, uint16_t pinMOSI, uint16_t pinMISO, uint16_t pinSCK, int8_t link_index = -1);
		TMC5130Stepper(uint16_t pinCS, float RS, uint16_t pinMOSI, uint16_t pinMISO, uint16_t pinSCK, int8_t link_index = -1);

		void begin();
		void defaults();
		void push();

		void rms_current(uint16_t mA) { TMC2130Stepper::rms_current(mA); }
		void rms_current(uint16_t mA, float mult) { TMC2130Stepper::rms_current(mA, mult); }
		uint16_t rms_current() { return TMC2130Stepper::rms_current(); }

		// R: IFCNT
		uint8_t IFCNT();
		// W: SLAVECONF
		uint16_t SLAVECONF();
		void SLAVECONF(uint16_t input);
		// R: IOIN
		uint32_t 	IOIN();
		bool 			refl_step();
		bool 			refr_dir();
		bool 			encb_dcen_cfg4();
		bool 			enca_dcin_cfg5();
		bool 			drv_enn_cfg6();
		bool 			enc_n_dco();
		bool 			sd_mode();
		bool 			swcomp_in();
		uint8_t 	version();

		// RW: GCONF
		void diag1_poscomp_pushpull(bool B) { diag1_pushpull(B); }
		bool diag1_poscomp_pushpull() { return diag1_pushpull(); }

		// RW: SW_MODE
		uint32_t SW_MODE();
		void SW_MODE(uint32_t input);

		void stop_l_enable(bool B);
		void stop_r_enable(bool B);
		void pol_stop_l(bool B);
		void pol_stop_r(bool B);
		void swap_lr(bool B);
		void latch_l_active(bool B);
		void latch_l_inactive(bool B);
		void latch_r_active(bool B);
		void latch_r_inactive(bool B);
		void en_latch_encoder(bool B);
		void sg_stop(bool B);
		void en_softstop(bool B);

		bool stop_r_enable();
		bool pol_stop_l();
		bool pol_stop_r();
		bool swap_lr();
		bool latch_l_active();
		bool latch_l_inactive();
		bool latch_r_active();
		bool latch_r_inactive();
		bool en_latch_encoder();
		bool sg_stop();
		bool en_softstop();

		// R+C: RAMP_STAT
		uint32_t RAMP_STAT();
		bool status_stop_l();
		bool status_stop_r();
		bool status_latch_l();
		bool status_latch_r();
		bool event_stop_l();
		bool event_stop_r();
		bool event_stop_sg();
		bool event_pos_reached();
		bool velocity_reached();
		bool position_reached();
		bool vzero();
		bool t_zerowait_active();
		bool second_move();
		bool status_sg();

		// RW: ENCMODE
		uint32_t ENCMODE();
		void ENCMODE(uint32_t input);
		void pol_a(bool B);
		void pol_b(bool B);
		void pol_n(bool B);
		void ignore_ab(bool B);
		void clr_cont(bool B);
		void clr_once(bool B);
		void pos_edge(bool B);
		void neg_edge(bool B);
		void clr_enc_x(bool B);
		void latch_x_act(bool B);
		void enc_sel_decimal(bool B);
		bool pol_a();
		bool pol_b();
		bool pol_n();
		bool ignore_ab();
		bool clr_cont();
		bool clr_once();
		bool pos_edge();
		bool neg_edge();
		bool clr_enc_x();
		bool latch_x_act();
		bool enc_sel_decimal();

		// W: OUTPUT
		bool TMC_OUTPUT();
		void TMC_OUTPUT(bool input);
		// W: X_COMPARE
		uint32_t X_COMPARE();
		void X_COMPARE(uint32_t input);
		// RW: RAMPMODE
		uint8_t RAMPMODE();
		void RAMPMODE(uint8_t input);
		// RW: XACTUAL
		int32_t XACTUAL();
		void XACTUAL(int32_t input);
		// R: VACTUAL
		int32_t VACTUAL();
		// W: VSTART
		uint32_t VSTART();
		void VSTART(uint32_t input);
		// W: A1
		uint16_t a1();
		void a1(uint16_t input);
		// W: V1
		uint32_t v1();
		void v1(uint32_t input);
		// W: AMAX
		uint16_t AMAX();
		void AMAX(uint16_t input);
		// W: VMAX
		uint32_t VMAX();
		void VMAX(uint32_t input);
		// W: DMAX
		uint16_t DMAX();
		void DMAX(uint16_t input);
		// W: D1
		uint16_t d1();
		void d1(uint16_t input);
		// W: VSTOP
		uint32_t VSTOP();
		void VSTOP(uint32_t input);
		// W: TZEROWAIT
		uint16_t TZEROWAIT();
		void TZEROWAIT(uint16_t input);
		// RW: XTARGET
		int32_t XTARGET();
		void XTARGET(int32_t input);
		// R: XLATCH
		uint32_t XLATCH();
		// RW: X_ENC
		int32_t X_ENC();
		void X_ENC(int32_t input);
		// W: ENC_CONST
		uint32_t ENC_CONST();
		void ENC_CONST(uint32_t input);
		// R: ENC_STATUS
		bool ENC_STATUS();
		// R: ENC_LATCH
		uint32_t ENC_LATCH();

		using TMC2130Stepper::ENCM_CTRL;

		using TMC2130Stepper::PWMCONF;
		using TMC2130Stepper::pwm_ampl;
		using TMC2130Stepper::pwm_grad;
		using TMC2130Stepper::pwm_freq;
		using TMC2130Stepper::pwm_autoscale;
		using TMC2130Stepper::pwm_symmetric;
		using TMC2130Stepper::freewheel;

		using TMC2130Stepper::PWM_SCALE;

	protected:
		INIT_REGISTER(SLAVECONF){{.sr=0}};
		INIT_REGISTER(OUTPUT){.sr=0};
		INIT_REGISTER(X_COMPARE){.sr=0};
		INIT_REGISTER(RAMPMODE){.sr=0};
		INIT_REGISTER(XACTUAL){.sr=0};
		INIT_REGISTER(VSTART){.sr=0};
		INIT_REGISTER(A1){.sr=0};
		INIT_REGISTER(V1){.sr=0};
		INIT_REGISTER(AMAX){.sr=0};
		INIT_REGISTER(VMAX){.sr=0};
		INIT_REGISTER(DMAX){.sr=0};
		INIT_REGISTER(D1){.sr=0};
		INIT_REGISTER(VSTOP){.sr=0};
		INIT_REGISTER(TZEROWAIT){.sr=0};
		INIT_REGISTER(SW_MODE){{.sr=0}};
		INIT_REGISTER(ENCMODE){{.sr=0}};
		INIT_REGISTER(ENC_CONST){.sr=0};

		struct IFCNT_t 		{ constexpr static uint8_t address = 0x02; }; // R
		struct VACTUAL_t 	{ constexpr static uint8_t address = 0x22; }; // R
		struct XTARGET_t 	{ constexpr static uint8_t address = 0x2D; }; // RW
		struct XLATCH_t 	{ constexpr static uint8_t address = 0x36; }; // R
		struct X_ENC_t 		{ constexpr static uint8_t address = 0x39; }; // RW
		struct ENC_STATUS_t { constexpr static uint8_t address = 0x3B; }; // R+C
		struct ENC_LATCH_t 	{ constexpr static uint8_t address = 0x3C; }; // R

		/*
		INIT_REGISTER(MSLUT0){0};
		INIT_REGISTER(MSLUT1){0};
		INIT_REGISTER(MSLUT2){0};
		INIT_REGISTER(MSLUT3){0};
		INIT_REGISTER(MSLUT4){0};
		INIT_REGISTER(MSLUT5){0};
		INIT_REGISTER(MSLUT6){0};
		INIT_REGISTER(MSLUT7){0};
		INIT_REGISTER(MSLUTSEL){0};
		INIT_REGISTER(MSLUTSTART){0};
		INIT_REGISTER(MSCNT){0};
		INIT_REGISTER(MSCURACT){0};
		*/

		static constexpr float default_RS = 0.15;

	protected:
		using TMC2160Stepper::SHORT_CONF;
		using TMC2160Stepper::s2vs_level;
		using TMC2160Stepper::s2g_level;
		using TMC2160Stepper::shortfilter;
		using TMC2160Stepper::shortdelay;
		using TMC2160Stepper::DRV_CONF;
		using TMC2160Stepper::bbmtime;
		using TMC2160Stepper::bbmclks;
		using TMC2160Stepper::otselect;
		using TMC2160Stepper::drvstrength;
		using TMC2160Stepper::filt_isense;
		using TMC2160Stepper::GLOBAL_SCALER;
		using TMC2160Stepper::OFFSET_READ;

		using TMC2160Stepper::pwm_ofs;
		using TMC2160Stepper::pwm_autograd;
		using TMC2160Stepper::pwm_reg;
		using TMC2160Stepper::pwm_lim;

		using TMC2160Stepper::pwm_scale_sum;
		using TMC2160Stepper::pwm_scale_auto;
};

class TMC5160Stepper : public TMC5130Stepper {
	public:
		TMC5160Stepper(uint16_t pinCS, float RS = default_RS, int8_t link_index = -1);
		TMC5160Stepper(uint16_t pinCS, uint16_t pinMOSI, uint16_t pinMISO, uint16_t pinSCK, int8_t link_index = -1);
		TMC5160Stepper(uint16_t pinCS, float RS, uint16_t pinMOSI, uint16_t pinMISO, uint16_t pinSCK, int8_t link_index = -1);

		void rms_current(uint16_t mA) { TMC2160Stepper::rms_current(mA); }
		void rms_current(uint16_t mA, float mult) { TMC2160Stepper::rms_current(mA, mult); }
		uint16_t rms_current() { return TMC2160Stepper::rms_current(); }
		void defaults();
		void push();

		// RW: GCONF
		void recalibrate(bool);
		void faststandstill(bool);
		void multistep_filt(bool);
		bool recalibrate();
		bool faststandstill();
		bool multistep_filt();

		// R: IOIN
		bool drv_enn() { return drv_enn_cfg6(); }
		bool enc_n_dco_cfg6() { return enc_n_dco(); }

		// W: SHORT_CONF
		using TMC2160Stepper::SHORT_CONF;
		using TMC2160Stepper::s2vs_level;
		using TMC2160Stepper::s2g_level;
		using TMC2160Stepper::shortfilter;
		using TMC2160Stepper::shortdelay;

		// W: DRV_CONF
		using TMC2160Stepper::DRV_CONF;
		using TMC2160Stepper::bbmtime;
		using TMC2160Stepper::bbmclks;
		using TMC2160Stepper::otselect;
		using TMC2160Stepper::drvstrength;
		using TMC2160Stepper::filt_isense;

		// W: GLOBAL_SCALER
		using TMC2160Stepper::GLOBAL_SCALER;

		// R: OFFSET_READ
		using TMC2160Stepper::OFFSET_READ;

		// R+WC: ENC_STATUS
		void ENC_STATUS(uint8_t);
		uint8_t ENC_STATUS();

		// W: ENC_DEVIATION
		void ENC_DEVIATION(uint32_t);
		uint32_t ENC_DEVIATION();

		// R: PWM_AUTO
		uint32_t PWM_AUTO();
		uint8_t pwm_ofs_auto();
		uint8_t pwm_grad_auto();

		// RW: CHOPCONF
		void diss2vs(bool);
		void tpfd(uint8_t);
		bool diss2vs();
		uint8_t tpfd();

		// W: PWM_CONF
		using TMC2160Stepper::PWMCONF;
		using TMC2160Stepper::pwm_ofs;
		using TMC2160Stepper::pwm_grad;
		using TMC2160Stepper::pwm_freq;
		using TMC2160Stepper::pwm_autoscale;
		using TMC2160Stepper::pwm_autograd;
		using TMC2160Stepper::freewheel;
		using TMC2160Stepper::pwm_reg;
		using TMC2160Stepper::pwm_lim;

		using TMC2160Stepper::PWM_SCALE;
		using TMC2160Stepper::pwm_scale_sum;
		using TMC2160Stepper::pwm_scale_auto;

	protected:
		using TMC5130Stepper::I_scale_analog;
		using TMC5130Stepper::internal_Rsense;
		using TMC5130Stepper::enc_commutation;
		using TMC5130Stepper::drv_enn_cfg6;
		using TMC5130Stepper::enc_n_dco;
		using TMC5130Stepper::ENCM_CTRL;
		using TMC5130Stepper::vsense;
		using TMC5130Stepper::rndtf;

		INIT_REGISTER(ENC_DEVIATION){.sr=0};

		static constexpr float default_RS = 0.075;
};

class TMC5161Stepper : public TMC5160Stepper {
        public:
    TMC5161Stepper(uint16_t pinCS, float RS = default_RS, int8_t link_index = -1) : TMC5160Stepper(pinCS, RS, link_index) {}
                TMC5161Stepper(uint16_t pinCS, uint16_t pinMOSI, uint16_t pinMISO, uint16_t pinSCK, int8_t link_index = -1) :
                        TMC5160Stepper(pinCS, pinMOSI, pinMISO, pinSCK, link_index) {}
                TMC5161Stepper(uint16_t pinCS, float RS, uint16_t pinMOSI, uint16_t pinMISO, uint16_t pinSCK, int8_t link_index = -1) :
                        TMC5160Stepper(pinCS, RS, pinMOSI, pinMISO, pinSCK, link_index) {}
};

class TMC5240Stepper : public TMC5160Stepper {
       public:
               static constexpr float default_RREF = 12000.0; // Ohms
               TMC5240Stepper(uint16_t pinCS, float Rref = default_RREF, int8_t link_index = -1);
               TMC5240Stepper(uint16_t pinCS, uint16_t pinMOSI, uint16_t pinMISO, uint16_t pinSCK, int8_t link_index = -1);
               TMC5240Stepper(uint16_t pinCS, float Rref, uint16_t pinMOSI, uint16_t pinMISO, uint16_t pinSCK, int8_t link_index = -1);

               uint16_t cs2rms(uint8_t CS);
               void rms_current(uint16_t mA);
               void rms_current(uint16_t mA, float mult);
               uint16_t rms_current();

               void defaults();
               void push();

               // R+WC: ENC_STATUS
               void ENC_STATUS(uint8_t);
               uint8_t ENC_STATUS();

               // W: ENC_DEVIATION
               void ENC_DEVIATION(uint32_t);
               uint32_t ENC_DEVIATION();

               // R: PWM_AUTO
               uint32_t PWM_AUTO();
               uint8_t pwm_ofs_auto();
               uint8_t pwm_grad_auto();
			   
       private:
               const float Rref;
};
