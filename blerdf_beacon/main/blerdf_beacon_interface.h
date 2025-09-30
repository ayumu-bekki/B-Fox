#ifndef BleRDF_BEACON_MAIN_BleRDF_BEACON_INTERFACE_H_
#define BleRDF_BEACON_MAIN_BleRDF_BEACON_INTERFACE_H_
// ESP32 BleRDF Beacon
// (C)2025 bekki.jp

#include <chrono>
#include <memory>

#include "beacon_setting.h"

namespace blerdf_beacon_system {

class BleRDFBeaconInterface {
 public:
  virtual ~BleRDFBeaconInterface() = default;

  virtual float GetBatteryVoltage() const = 0;
  virtual BeaconSettingConstWeakPtr GetSetting() const = 0;
};

using BleRDFBeaconInterfaceSharedPtr = std::shared_ptr<BleRDFBeaconInterface>;
using BleRDFBeaconInterfaceConstSharedPtr =
    std::shared_ptr<const BleRDFBeaconInterface>;
using BleRDFBeaconInterfaceWeakPtr = std::weak_ptr<BleRDFBeaconInterface>;
using BleRDFBeaconInterfaceConstWeakPtr =
    std::weak_ptr<const BleRDFBeaconInterface>;

}  // namespace blerdf_beacon_system

#endif  // BleRDF_BEACON_MAIN_BleRDF_BEACON_INTERFACE_H_
