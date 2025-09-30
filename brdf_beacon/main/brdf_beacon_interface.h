#ifndef brdf_beacon_INTERFACE_H_
#define brdf_beacon_INTERFACE_H_
// ESP32 BRDF Beacon
// (C)2025 bekki.jp

// Include ----------------------
#include <chrono>
#include <memory>

#include "beacon_setting.h"

namespace BrdfBeaconSystem {

class BeaconSetting;
using BeaconSettingConstWeakPtr = std::weak_ptr<const BeaconSetting>;

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

}  // namespace BrdfBeaconSystem

#endif  // brdf_beacon_INTERFACE_H_
