#include "TMC5240.h"
#include <cmath>

#define TMC_WRITE 0x80

// Register addresses (only subset used)
static constexpr uint8_t REG_GCONF      = 0x00;
static constexpr uint8_t REG_IHOLD_IRUN = 0x10;
static constexpr uint8_t REG_TPOWERDOWN = 0x11;
static constexpr uint8_t REG_TCOOLTHRS  = 0x14;
static constexpr uint8_t REG_CHOPCONF   = 0x6C;
static constexpr uint8_t REG_COOLCONF   = 0x6D;
static constexpr uint8_t REG_DCCTRL     = 0x6E;
static constexpr uint8_t REG_DRV_STATUS = 0x6F;
static constexpr uint8_t REG_PWMCONF    = 0x70;
static constexpr uint8_t REG_RAMPMODE   = 0x20;
static constexpr uint8_t REG_XACTUAL    = 0x21;
static constexpr uint8_t REG_VSTART     = 0x23;
static constexpr uint8_t REG_A1         = 0x24;
static constexpr uint8_t REG_V1         = 0x25;
static constexpr uint8_t REG_AMAX       = 0x26;
static constexpr uint8_t REG_VMAX       = 0x27;
static constexpr uint8_t REG_DMAX       = 0x28;
static constexpr uint8_t REG_D1         = 0x2A;
static constexpr uint8_t REG_VSTOP      = 0x2B;
static constexpr uint8_t REG_TZEROWAIT  = 0x2C;
static constexpr uint8_t REG_XTARGET    = 0x2D;
static constexpr uint8_t REG_SW_MODE    = 0x34;
static constexpr uint8_t REG_RAMP_STAT  = 0x35;

TMC5240::TMC5240(uint8_t csPin, float Rref, SPIClass &spi) : _cs(csPin), _Rref(Rref), _spi(&spi) {}

void TMC5240::writeReg(uint8_t addr, uint32_t val) {
    digitalWrite(_cs, LOW);
    _spi->transfer(addr | TMC_WRITE);
    _spi->transfer(val >> 24);
    _spi->transfer(val >> 16);
    _spi->transfer(val >> 8);
    _spi->transfer(val);
    digitalWrite(_cs, HIGH);
}

uint32_t TMC5240::readReg(uint8_t addr) {
    digitalWrite(_cs, LOW);
    _spi->transfer(addr & 0x7F);
    _spi->transfer(0); _spi->transfer(0); _spi->transfer(0); _spi->transfer(0);
    digitalWrite(_cs, HIGH);
    digitalWrite(_cs, LOW);
    _spi->transfer(addr & 0x7F);
    uint32_t val = 0;
    val = _spi->transfer(0); val <<= 8;
    val |= _spi->transfer(0); val <<= 8;
    val |= _spi->transfer(0); val <<= 8;
    val |= _spi->transfer(0);
    digitalWrite(_cs, HIGH);
    return val;
}

void TMC5240::begin() {
    pinMode(_cs, OUTPUT);
    digitalWrite(_cs, HIGH);
    _spi->begin();
    // load defaults to driver
    writeReg(REG_CHOPCONF, CHOPCONF_reg);
    writeReg(REG_PWMCONF,  PWMCONF_reg);
    writeReg(REG_TPOWERDOWN, 10); // default
    writeReg(REG_VSTOP, 1);
    writeReg(REG_RAMPMODE, 0);
}

void TMC5240::filt_isense(uint8_t val) {
    DRV_CONF_reg &= ~(0x3);
    DRV_CONF_reg |= (val & 0x3);
    writeReg(REG_DCCTRL, DRV_CONF_reg); // reuse DCCTRL register slot
}

void TMC5240::toff(uint8_t val) {
    CHOPCONF_reg = (CHOPCONF_reg & ~0xF) | (val & 0xF);
    writeReg(REG_CHOPCONF, CHOPCONF_reg);
}

void TMC5240::blank_time(uint8_t clk) {
    uint8_t tbl = 0;
    switch(clk) {
        case 16: tbl = 0; break;
        case 24: tbl = 1; break;
        case 36: tbl = 2; break;
        case 54: tbl = 3; break;
        default: return;
    }
    CHOPCONF_reg &= ~(0x3 << 15);
    CHOPCONF_reg |= (tbl & 0x3) << 15;
    writeReg(REG_CHOPCONF, CHOPCONF_reg);
}

