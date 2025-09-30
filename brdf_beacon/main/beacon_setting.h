#ifndef BEACON_SETTING_H_
#define BEACON_SETTING_H_
// ESP32 BRDF Beacon
// (C)2025 bekki.jp

// Include ----------------------
#include <cJSON.h>
#include <esp_bt.h>

#include <cstdint>
#include <memory>
#include <string>

namespace BrdfBeaconSystem {

class BeaconSetting final {
 public:
  enum TxPower {
    None = 0,  // None
    High,      // + 9dbm (8mW)    ESP_PWR_LVL_P9
    Mid,       // + 6dbm (4mW)    ESP_PWR_LVL_P6
    Low,       // + 3dbm (2mW)    ESP_PWR_LVL_P3
    S_Low,     // -12dBm (0.06mW) ESP_PWR_LVL_N12
    MaxTxPower,
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
  esp_power_level_t GetEspTxPowerLevel() const;

  void SetDeviceName(const std::string& device_name);
  void SetMajor(uint16_t major);
  void SetMinor(uint16_t minor);
  void SetMeasuredPower(int32_t measured_power);
  void SetTxPower(int32_t tx_power);

 private:
  bool Parse(const std::string& body) noexcept;

 private:
  bool is_active_;
  std::string device_name_;
  uint16_t major_;
  uint16_t minor_;
  int32_t measured_power_;
  int32_t tx_power_;
};

using BeaconSettingSharedPtr = std::shared_ptr<BeaconSetting>;
using BeaconSettingConstSharedPtr = std::shared_ptr<const BeaconSetting>;
using BeaconSettingWeakPtr = std::weak_ptr<BeaconSetting>;
using BeaconSettingConstWeakPtr = std::weak_ptr<const BeaconSetting>;

}  // namespace BrdfBeaconSystem

#endif  // BEACON_SETTING_H_
// EOF
