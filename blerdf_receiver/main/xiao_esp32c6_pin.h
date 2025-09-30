#ifndef BleRDF_RECEIVER_MAIN_XIAO_ESP32C6_PIN_H_
#define BleRDF_RECEIVER_MAIN_XIAO_ESP32C6_PIN_H_
// ESP32 BleRDF Receiver
// (C)2025 bekki.jp

#include <driver/gpio.h>

namespace blerdf_receiver_system {
namespace xiao_esp32c6_pin {

constexpr gpio_num_t kSda = static_cast<gpio_num_t>(22);
constexpr gpio_num_t kScl = static_cast<gpio_num_t>(23);

constexpr gpio_num_t kA0 = static_cast<gpio_num_t>(0);
constexpr gpio_num_t kD7 = static_cast<gpio_num_t>(17);
constexpr gpio_num_t kD9 = static_cast<gpio_num_t>(20);

// RF (Wi-Fi & BLE)
constexpr gpio_num_t kWifiEnable = static_cast<gpio_num_t>(3);
// External Antenna
constexpr gpio_num_t kWifiAntConfig = static_cast<gpio_num_t>(14);

}  // namespace xiao_esp32c6_pin
}  // namespace blerdf_receiver_system

#endif  // BleRDF_RECEIVER_MAIN_XIAO_ESP32C6_PIN_H_
