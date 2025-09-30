// ESP32 BleRDF Receiver
// (C)2025 bekki.jp
// Utilities

// Include ----------------------
#include "i2c_util.h"

namespace blerdf_receiver_system {
namespace i2c_util {

void InitializeMaster(const i2c_port_t port, const gpio_num_t sda_pin,
                      const gpio_num_t scl_pin) {
  constexpr i2c_mode_t mode = I2C_MODE_MASTER;
  constexpr uint32_t i2c_master_freq_hz = 100000;

  i2c_config_t config = {};
  config.mode = mode;
  config.sda_io_num = sda_pin;
  config.scl_io_num = scl_pin;
  config.scl_pullup_en = true;
  config.sda_pullup_en = true;
  config.master.clk_speed = i2c_master_freq_hz;

  i2c_param_config(port, &config);
  i2c_driver_install(port, mode, 0, 0, 0);
}

}  // namespace i2c_util
}  // namespace blerdf_receiver_system

// EOF
