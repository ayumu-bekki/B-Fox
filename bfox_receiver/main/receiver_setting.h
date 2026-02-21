#ifndef BFOX_RECEIVER_MAIN_RECEIVER_SETTING_H_
#define BFOX_RECEIVER_MAIN_RECEIVER_SETTING_H_
// ESP32 B-Fox Beacon
// (C)2025 bekki.jp

// Include ----------------------
#include <nvs.h>

#include <cstdint>
#include <memory>

namespace bfox_receiver_system {

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
  bool is_active_;
  uint16_t major_;
};

using ReceiverSettingSharedPtr = std::shared_ptr<ReceiverSetting>;
using ReceiverSettingConstSharedPtr = std::shared_ptr<const ReceiverSetting>;
using ReceiverSettingWeakPtr = std::weak_ptr<ReceiverSetting>;
using ReceiverSettingConstWeakPtr = std::weak_ptr<const ReceiverSetting>;

}  // namespace bfox_receiver_system

#endif  // BFOX_RECEIVER_MAIN_RECEIVER_SETTING_H_
// EOF
