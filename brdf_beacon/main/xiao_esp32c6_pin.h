#ifndef xiao_esp32c6_pin_H_
#define xiao_esp32c6_pin_H_
// ESP32 BRDF Beacon
// (C)2025 bekki.jp

#include <driver/gpio.h>

namespace BrdfBeaconSystem {
namespace XIAO_ESP32_PIN {

static constexpr gpio_num_t SDA = static_cast<gpio_num_t>(22);
static constexpr gpio_num_t SCL = static_cast<gpio_num_t>(23);

static constexpr gpio_num_t A0 = static_cast<gpio_num_t>(0);
static constexpr gpio_num_t D9 = static_cast<gpio_num_t>(20);

static constexpr gpio_num_t WIFI_ENABLE =
    static_cast<gpio_num_t>(3);  // RF (Wi-Fi & BLE)
static constexpr gpio_num_t WIFI_ANT_CONFIG = static_cast<gpio_num_t>(14);

}  // namespace XIAO_ESP32_PIN
}  // namespace BrdfBeaconSystem

#endif  // xiao_esp32c6_pin_H_
