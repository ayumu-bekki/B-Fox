#ifndef brdf_beacon_H_
#define brdf_beacon_H_
// ESP32 BRDF Beacon
// (C)2025 bekki.jp

// Include ----------------------
#include <memory>

#include "beacon_setting.h"
#include "brdf_beacon_interface.h"
#include "voltage_check_task.h"

namespace BrdfBeaconSystem {

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

}  // namespace BrdfBeaconSystem

#endif  // brdf_beacon_H_
