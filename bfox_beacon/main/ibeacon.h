#ifndef BFOX_BEACON_MAIN_IBEACON_H_
#define BFOX_BEACON_MAIN_IBEACON_H_
// ESP32 B-Fox Beacon
// (C)2025 bekki.jp
#include <esp_bt_defs.h>

#include <cstring>

namespace bfox_beacon_system {

// BLE basically uses little-endian, but iBeacon uses big-endian (network byte
// order) in Apple's proprietary format. iBeacon header
struct __attribute__((packed)) BleIBeaconHead {
  uint8_t flags[3];
  uint8_t length;
  uint8_t type;
  uint16_t company_id;   // Apple Company ID: LittleEndian(0x004C)
  uint16_t beacon_type;  // LittleEndian
};

// iBeacon vendor
struct __attribute__((packed)) BleIBeaconVendor {
  uint8_t proximity_uuid[ESP_UUID_LEN_128];
  uint16_t major;  // BigEndian
  uint16_t minor;  // BigEndian
  int8_t measured_power;
};

// iBeacon structure
struct __attribute__((packed)) BleIBeacon {
  BleIBeaconHead ibeacon_head;
  BleIBeaconVendor ibeacon_vendor;
};

uint16_t EndianChangeU16(const uint16_t data);

BleIBeacon CreateIBeaconAttr(const uint8_t* const proximity_uuid,
                             const uint16_t major, const uint16_t minor,
                             const int8_t measured_power);

}  // namespace bfox_beacon_system

#endif  // BFOX_BEACON_MAIN_IBEACON_H_
