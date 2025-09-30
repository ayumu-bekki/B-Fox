// ESP32 BleRDF Beacon
// (C)2025 bekki.jp

#include "beacon_setting.h"

#include <algorithm>
#include <sstream>
#include <stdexcept>

#include "file_system.h"
#include "logger.h"

namespace blerdf_beacon_system {

constexpr const char kSettingFileName[] = "beacon_setting.json";

BeaconSetting::BeaconSetting()
    : is_active_(false),
      device_name_("BleRDF Beacon"),
      major_(0),
      minor_(0),
      measured_power_(-59),
      tx_power_(TxPower::kHigh) {}

bool BeaconSetting::Save() {
  // ex) {"device_name": "BleRDF Beacon", "major":0, "minor":0}
  cJSON *json = cJSON_CreateObject();
  if (json == NULL) {
    ESP_LOGE(TAG, "Failed Create JSON Object");
    return false;
  }
  cJSON_AddStringToObject(json, "device_name", device_name_.c_str());
  cJSON_AddNumberToObject(json, "major", major_);
  cJSON_AddNumberToObject(json, "minor", minor_);
  cJSON_AddNumberToObject(json, "tx_power", tx_power_);

  // Convert JSON to string
  char *json_string = cJSON_PrintUnformatted(json);
  if (json_string == nullptr) {
    ESP_LOGE(TAG, "Failed JSON String");
    cJSON_Delete(json);
    return false;
  }

  ESP_LOGI(TAG, "OutJson:%s", json_string);

  bool ret = file_system::Write(kSettingFileName, json_string);

  free(json_string);
  cJSON_Delete(json);

  return ret;
}

bool BeaconSetting::Load() {
  std::string body;
  const bool is_read_ok = file_system::Read(kSettingFileName, body);
  if (!is_read_ok) {
    return false;
  }

  device_name_ = "";
  major_ = 0;
  minor_ = 0;
  tx_power_ = TxPower::kHigh;

  cJSON *json_root = nullptr;
  json_root = cJSON_Parse(body.c_str());
  if (!json_root) {
    const char *error_ptr = cJSON_GetErrorPtr();
    if (error_ptr) {
      ESP_LOGE(TAG, "Parse Error %s", error_ptr);
      return false;
    }
    ESP_LOGE(TAG, "Parse Error");
    return false;
  }

  // Get Device Name
  const cJSON *const json_device_name =
      cJSON_GetObjectItemCaseSensitive(json_root, "device_name");
  if (!cJSON_IsString(json_device_name)) {
    ESP_LOGE(TAG, "Illegal object type device_name.");
    cJSON_Delete(json_root);
    return false;
  }
  device_name_ = json_device_name->valuestring;

  // Major
  const cJSON *const json_major =
      cJSON_GetObjectItemCaseSensitive(json_root, "major");
  if (!cJSON_IsNumber(json_major)) {
    ESP_LOGE(TAG, "Illegal object type major.");
    cJSON_Delete(json_root);
    return false;
  }
  major_ = json_major->valueint;

  // Minor
  const cJSON *const json_minor =
      cJSON_GetObjectItemCaseSensitive(json_root, "minor");
  if (!cJSON_IsNumber(json_minor)) {
    ESP_LOGE(TAG, "Illegal object type minor.");
    cJSON_Delete(json_root);
    return false;
  }
  minor_ = json_minor->valueint;

  // TxPower
  const cJSON *const json_tx_power =
      cJSON_GetObjectItemCaseSensitive(json_root, "tx_power");
  if (!cJSON_IsNumber(json_tx_power)) {
    ESP_LOGE(TAG, "Illegal object type tx_power.");
    cJSON_Delete(json_root);
    return false;
  }
  tx_power_ = json_tx_power->valueint;

  is_active_ = true;

  cJSON_Delete(json_root);

  return true;
}

bool BeaconSetting::Delete() { return file_system::Delete(kSettingFileName); }

bool BeaconSetting::IsActive() const { return is_active_; }

const std::string &BeaconSetting::GetDeviceName() const { return device_name_; }

uint16_t BeaconSetting::GetMajor() const { return major_; }

uint16_t BeaconSetting::GetMinor() const { return minor_; }

int32_t BeaconSetting::GetMeasuredPower() const { return measured_power_; }

int32_t BeaconSetting::GetTxPower() const { return tx_power_; }

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

}  // namespace blerdf_beacon_system
