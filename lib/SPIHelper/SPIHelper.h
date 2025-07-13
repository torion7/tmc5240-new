#pragma once
#include <Arduino.h>
#include "driver/spi_master.h"

class SPIHelper {
public:
  SPIHelper(spi_host_device_t host, int mosi, int miso, int sclk, int cs);
  void begin();
  void write(uint8_t reg, uint32_t data);
  uint32_t read(uint8_t reg);

private:
  spi_device_handle_t device;
  spi_host_device_t host;
  int pinMOSI, pinMISO, pinSCLK, pinCS;
};
