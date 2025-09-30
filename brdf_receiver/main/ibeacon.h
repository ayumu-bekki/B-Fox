#ifndef ibeacon_H_
#define ibeacon_H_
// ESP32 BRDF Receiver
// (C)2025 bekki.jp

#include <cstring>

namespace brdf_receiver_system {

// BLE basically uses little-endian, but iBeacon uses big-endian (network byte
// order) in Apple's proprietary format.

// iBeacon header
struct __attribute__((packed)) BleIBeaconHead {
  uint8_t flags[3];
  uint8_t length;
  uint8_t type;
  uint16_t company_id;   // LittleEndian
  uint16_t beacon_type;  // LittleEndian
};
// company_id Apple Company ID: Little Endian (0x004C)

// iBeacon vendor
struct __attribute__((packed)) BleIBeaconVendor {
  uint8_t proximity_uuid[16];
  uint16_t major;  // BigEndian
  uint16_t minor;  // BigEndian
  int8_t measured_power;
};

// iBeacon structure
struct __attribute__((packed)) BleIBeacon {
  BleIBeaconHead ibeacon_head;
  BleIBeaconVendor ibeacon_vendor;
};

// iBeacon header
static const BleIBeaconHead IBEACON_HEADER = {.flags = {0x02, 0x01, 0x06},
                                              .length = 0x1A,
                                              .type = 0xFF,
                                              .company_id = 0x004C,
                                              .beacon_type = 0x1502};

uint16_t endian_change_u16(const uint16_t data) {
  return ((data & 0xff00) >> 8) + ((data & 0xff) << 8);
}

BleIBeacon CreateiBeaconAttr(const uint8_t *const proximity_uuid,
                             const uint16_t major, const uint16_t minor,
                             const int8_t measured_power) {
  BleIBeacon ble_ibeacon = {};

  // Set header
  std::memcpy(&ble_ibeacon.ibeacon_head, &IBEACON_HEADER,
              sizeof(IBEACON_HEADER));

  // Specify vendor
  std::memcpy(&ble_ibeacon.ibeacon_vendor.proximity_uuid, proximity_uuid, 16);
  ble_ibeacon.ibeacon_vendor.major = endian_change_u16(major);  // BigEndian
  ble_ibeacon.ibeacon_vendor.minor = endian_change_u16(minor);  // BigEndian
  ble_ibeacon.ibeacon_vendor.measured_power = measured_power;

  return ble_ibeacon;
}

}  // namespace brdf_receiver_system

#endif  // ibeacon_H_
