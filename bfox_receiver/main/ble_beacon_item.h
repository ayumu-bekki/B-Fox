#ifndef BFOX_RECEIVER_MAIN_BLE_BEACON_ITEM_H_
#define BFOX_RECEIVER_MAIN_BLE_BEACON_ITEM_H_
// ESP32 B-Fox Receiver
// (C)2025 bekki.jp

#include <cstdint>
#include <tuple>

namespace bfox_receiver_system {

struct BleBeaconItem {
  uint16_t minor;
  int32_t rssi;
  int64_t last_seen_ms;  // timestamp in ms (esp_timer_get_time() / 1000)

  bool operator<(const BleBeaconItem& other) const {
    return std::tie(minor) < std::tie(other.minor);
  }
};

}  // namespace bfox_receiver_system

#endif  // BFOX_RECEIVER_MAIN_BLE_BEACON_ITEM_H_
