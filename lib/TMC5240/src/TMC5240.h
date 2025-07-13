#ifndef TMC5240_H
#define TMC5240_H

#include <Arduino.h>
#include <SPI.h>

struct DRV_STATUS_t {
  uint32_t sr;
  struct {
    uint16_t sg_result : 10;
    uint8_t  :5;
    bool fsactive :1;
    uint8_t cs_actual :5;
    uint8_t :3;
    bool stallGuard:1;
    bool ot:1;
    bool otpw:1;
    bool s2ga:1;
    bool s2gb:1;
    bool ola:1;
    bool olb:1;
    bool stst:1;
  };
};

class TMC5240 {
public:
    static constexpr float default_RREF = 12000.0f; // Ohms
    TMC5240(uint8_t csPin, float Rref = default_RREF, SPIClass &spi = SPI);
    void begin();

    void filt_isense(uint8_t val);
    void toff(uint8_t val);
    void blank_time(uint8_t clk);
    void microsteps(uint16_t ms);
    void rms_current(uint16_t mA, float mult = 0.5f);
    void en_pwm_mode(bool enable);
    void pwm_autoscale(bool enable);

    void TCOOLTHRS(uint32_t val);
    void semin(uint8_t val);
    void semax(uint8_t val);
    void sedn(uint8_t val);
    void sgt(int8_t val);

    void RAMPMODE(uint8_t val);
    void AMAX(uint16_t val);
    void DMAX(uint16_t val);
    void VMAX(uint32_t val);
    void VSTART(uint32_t val);
    void VSTOP(uint32_t val);

    int32_t XACTUAL();
    void XACTUAL(int32_t val);
    int32_t XTARGET();
    void XTARGET(int32_t val);

    bool position_reached();
    bool status_sg();

    uint32_t DRV_STATUS();
    uint16_t cs2rms(uint8_t CS);

private:
    uint8_t _cs;
    float _Rref;
    SPIClass *_spi;

    uint32_t CHOPCONF_reg = 0x10410150UL;
    uint32_t PWMCONF_reg  = 0xC40C001EUL;
    uint32_t DRV_CONF_reg = 0x00001500UL; // initial bits from defaults

    void writeReg(uint8_t address, uint32_t value);
    uint32_t readReg(uint8_t address);
};

#endif
