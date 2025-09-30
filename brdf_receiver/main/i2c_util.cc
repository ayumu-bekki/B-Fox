// ESP32 BRDF Receiver
// (C)2025 bekki.jp
// Utilities

// Include ----------------------
#include "i2c_util.h"

namespace brdf_receiver_system {
namespace i2c_util {

void InitializeMaster(const i2c_port_t port, const gpio_num_t sdaPin,
                      const gpio_num_t sclPin) {
  constexpr i2c_mode_t mode = I2C_MODE_MASTER;
  constexpr uint32_t i2c_master_feq_hz = 100000;

  i2c_config_t config = {};
  config.mode = mode;
  config.sda_io_num = sdaPin;
  config.scl_io_num = sclPin;
  config.scl_pullup_en = true;
  config.sda_pullup_en = true;
  config.master.clk_speed = i2c_master_feq_hz;

  i2c_param_config(port, &config);
  i2c_driver_install(port, mode, 0, 0, 0);
}

}  // namespace i2c_util
}  // namespace brdf_receiver_system

// EOF
