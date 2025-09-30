#ifndef BRDF_BEACON_MAIN_BRDF_BEACON_INTERFACE_H_
#define BRDF_BEACON_MAIN_BRDF_BEACON_INTERFACE_H_
// ESP32 BRDF Beacon
// (C)2025 bekki.jp

#include <chrono>
#include <memory>

#include "beacon_setting.h"

namespace brdf_beacon_system {

class BrdfBeaconInterface {
 public:
  virtual ~BrdfBeaconInterface() = default;

  virtual float GetBatteryVoltage() const = 0;
  virtual BeaconSettingConstWeakPtr GetSetting() const = 0;
};

using BrdfBeaconInterfaceSharedPtr = std::shared_ptr<BrdfBeaconInterface>;
using BrdfBeaconInterfaceConstSharedPtr =
    std::shared_ptr<const BrdfBeaconInterface>;
using BrdfBeaconInterfaceWeakPtr = std::weak_ptr<BrdfBeaconInterface>;
using BrdfBeaconInterfaceConstWeakPtr =
    std::weak_ptr<const BrdfBeaconInterface>;

}  // namespace brdf_beacon_system

#endif  // BRDF_BEACON_MAIN_BRDF_BEACON_INTERFACE_H_
