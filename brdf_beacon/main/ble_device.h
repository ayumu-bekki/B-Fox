#ifndef BRDF_BEACON_MAIN_BLE_DEVICE_H_
#define BRDF_BEACON_MAIN_BLE_DEVICE_H_
// ESP32 BRDF Beacon
// (C)2025 bekki.jp

#include <esp_bt.h>
#include <esp_bt_defs.h>
#include <esp_bt_main.h>
#include <esp_gap_ble_api.h>
#include <esp_gatt_common_api.h>
#include <esp_gatts_api.h>

#include <memory>
#include <vector>

#include "ibeacon.h"

namespace brdf_beacon_system {

class BleCharacteristicInterface {
 public:
  virtual ~BleCharacteristicInterface() {}

  virtual void Write(const std::vector<uint8_t> *const data) = 0;
  virtual void Read(std::vector<uint8_t> *const data) = 0;

  virtual void SetHandle(const uint16_t handle) = 0;
  virtual uint16_t GetHandle() const = 0;

  virtual esp_bt_uuid_t GetUuid() const = 0;
  virtual esp_gatt_char_prop_t GetProperty() const = 0;
};

using BleCharacteristicInterfaceSharedPtr =
    std::shared_ptr<BleCharacteristicInterface>;

class BleServiceInterface {
 public:
  virtual ~BleServiceInterface() {}

  virtual void GattsEvent(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if,
                          esp_ble_gatts_cb_param_t *param) = 0;
  virtual void AddCharacteristic(
      BleCharacteristicInterfaceSharedPtr bleCharacteristic) = 0;

  virtual void SetGattsIf(const uint16_t gatts_if) = 0;
  virtual uint16_t GetAppId() const = 0;
  virtual uint16_t GetGattsIf() const = 0;
};

using BleServiceInterfaceSharedPtr = std::shared_ptr<BleServiceInterface>;

class BleDevice final {
 public:
  static BleDevice *GetInstance();

 private:
  static BleDevice *this_;

 public:
  void GapEvent(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);
  void GattsEvent(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if,
                  esp_ble_gatts_cb_param_t *param);
  void Initialize(const std::string &device_name, BleIBeacon &ibeacon_adv_data);
  void AddService(BleServiceInterfaceSharedPtr bleService);
  void StartAdvertising();

 private:
  BleDevice();

 private:
  std::vector<BleServiceInterfaceSharedPtr> services_;
};

}  // namespace brdf_beacon_system

#endif  // BRDF_BEACON_MAIN_BLE_DEVICE_H_
