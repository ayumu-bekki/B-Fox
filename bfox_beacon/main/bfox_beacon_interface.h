#ifndef BFOX_BEACON_MAIN_BFOX_BEACON_INTERFACE_H_
#define BFOX_BEACON_MAIN_BFOX_BEACON_INTERFACE_H_
// ESP32 B-Fox Beacon
// (C)2025 bekki.jp

#include <chrono>
#include <memory>

#include "beacon_setting.h"

namespace bfox_beacon_system {

class BFoxBeaconInterface {
 public:
  virtual ~BFoxBeaconInterface() = default;

  virtual float GetBatteryVoltage() const = 0;
  virtual BeaconSettingConstWeakPtr GetSetting() const = 0;
};

using BFoxBeaconInterfaceSharedPtr = std::shared_ptr<BFoxBeaconInterface>;
using BFoxBeaconInterfaceConstSharedPtr =
    std::shared_ptr<const BFoxBeaconInterface>;
using BFoxBeaconInterfaceWeakPtr = std::weak_ptr<BFoxBeaconInterface>;
using BFoxBeaconInterfaceConstWeakPtr =
    std::weak_ptr<const BFoxBeaconInterface>;

}  // namespace bfox_beacon_system

#endif  // BFOX_BEACON_MAIN_BFOX_BEACON_INTERFACE_H_
