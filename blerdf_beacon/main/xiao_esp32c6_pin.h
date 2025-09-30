#ifndef BleRDF_BEACON_MAIN_XIAO_ESP32C6_PIN_H_
#define BleRDF_BEACON_MAIN_XIAO_ESP32C6_PIN_H_
// ESP32 BleRDF Beacon
// (C)2025 bekki.jp

#include <driver/gpio.h>

namespace blerdf_beacon_system {
namespace xiao_esp32_pin {

static constexpr gpio_num_t kSda = static_cast<gpio_num_t>(22);
static constexpr gpio_num_t kScl = static_cast<gpio_num_t>(23);

static constexpr gpio_num_t kA0 = static_cast<gpio_num_t>(0);
static constexpr gpio_num_t kD9 = static_cast<gpio_num_t>(20);

static constexpr gpio_num_t kWifiEnable =
    static_cast<gpio_num_t>(3);  // RF (Wi-Fi & BLE)
static constexpr gpio_num_t kWifiAntConfig = static_cast<gpio_num_t>(14);

}  // namespace xiao_esp32_pin
}  // namespace blerdf_beacon_system

#endif  // BleRDF_BEACON_MAIN_XIAO_ESP32C6_PIN_H_
