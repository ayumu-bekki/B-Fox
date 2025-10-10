#ifndef BleRDF_RECEIVER_MAIN_XIAO_ESP32C6_PIN_H_
#define BleRDF_RECEIVER_MAIN_XIAO_ESP32C6_PIN_H_
// ESP32 BleRDF Receiver
// (C)2025 bekki.jp

#include <driver/gpio.h>

namespace blerdf_receiver_system {
namespace xiao_esp32c6_pin {

constexpr gpio_num_t kLedBuiltin = static_cast<gpio_num_t>(15);

constexpr gpio_num_t kWifiAntConfig = static_cast<gpio_num_t>(14);

constexpr gpio_num_t kTx = static_cast<gpio_num_t>(16); // kD6 Share
constexpr gpio_num_t kRx = static_cast<gpio_num_t>(17); // kD7 Share

constexpr gpio_num_t kMosi = static_cast<gpio_num_t>(18); // kD10 Share
constexpr gpio_num_t kSck = static_cast<gpio_num_t>(19); // kD8 Share
constexpr gpio_num_t kMiso = static_cast<gpio_num_t>(20); // kD9 Share
constexpr gpio_num_t kSS = static_cast<gpio_num_t>(21); // kD3 Share

constexpr gpio_num_t kSda = static_cast<gpio_num_t>(22); // kD4 Share
constexpr gpio_num_t kScl = static_cast<gpio_num_t>(23); // kD5 Share

constexpr gpio_num_t kA0 = static_cast<gpio_num_t>(0);  // kD0 Share
constexpr gpio_num_t kA1 = static_cast<gpio_num_t>(1);  // kD1 Share
constexpr gpio_num_t kA2 = static_cast<gpio_num_t>(2);  // kD2 Share
constexpr gpio_num_t kA4 = static_cast<gpio_num_t>(4);  // kMtms Share
constexpr gpio_num_t kA5 = static_cast<gpio_num_t>(5);  // kMtck Share
constexpr gpio_num_t kA6 = static_cast<gpio_num_t>(6);  // kMtdo Share

constexpr gpio_num_t kWifiEnable = static_cast<gpio_num_t>(3);

constexpr gpio_num_t kMtms = static_cast<gpio_num_t>(4); // kA4 Share
constexpr gpio_num_t kMtdi = static_cast<gpio_num_t>(5); // kA5 Share
constexpr gpio_num_t kMtck = static_cast<gpio_num_t>(6); // kA6 Share
constexpr gpio_num_t kMtdo = static_cast<gpio_num_t>(7);

constexpr gpio_num_t kBoot = static_cast<gpio_num_t>(9);

constexpr gpio_num_t kD0 = static_cast<gpio_num_t>(0);  // kA0 Share
constexpr gpio_num_t kD1 = static_cast<gpio_num_t>(1);  // kA1 Share
constexpr gpio_num_t kD2 = static_cast<gpio_num_t>(2);  // kA2 Share
constexpr gpio_num_t kD3 = static_cast<gpio_num_t>(21); // kSS Share
constexpr gpio_num_t kD4 = static_cast<gpio_num_t>(22); // kSda Share
constexpr gpio_num_t kD5 = static_cast<gpio_num_t>(23); // kScl Share
constexpr gpio_num_t kD6 = static_cast<gpio_num_t>(16); // kTx Share
constexpr gpio_num_t kD7 = static_cast<gpio_num_t>(17); // kRx Share
constexpr gpio_num_t kD8 = static_cast<gpio_num_t>(19); // kSck Share
constexpr gpio_num_t kD9 = static_cast<gpio_num_t>(20); // kMiso Share
constexpr gpio_num_t kD10 = static_cast<gpio_num_t>(18); // kMosi Share

}  // namespace xiao_esp32c6_pin
}  // namespace blerdf_receiver_system

#endif  // BleRDF_RECEIVER_MAIN_XIAO_ESP32C6_PIN_H_
