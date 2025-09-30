#ifndef BRDF_BEACON_MAIN_BRDF_BEACON_H_
#define BRDF_BEACON_MAIN_BRDF_BEACON_H_
// ESP32 BRDF Beacon
// (C)2025 bekki.jp

#include <memory>

#include "beacon_setting.h"
#include "brdf_beacon_interface.h"
#include "voltage_check_task.h"

namespace brdf_beacon_system {

/// BrdfBeacon
class BrdfBeacon final : public BrdfBeaconInterface,
                         public std::enable_shared_from_this<BrdfBeacon> {
 public:
  BrdfBeacon();
  ~BrdfBeacon();

  void Start();

  float GetBatteryVoltage() const override;
  BeaconSettingConstWeakPtr GetSetting() const override;

 private:
  void CreateBLEService();

 private:
  VoltageCheckTaskUniquePtr voltage_check_task_;
  BeaconSettingSharedPtr setting_;
};

}  // namespace brdf_beacon_system

#endif  // BRDF_BEACON_MAIN_BRDF_BEACON_H_
