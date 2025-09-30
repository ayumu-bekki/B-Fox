#ifndef BLE_BEACON_ITEM_H_
#define BLE_BEACON_ITEM_H_
// ESP32 BRDF Receiver
// (C)2025 bekki.jp

#include <tuple>

namespace brdf_receiver_system {

struct BleBeaconItem {
  uint16_t minor;
  int32_t rssi;

  bool operator<(const BleBeaconItem& other) const {
    return std::tie(minor) < std::tie(other.minor);
  }
};

}  // namespace brdf_receiver_system

#endif  // BLE_BEACON_ITEM_H_
