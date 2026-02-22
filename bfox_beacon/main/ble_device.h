#ifndef BFOX_BEACON_MAIN_BLE_DEVICE_H_
#define BFOX_BEACON_MAIN_BLE_DEVICE_H_
// ESP32 B-Fox Beacon
// (C)2025 bekki.jp

#include <memory>
#include <string>

#include "ibeacon.h"

// Forward declare for NimBLE
struct ble_gatt_svc_def;
struct ble_gap_event;

namespace bfox_beacon_system {

class BleDevice final {
 public:
  static BleDevice* GetInstance();

 private:
  static BleDevice* this_;

 public:
  // Initialize NimBLE stack and setup device name/iBeacon data
  void Initialize(const std::string& device_name, BleIBeacon& ibeacon_adv_data);

  // Register NimBLE GATT services before starting the host
  void RegisterServices(const struct ble_gatt_svc_def* svcs);

  // Start the NimBLE host task
  void StartHost();

  // Start or restart iBeacon advertising
  void StartAdvertising(uint16_t interval_ms);
  void RestartAdvertising();

 private:
  BleDevice();

  static void HostTaskStatic(void* param);
  static void OnSyncStatic();
  static void OnResetStatic(int reason);
  static int GapEventStatic(struct ble_gap_event* event, void* arg);

  void HostTask();
  void OnSync();
  void OnReset(int reason);
  int GapEvent(struct ble_gap_event* event, void* arg);

 private:
  uint16_t adv_interval_ms_;
  BleIBeacon ibeacon_adv_data_;
  std::string device_name_;
};

}  // namespace bfox_beacon_system

#endif  // BFOX_BEACON_MAIN_BLE_DEVICE_H_
