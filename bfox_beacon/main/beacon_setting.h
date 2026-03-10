#ifndef BFOX_BEACON_MAIN_BEACON_SETTING_H_
#define BFOX_BEACON_MAIN_BEACON_SETTING_H_
// ESP32 B-Fox Beacon
// (C)2025 bekki.jp

#include <esp_bt.h>
#include <nvs.h>

#include <cstdint>
#include <memory>
#include <string>

namespace bfox_beacon_system {

class BeaconSetting final {
 public:
  enum TxPower {
    kNone = 0,
    kP9,   // +9dBm (7.9mW)
    kP6,   // +6dBm (4.0mW)
    kP3,   // +3dBm (2.0mW)
    kN0,   //  0dBm (1.0mW)
    kN3,   // -3dBm (0.50mW)
    kN6,   // -6dBm (0.25mW)
    kN9,   // -9dBm (0.13mW)
    kN12,  //-12dBm (0.06mW)
    kN15,  //-15dBm (0.03mW)
    kMaxTxPower,
  };

 public:
  BeaconSetting();

  bool Save();
  bool Load();
  bool Delete();

  bool IsActive() const;

  const std::string& GetDeviceName() const;
  uint16_t GetMajor() const;
  uint16_t GetMinor() const;
  int32_t GetMeasuredPower() const;
  int32_t GetTxPower() const;
  uint16_t GetAdvIntervalMs() const;
  esp_power_level_t GetEspTxPowerLevel() const;

  void SetDeviceName(const std::string& device_name);
  void SetMajor(uint16_t major);
  void SetMinor(uint16_t minor);
  void SetMeasuredPower(int32_t measured_power);
  void SetTxPower(int32_t tx_power);
  void SetAdvIntervalMs(uint16_t adv_interval_ms);

 private:
  bool Parse(const std::string& body) noexcept;

 private:
  bool is_active_;
  std::string device_name_;
  uint16_t major_;
  uint16_t minor_;
  int32_t measured_power_;
  int32_t tx_power_;
  uint16_t adv_interval_ms_;
};

using BeaconSettingSharedPtr = std::shared_ptr<BeaconSetting>;
using BeaconSettingConstSharedPtr = std::shared_ptr<const BeaconSetting>;
using BeaconSettingWeakPtr = std::weak_ptr<BeaconSetting>;
using BeaconSettingConstWeakPtr = std::weak_ptr<const BeaconSetting>;

}  // namespace bfox_beacon_system

#endif  // BFOX_BEACON_MAIN_BEACON_SETTING_H_
