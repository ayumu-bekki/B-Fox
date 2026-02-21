// ESP32 B-Fox Receiver
// (C)2025 bekki.jp

// Include ----------------------
#include "receiver_setting.h"

#include <nvs_flash.h>

#include "logger.h"

namespace bfox_receiver_system {

static constexpr const char kNvsNamespace[] = "bfox";
static constexpr const char kKeyMajor[] = "major";

ReceiverSetting::ReceiverSetting() : is_active_(false), major_(0) {}

bool ReceiverSetting::Save() {
  nvs_handle_t handle;
  esp_err_t err = nvs_open(kNvsNamespace, NVS_READWRITE, &handle);
  if (err != ESP_OK) {
    ESP_LOGE(kTag, "NVS open failed: %s", esp_err_to_name(err));
    return false;
  }

  bool ok = (nvs_set_u16(handle, kKeyMajor, major_) == ESP_OK);
  if (ok) {
    err = nvs_commit(handle);
    if (err != ESP_OK) {
      ESP_LOGE(kTag, "NVS commit failed: %s", esp_err_to_name(err));
      ok = false;
    }
  } else {
    ESP_LOGE(kTag, "NVS set failed");
  }

  nvs_close(handle);
  return ok;
}

bool ReceiverSetting::Load() {
  nvs_handle_t handle;
  esp_err_t err = nvs_open(kNvsNamespace, NVS_READONLY, &handle);
  if (err != ESP_OK) {
    ESP_LOGW(kTag, "NVS open failed (first boot?): %s", esp_err_to_name(err));
    return false;
  }

  uint16_t major = 0;
  err = nvs_get_u16(handle, kKeyMajor, &major);
  nvs_close(handle);

  if (err != ESP_OK) {
    ESP_LOGE(kTag, "NVS get major failed: %s", esp_err_to_name(err));
    return false;
  }

  major_ = major;
  is_active_ = true;
  return true;
}

bool ReceiverSetting::Delete() {
  nvs_handle_t handle;
  esp_err_t err = nvs_open(kNvsNamespace, NVS_READWRITE, &handle);
  if (err != ESP_OK) {
    ESP_LOGE(kTag, "NVS open failed: %s", esp_err_to_name(err));
    return false;
  }
  err = nvs_erase_all(handle);
  if (err == ESP_OK) {
    nvs_commit(handle);
  }
  nvs_close(handle);
  return err == ESP_OK;
}

bool ReceiverSetting::IsActive() const { return is_active_; }

uint16_t ReceiverSetting::GetMajor() const { return major_; }

void ReceiverSetting::SetMajor(uint16_t major) { major_ = major; }

}  // namespace bfox_receiver_system

// EOF
