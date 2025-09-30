#ifndef I2C_UTIL_H_
#define I2C_UTIL_H_
// ESP32 BRDF Receiver
// (C)2025 bekki.jp
// Utilities

// Include ----------------------
#include <driver/gpio.h>
#include <driver/i2c.h>

namespace brdf_receiver_system {
namespace i2c_util {

void InitializeMaster(const i2c_port_t port, const gpio_num_t sdaPin,
                      const gpio_num_t sclPin);

}  // namespace i2c_util
}  // namespace brdf_receiver_system

#endif  // I2C_UTIL_H_

// EOF
