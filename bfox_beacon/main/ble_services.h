#ifndef BFOX_BEACON_MAIN_BLE_SERVICES_H_
#define BFOX_BEACON_MAIN_BLE_SERVICES_H_
// ESP32 B-Fox Beacon
// (C)2025 bekki.jp

#include <memory>
#include <vector>

#include "bfox_beacon_interface.h"

// Forward declare for NimBLE
struct ble_gatt_svc_def;
struct ble_gatt_access_ctxt;

namespace bfox_beacon_system {

class BleCharacteristicInterface {
 public:
  virtual ~BleCharacteristicInterface() {}

  virtual void Write(const std::vector<uint8_t>* const data) = 0;
  virtual void Read(std::vector<uint8_t>* const data) = 0;
};

using BleCharacteristicInterfaceSharedPtr =
    std::shared_ptr<BleCharacteristicInterface>;

class BleVoltageCharacteristic final : public BleCharacteristicInterface {
 public:
  explicit BleVoltageCharacteristic(
      const BFoxBeaconInterfaceWeakPtr bfox_beacon_interface);

  void Write(const std::vector<uint8_t>* const data) override {};
  void Read(std::vector<uint8_t>* const data) override;

 private:
  const BFoxBeaconInterfaceWeakPtr bfox_beacon_interface_;
};

class BleBeaconSettingCharacteristic final : public BleCharacteristicInterface {
 public:
  explicit BleBeaconSettingCharacteristic(
      const BFoxBeaconInterfaceWeakPtr bfox_beacon_interface);

  void Write(const std::vector<uint8_t>* const data) override;
  void Read(std::vector<uint8_t>* const data) override;

 private:
  const BFoxBeaconInterfaceWeakPtr bfox_beacon_interface_;
};

class BleDeepSleepCharacteristic final : public BleCharacteristicInterface {
 public:
  explicit BleDeepSleepCharacteristic(
      const BFoxBeaconInterfaceWeakPtr bfox_beacon_interface);

  void Write(const std::vector<uint8_t>* const data) override;
  void Read(std::vector<uint8_t>* const data) override {};

 private:
  const BFoxBeaconInterfaceWeakPtr bfox_beacon_interface_;
};

class BleBFoxService final {
 public:
  explicit BleBFoxService(
      const BFoxBeaconInterfaceWeakPtr bfox_beacon_interface);

  // Return the NimBLE GATT service definition array
  const struct ble_gatt_svc_def* GetServiceDefs();

  static int GattSvrChrAccessStatic(uint16_t conn_handle, uint16_t attr_handle,
                                    struct ble_gatt_access_ctxt* ctxt,
                                    void* arg);

 private:
  int GattSvrChrAccess(uint16_t conn_handle, uint16_t attr_handle,
                       struct ble_gatt_access_ctxt* ctxt, void* arg);

 private:
  BleCharacteristicInterfaceSharedPtr voltage_char_;
  BleCharacteristicInterfaceSharedPtr setting_char_;
  BleCharacteristicInterfaceSharedPtr sleep_char_;
};

}  // namespace bfox_beacon_system

#endif  // BFOX_BEACON_MAIN_BLE_SERVICES_H_
