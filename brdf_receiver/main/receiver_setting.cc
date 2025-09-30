// ESP32 BRDF Beacon
// (C)2025 bekki.jp

// Include ----------------------
#include "receiver_setting.h"

#include <algorithm>
#include <sstream>
#include <stdexcept>

#include "file_system.h"
#include "logger.h"

namespace brdf_receiver_system {

constexpr char *const kSettingFileName = (char *)"receiver_setting.json";

ReceiverSetting::ReceiverSetting() : is_active_(false), major_(0) {}

bool ReceiverSetting::Save() {
  cJSON *json = cJSON_CreateObject();
  if (json == NULL) {
    ESP_LOGE(kTag, "Failed Create JSON Object");
    return false;
  }
  cJSON_AddNumberToObject(json, "major", major_);

  // Convert JSON to string
  char *json_string = cJSON_PrintUnformatted(json);
  if (json_string == nullptr) {
    ESP_LOGE(kTag, "Failed JSON String");
    cJSON_Delete(json);
    return false;
  }

  ESP_LOGI(kTag, "OutJson:%s", json_string);

  bool ret = file_system::Write(kSettingFileName, json_string);

  free(json_string);
  cJSON_Delete(json);

  return ret;
}

bool ReceiverSetting::Load() {
  std::string body;
  const bool is_read_ok = file_system::Read(kSettingFileName, body);
  if (!is_read_ok) {
    return false;
  }

  major_ = 0;

  cJSON *json_root = nullptr;
  json_root = cJSON_Parse(body.c_str());
  if (!json_root) {
    const char *error_ptr = cJSON_GetErrorPtr();
    if (error_ptr) {
      ESP_LOGE(kTag, "Parse Error %s", error_ptr);
      return false;
    }
    ESP_LOGE(kTag, "Parse Error");
    return false;
  }

  // Major
  const cJSON *const json_major =
      cJSON_GetObjectItemCaseSensitive(json_root, "major");
  if (!cJSON_IsNumber(json_major)) {
    ESP_LOGE(kTag, "Illegal object type major.");
    cJSON_Delete(json_root);
    return false;
  }
  major_ = json_major->valueint;

  is_active_ = true;

  cJSON_Delete(json_root);

  return true;
}

bool ReceiverSetting::Delete() { return file_system::Delete(kSettingFileName); }

bool ReceiverSetting::IsActive() const { return is_active_; }

uint16_t ReceiverSetting::GetMajor() const { return major_; }

void ReceiverSetting::SetMajor(uint16_t major) { major_ = major; }

}  // namespace brdf_receiver_system

// EOF
