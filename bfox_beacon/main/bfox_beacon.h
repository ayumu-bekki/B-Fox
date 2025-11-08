#ifndef BFOX_BEACON_MAIN_BFOX_BEACON_H_
#define BFOX__BEACON_MAIN_BFOX_BEACON_H_
// ESP32 B-Fox Beacon
// (C)2025 bekki.jp

#include <memory>

#include "beacon_setting.h"
#include "bfox_beacon_interface.h"
#include "voltage_check_task.h"

namespace bfox_beacon_system {

/// BFoxBeacon
class BFoxBeacon final : public BFoxBeaconInterface,
                         public std::enable_shared_from_this<BFoxBeacon> {
 public:
  BFoxBeacon();
  ~BFoxBeacon();

  void Start();

  float GetBatteryVoltage() const override;
  BeaconSettingConstWeakPtr GetSetting() const override;

 private:
  void CreateBLEService();

 private:
  VoltageCheckTaskUniquePtr voltage_check_task_;
  BeaconSettingSharedPtr setting_;
};

}  // namespace bfox_beacon_system

#endif  // BFOX_BEACON_MAIN_BFOX_BEACON_H_
