#ifndef BleRDF_RECEIVER_MAIN_I2C_UTIL_H_
#define BleRDF_RECEIVER_MAIN_I2C_UTIL_H_
// ESP32 BleRDF Receiver
// (C)2025 bekki.jp
// Utilities

// Include ----------------------
#include <driver/gpio.h>
#include <driver/i2c.h>

namespace blerdf_receiver_system {
namespace i2c_util {

void InitializeMaster(const i2c_port_t port, const gpio_num_t sda_pin,
                      const gpio_num_t scl_pin);

}  // namespace i2c_util
}  // namespace blerdf_receiver_system

#endif  // BleRDF_RECEIVER_MAIN_I2C_UTIL_H_

// EOF
