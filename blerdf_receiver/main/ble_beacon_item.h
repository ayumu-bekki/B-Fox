#ifndef BleRDF_RECEIVER_MAIN_BLE_BEACON_ITEM_H_
#define BleRDF_RECEIVER_MAIN_BLE_BEACON_ITEM_H_
// ESP32 BleRDF Receiver
// (C)2025 bekki.jp

#include <tuple>

namespace blerdf_receiver_system {

struct BleBeaconItem {
  uint16_t minor;
  int32_t rssi;

  bool operator<(const BleBeaconItem& other) const {
    return std::tie(minor) < std::tie(other.minor);
  }
};

}  // namespace blerdf_receiver_system

#endif  // BleRDF_RECEIVER_MAIN_BLE_BEACON_ITEM_H_
