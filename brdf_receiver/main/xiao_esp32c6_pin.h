#ifndef xiao_esp32c6_pin_H_
#define xiao_esp32c6_pin_H_
// ESP32 BRDF Receiver
// (C)2025 bekki.jp

#include <driver/gpio.h>

namespace brdf_receiver_system {
namespace xiao_esp32c6_pin {

constexpr gpio_num_t SDA = static_cast<gpio_num_t>(22);
constexpr gpio_num_t SCL = static_cast<gpio_num_t>(23);

constexpr gpio_num_t A0 = static_cast<gpio_num_t>(0);
constexpr gpio_num_t D7 = static_cast<gpio_num_t>(17);
constexpr gpio_num_t D9 = static_cast<gpio_num_t>(20);

// RF (Wi-Fi & BLE)
constexpr gpio_num_t WIFI_ENABLE = static_cast<gpio_num_t>(3);
// External Antenna
constexpr gpio_num_t WIFI_ANT_CONFIG = static_cast<gpio_num_t>(14);

}  // namespace xiao_esp32c6_pin
}  // namespace brdf_receiver_system

#endif  // xiao_esp32c6_pin_H_
