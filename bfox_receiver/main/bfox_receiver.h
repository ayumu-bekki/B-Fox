#ifndef BFOX_RECEIVER_MAIN_BFOX_RECEIVER_H_
#define BFOX_RECEIVER_MAIN_BFOX_RECEIVER_H_
// ESP32 B-Fox Receiver
// (C)2025 bekki.jp

// Include ----------------------
#include <atomic>
#include <cstdint>
#include <memory>

#include "beacon_receive_task.h"
#include "bfox_receiver_interface.h"
#include "gpio_input_watch_task.h"
#include "st7032.h"

namespace bfox_receiver_system {

/// BFoxReceiver
class BFoxReceiver final : public BFoxReceiverInterface,
                           public std::enable_shared_from_this<BFoxReceiver> {
 public:
  enum class ReceiverStatus : int32_t {
    kSearchMode,
    kSettingMode,
    kSettingFinishMode,
  };

 public:
  BFoxReceiver();
  ~BFoxReceiver();

  void Start();

 private:
  void BeaconSearchMode();
  void SettingMode();
  void SettingFinishMode();

  void OnActivityButton();
  void OnSetMajorButton();
  void OnSetMajorLongButton();

 private:
  GpioInputWatchTask gpio_watcher_;
  ST7032 st7032_;
  BeaconReceiveTaskUniquePtr beacon_receive_task_;
  ReceiverStatus receiver_status_;
  uint16_t major_;
  std::atomic<int64_t> sleep_deadline_ms_;  // absolute time (ms) to enter Deep Sleep
};

}  // namespace bfox_receiver_system

#endif  // BFOX_RECEIVER_MAIN_BFOX_RECEIVER_H_
