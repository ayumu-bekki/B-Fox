#ifndef RECEIVER_SETTING_H_
#define RECEIVER_SETTING_H_
// ESP32 BRDF Beacon
// (C)2025 bekki.jp

// Include ----------------------
#include <cstdint>
#include <memory>
#include <string>

#include <cJSON.h>
#include <esp_bt.h>

namespace brdf_receiver_system {

class ReceiverSetting final {
 public:
  ReceiverSetting();

  bool Save();
  bool Load();
  bool Delete();

  bool IsActive() const;

  uint16_t GetMajor() const;

  void SetMajor(uint16_t major);

 private:
  bool Parse(const std::string& body) noexcept;

 private:
  bool is_active_;
  uint16_t major_;
};

using ReceiverSettingSharedPtr = std::shared_ptr<ReceiverSetting>;
using ReceiverSettingConstSharedPtr = std::shared_ptr<const ReceiverSetting>;
using ReceiverSettingWeakPtr = std::weak_ptr<ReceiverSetting>;
using ReceiverSettingConstWeakPtr = std::weak_ptr<const ReceiverSetting>;

}  // namespace brdf_receiver_system

#endif  // RECEIVER_SETTING_H_
// EOF