void TMC5240::microsteps(uint16_t ms) {
    uint8_t mres = 0;
    switch(ms) {
        case 256: mres = 0; break;
        case 128: mres = 1; break;
        case 64:  mres = 2; break;
        case 32:  mres = 3; break;
        case 16:  mres = 4; break;
        case 8:   mres = 5; break;
        case 4:   mres = 6; break;
        case 2:   mres = 7; break;
        default: return;
    }
    CHOPCONF_reg &= ~(0xF << 24);
    CHOPCONF_reg |= (uint32_t)(mres & 0xF) << 24;
    writeReg(REG_CHOPCONF, CHOPCONF_reg);
}

void TMC5240::rms_current(uint16_t mA, float mult) {
    // simplified from library
    float IFS_peak = 36.0f * 1000.0f / 0.075f; // Rs = 0.075 from defaults
    float IFS_rms  = IFS_peak / 1.41421f;
    uint8_t cs = roundf((float)mA / IFS_rms * 31.0f);
    cs = constrain(cs, 1, 31);
    uint32_t ihold_irun = ((uint32_t)(cs * mult) & 0x1F) | ((uint32_t)cs & 0x1F) << 8;
    writeReg(REG_IHOLD_IRUN, ihold_irun);
}

void TMC5240::en_pwm_mode(bool enable) {
    uint32_t gconf = readReg(REG_GCONF);
    if(enable) gconf |= 1 << 2; else gconf &= ~(1 << 2);
    writeReg(REG_GCONF, gconf);
}

void TMC5240::pwm_autoscale(bool enable) {
    if(enable) PWMCONF_reg |= 1 << 18; else PWMCONF_reg &= ~(1 << 18);
    writeReg(REG_PWMCONF, PWMCONF_reg);
}

void TMC5240::TCOOLTHRS(uint32_t val) { writeReg(REG_TCOOLTHRS, val); }
void TMC5240::semin(uint8_t val) { writeReg(REG_COOLCONF, (readReg(REG_COOLCONF) & ~0xF) | (val & 0xF)); }
void TMC5240::semax(uint8_t val) { uint32_t r = readReg(REG_COOLCONF); r &= ~(0xF << 8); r |= (uint32_t)(val & 0xF) << 8; writeReg(REG_COOLCONF, r); }
void TMC5240::sedn(uint8_t val) { uint32_t r = readReg(REG_COOLCONF); r &= ~(0x3 << 13); r |= (uint32_t)(val & 0x3) << 13; writeReg(REG_COOLCONF, r); }
void TMC5240::sgt(int8_t val) { uint32_t r = readReg(REG_COOLCONF); r &= ~(0x7F << 16); r |= (uint32_t)(val & 0x7F) << 16; writeReg(REG_COOLCONF, r); }

void TMC5240::RAMPMODE(uint8_t val) { writeReg(REG_RAMPMODE, val & 0x3); }
void TMC5240::AMAX(uint16_t val) { writeReg(REG_AMAX, val); }
void TMC5240::DMAX(uint16_t val) { writeReg(REG_DMAX, val); }
void TMC5240::VMAX(uint32_t val) { writeReg(REG_VMAX, val); }
void TMC5240::VSTART(uint32_t val) { writeReg(REG_VSTART, val & 0x3FFFF); }
void TMC5240::VSTOP(uint32_t val) { writeReg(REG_VSTOP, val & 0x3FFFF); }

int32_t TMC5240::XACTUAL() { return (int32_t)readReg(REG_XACTUAL); }
void TMC5240::XACTUAL(int32_t val) { writeReg(REG_XACTUAL, val); }
int32_t TMC5240::XTARGET() { return (int32_t)readReg(REG_XTARGET); }
void TMC5240::XTARGET(int32_t val) { writeReg(REG_XTARGET, val); }

bool TMC5240::position_reached() {
    uint32_t r = readReg(REG_RAMP_STAT);
    return r & (1<<9);
}

bool TMC5240::status_sg() {
    uint32_t r = readReg(REG_RAMP_STAT);
    return r & (1<<10);
}

uint32_t TMC5240::DRV_STATUS() { return readReg(REG_DRV_STATUS); }

uint16_t TMC5240::cs2rms(uint8_t CS) {
    uint16_t scaler = 256;
    float KIFS = 36.0f;
    float IFS = KIFS / (_Rref/1000.0f);
    float numerator = scaler * (CS+1) * IFS;
    numerator *= 1000.0f;
    numerator /= 256.0f;
    numerator /= 32.0f;
    numerator /= 1.41421f;
    return (uint16_t)numerator;
}
