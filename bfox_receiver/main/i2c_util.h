#ifndef BFOX_RECEIVER_MAIN_I2C_UTIL_H_
#define BFOX_RECEIVER_MAIN_I2C_UTIL_H_
// ESP32 B-Fox Receiver
// (C)2025 bekki.jp
// Utilities

// Include ----------------------
#include <driver/gpio.h>
#include <driver/i2c.h>

namespace bfox_receiver_system {
namespace i2c_util {

void InitializeMaster(const i2c_port_t port, const gpio_num_t sda_pin,
                      const gpio_num_t scl_pin);

}  // namespace i2c_util
}  // namespace bfox_receiver_system

#endif  // BFOX_RECEIVER_MAIN_I2C_UTIL_H_

// EOF
