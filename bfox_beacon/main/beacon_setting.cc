// ESP32 bfox Beacon
// (C)2025 bekki.jp

#include "beacon_setting.h"

#include <nvs_flash.h>

#include "logger.h"

namespace bfox_beacon_system {

static constexpr const char kNvsNamespace[] = "bfox";
static constexpr const char kKeyDeviceName[] = "device_name";
static constexpr const char kKeyMajor[] = "major";
static constexpr const char kKeyMinor[] = "minor";
static constexpr const char kKeyMeasuredPower[] = "measured_power";
static constexpr const char kKeyTxPower[] = "tx_power";
static constexpr const char kKeyAdvIntervalMs[] = "adv_interval_ms";

BeaconSetting::BeaconSetting()
    : is_active_(false),
      device_name_("B-Fox Beacon"),
      major_(0),
      minor_(0),
      measured_power_(-59),
      tx_power_(TxPower::kHigh),
      adv_interval_ms_(500) {}

bool BeaconSetting::Save() {
  nvs_handle_t handle;
  esp_err_t err = nvs_open(kNvsNamespace, NVS_READWRITE, &handle);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "NVS open failed: %s", esp_err_to_name(err));
    return false;
  }

  bool ok = true;
  ok &= (nvs_set_str(handle, kKeyDeviceName, device_name_.c_str()) == ESP_OK);
  ok &= (nvs_set_u16(handle, kKeyMajor, major_) == ESP_OK);
  ok &= (nvs_set_u16(handle, kKeyMinor, minor_) == ESP_OK);
  ok &= (nvs_set_i32(handle, kKeyMeasuredPower, measured_power_) == ESP_OK);
  ok &= (nvs_set_i32(handle, kKeyTxPower, tx_power_) == ESP_OK);
  ok &= (nvs_set_u16(handle, kKeyAdvIntervalMs, adv_interval_ms_) == ESP_OK);

  if (ok) {
    err = nvs_commit(handle);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "NVS commit failed: %s", esp_err_to_name(err));
      ok = false;
    }
  } else {
    ESP_LOGE(TAG, "NVS set failed");
  }

  nvs_close(handle);
  return ok;
}

bool BeaconSetting::Load() {
  nvs_handle_t handle;
  esp_err_t err = nvs_open(kNvsNamespace, NVS_READONLY, &handle);
  if (err != ESP_OK) {
    ESP_LOGW(TAG, "NVS open failed (first boot?): %s", esp_err_to_name(err));
    return false;
  }

  // device_name
  size_t name_len = 0;
  err = nvs_get_str(handle, kKeyDeviceName, nullptr, &name_len);
  if (err != ESP_OK || name_len == 0) {
    ESP_LOGE(TAG, "NVS get device_name length failed: %s", esp_err_to_name(err));
    nvs_close(handle);
    return false;
  }
  std::string device_name(name_len, '\0');
  err = nvs_get_str(handle, kKeyDeviceName, &device_name[0], &name_len);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "NVS get device_name failed: %s", esp_err_to_name(err));
    nvs_close(handle);
    return false;
  }
  // nvs_get_str includes null terminator in name_len; trim it
  if (!device_name.empty() && device_name.back() == '\0') {
    device_name.pop_back();
  }

  uint16_t major = 0;
  uint16_t minor = 0;
  int32_t measured_power = -59;
  int32_t tx_power = TxPower::kHigh;
  uint16_t adv_interval_ms = 100;

  if (nvs_get_u16(handle, kKeyMajor, &major) != ESP_OK ||
      nvs_get_u16(handle, kKeyMinor, &minor) != ESP_OK ||
      nvs_get_i32(handle, kKeyMeasuredPower, &measured_power) != ESP_OK ||
      nvs_get_i32(handle, kKeyTxPower, &tx_power) != ESP_OK) {
    ESP_LOGE(TAG, "NVS get numeric value failed");
    nvs_close(handle);
    return false;
  }

  // adv_interval_ms is optional (backward compat: default 100ms if missing)
  if (nvs_get_u16(handle, kKeyAdvIntervalMs, &adv_interval_ms) != ESP_OK) {
    ESP_LOGW(TAG, "adv_interval_ms not found in NVS, using default 500ms");
    adv_interval_ms = 500;
  }

  nvs_close(handle);

  device_name_ = device_name;
  major_ = major;
  minor_ = minor;
  measured_power_ = measured_power;
  tx_power_ = tx_power;
  adv_interval_ms_ = adv_interval_ms;
  is_active_ = true;

  return true;
}

bool BeaconSetting::Delete() {
  nvs_handle_t handle;
  esp_err_t err = nvs_open(kNvsNamespace, NVS_READWRITE, &handle);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "NVS open failed: %s", esp_err_to_name(err));
    return false;
  }
  err = nvs_erase_all(handle);
  if (err == ESP_OK) {
    nvs_commit(handle);
  }
  nvs_close(handle);
  return err == ESP_OK;
}

bool BeaconSetting::IsActive() const { return is_active_; }

const std::string &BeaconSetting::GetDeviceName() const { return device_name_; }

uint16_t BeaconSetting::GetMajor() const { return major_; }

uint16_t BeaconSetting::GetMinor() const { return minor_; }

int32_t BeaconSetting::GetMeasuredPower() const { return measured_power_; }

int32_t BeaconSetting::GetTxPower() const { return tx_power_; }

uint16_t BeaconSetting::GetAdvIntervalMs() const { return adv_interval_ms_; }

esp_power_level_t BeaconSetting::GetEspTxPowerLevel() const {
  if (tx_power_ <= 0 || kMaxTxPower <= tx_power_) {
    ESP_LOGW(TAG, "Invalid Tx Power Value. %d", tx_power_);
    return ESP_PWR_LVL_P9;
  }
  constexpr esp_power_level_t kTxPowerToEspPowerLevelTable[kMaxTxPower] = {
      ESP_PWR_LVL_P9,   // kNone
      ESP_PWR_LVL_P9,   // kHigh
      ESP_PWR_LVL_P6,   // kMid
      ESP_PWR_LVL_P3,   // kLow
      ESP_PWR_LVL_N12,  // kSLow
  };
  return kTxPowerToEspPowerLevelTable[tx_power_];
}

void BeaconSetting::SetDeviceName(const std::string &device_name) {
  device_name_ = device_name;
}
void BeaconSetting::SetMajor(uint16_t major) { major_ = major; }
void BeaconSetting::SetMinor(uint16_t minor) { minor_ = minor; }
void BeaconSetting::SetMeasuredPower(int32_t measured_power) {
  measured_power_ = measured_power;
}
void BeaconSetting::SetTxPower(int32_t tx_power) { tx_power_ = tx_power; }
void BeaconSetting::SetAdvIntervalMs(uint16_t adv_interval_ms) {
  adv_interval_ms_ = adv_interval_ms;
}

}  // namespace bfox_beacon_system
