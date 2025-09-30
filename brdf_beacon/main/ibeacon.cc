// ESP32 BRDF Receiver
// (C)2025 bekki.jp
#include "ibeacon.h"

#include <cstring>

namespace BrdfBeaconSystem {

uint16_t endian_change_u16(const uint16_t data) {
  return ((data & 0xff00) >> 8) + ((data & 0x00ff) << 8);
}

BleIBeacon CreateiBeaconAttr(const uint8_t *const proximity_uuid,
                             const uint16_t major, const uint16_t minor,
                             const int8_t measured_power) {
  BleIBeacon ble_ibeacon = {
      .ibeacon_head = {.flags = {0x02, 0x01, 0x06},
                       .length = 0x1A,
                       .type = 0xFF,
                       .company_id = 0x004C,
                       .beacon_type = 0x1502},
      .ibeacon_vendor = {.proximity_uuid = {},
                         .major = endian_change_u16(major),  // BigEndian
                         .minor = endian_change_u16(minor),  // BigEndian
                         .measured_power = measured_power}};
  std::memcpy(&ble_ibeacon.ibeacon_vendor.proximity_uuid, proximity_uuid,
              ESP_UUID_LEN_128);

  /*
    // Set header
    constexpr BleIBeaconHead IBEACON_HEADER = {.flags = {0x02, 0x01, 0x06},
                                               .length = 0x1A,
                                               .type = 0xFF,
                                               .company_id = 0x004C,
                                               .beacon_type = 0x1502};
    std::memcpy(&ble_ibeacon.ibeacon_head, &IBEACON_HEADER,
                sizeof(IBEACON_HEADER));
    // Set vendor
    std::memcpy(&ble_ibeacon.ibeacon_vendor.proximity_uuid, proximity_uuid,
    ESP_UUID_LEN_128); ble_ibeacon.ibeacon_vendor.major =
    endian_change_u16(major);  // BigEndian ble_ibeacon.ibeacon_vendor.minor =
    endian_change_u16(minor);  // BigEndian
    ble_ibeacon.ibeacon_vendor.measured_power = measured_power;
  */

  return ble_ibeacon;
}

}  // namespace BrdfBeaconSystem
