#include "SPIHelper.h"

SPIHelper::SPIHelper(spi_host_device_t host, int mosi, int miso, int sclk, int cs)
: host(host), pinMOSI(mosi), pinMISO(miso), pinSCLK(sclk), pinCS(cs) {}

void SPIHelper::begin() {
  spi_bus_config_t buscfg = {
    .mosi_io_num = pinMOSI,
    .miso_io_num = pinMISO,
    .sclk_io_num = pinSCLK,
    .quadwp_io_num = -1,
    .quadhd_io_num = -1,
    .max_transfer_sz = 32
  };

  spi_device_interface_config_t devcfg = {
    .command_bits = 0,
    .address_bits = 0,
    .dummy_bits = 0,
    .mode = 0,
    .clock_speed_hz = 10000000,
    .spics_io_num = pinCS,
    .queue_size = 1
  };

  spi_bus_initialize(host, &buscfg, SPI_DMA_DISABLED);
  spi_bus_add_device(host, &devcfg, &device);
}

void SPIHelper::write(uint8_t reg, uint32_t data) {
  uint8_t tx[5] = {
    static_cast<uint8_t>(reg | 0x80),
    (uint8_t)(data >> 24),
    (uint8_t)(data >> 16),
    (uint8_t)(data >> 8),
    (uint8_t)(data)
  };
  spi_transaction_t t = {};
  t.length = 8 * 5;
  t.tx_buffer = tx;
  spi_device_transmit(device, &t);
}

uint32_t SPIHelper::read(uint8_t reg) {
  uint8_t tx[5] = { static_cast<uint8_t>(reg & 0x7F), 0, 0, 0, 0 };
  uint8_t rx[5] = {};
  spi_transaction_t t = {};
  t.length = 8 * 5;
  t.tx_buffer = tx;
  t.rx_buffer = rx;
  spi_device_transmit(device, &t);
  delayMicroseconds(5);
  spi_device_transmit(device, &t);
  return (rx[1] << 24) | (rx[2] << 16) | (rx[3] << 8) | rx[4];
}
