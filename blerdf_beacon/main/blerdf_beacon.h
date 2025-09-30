#ifndef BleRDF_BEACON_MAIN_BleRDF_BEACON_H_
#define BleRDF_BEACON_MAIN_BleRDF_BEACON_H_
// ESP32 BleRDF Beacon
// (C)2025 bekki.jp

#include <memory>

#include "beacon_setting.h"
#include "blerdf_beacon_interface.h"
#include "voltage_check_task.h"

namespace blerdf_beacon_system {

/// BleRDFBeacon
class BleRDFBeacon final : public BleRDFBeaconInterface,
                         public std::enable_shared_from_this<BleRDFBeacon> {
 public:
  BleRDFBeacon();
  ~BleRDFBeacon();

  void Start();

  float GetBatteryVoltage() const override;
  BeaconSettingConstWeakPtr GetSetting() const override;

 private:
  void CreateBLEService();

 private:
  VoltageCheckTaskUniquePtr voltage_check_task_;
  BeaconSettingSharedPtr setting_;
};

}  // namespace blerdf_beacon_system

#endif  // BleRDF_BEACON_MAIN_BleRDF_BEACON_H_
