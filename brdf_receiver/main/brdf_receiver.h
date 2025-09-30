#ifndef BRDF_RECEIVER_MAIN_BRDF_RECEIVER_H_
#define BRDF_RECEIVER_MAIN_BRDF_RECEIVER_H_
// ESP32 BRDF Receiver
// (C)2025 bekki.jp

// Include ----------------------
#include <memory>

#include "beacon_receive_task.h"
#include "brdf_receiver_interface.h"
#include "gpio_input_watch_task.h"
#include "st7032.h"

namespace brdf_receiver_system {

/// BrdfReceiver
class BrdfReceiver final : public BrdfReceiverInterface,
                           public std::enable_shared_from_this<BrdfReceiver> {
 public:
  enum class ReceiverStatus : int32_t {
    kSearchMode,
    kSettingMode,
    kSettingFinishMode,
  };

 public:
  BrdfReceiver();
  ~BrdfReceiver();

  void Start();

 private:
  void BeaconSearchMode();
  void SettingMode();
  void SettingFinishMode();

  void OnSetMajorButton();
  void OnSetMajorLongButton();

 private:
  GpioInputWatchTask gpio_watcher_;
  ST7032 st7032_;
  BeaconReceiveTaskUniquePtr beacon_receive_task_;
  ReceiverStatus receiver_status_;
  uint16_t major_;
  int32_t sleep_count_;
};

}  // namespace brdf_receiver_system

#endif  // BRDF_RECEIVER_MAIN_BRDF_RECEIVER_H_
