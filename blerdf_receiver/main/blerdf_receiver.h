#ifndef BleRDF_RECEIVER_MAIN_BleRDF_RECEIVER_H_
#define BleRDF_RECEIVER_MAIN_BleRDF_RECEIVER_H_
// ESP32 BleRDF Receiver
// (C)2025 bekki.jp

// Include ----------------------
#include <memory>

#include "beacon_receive_task.h"
#include "blerdf_receiver_interface.h"
#include "gpio_input_watch_task.h"
#include "st7032.h"

namespace blerdf_receiver_system {

/// BleRDFReceiver
class BleRDFReceiver final : public BleRDFReceiverInterface,
                           public std::enable_shared_from_this<BleRDFReceiver> {
 public:
  enum class ReceiverStatus : int32_t {
    kSearchMode,
    kSettingMode,
    kSettingFinishMode,
  };

 public:
  BleRDFReceiver();
  ~BleRDFReceiver();

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

}  // namespace blerdf_receiver_system

#endif  // BleRDF_RECEIVER_MAIN_BleRDF_RECEIVER_H_
