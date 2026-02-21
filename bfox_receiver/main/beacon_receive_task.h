#ifndef BFOX_RECEIVER_MAIN_BEACON_RECEIVE_TASK_H_
#define BFOX_RECEIVER_MAIN_BEACON_RECEIVE_TASK_H_
// ESP32 B-Fox Receiver
// (C)2025 bekki.jp

// Include ----------------------
#include <esp_gap_ble_api.h>
#include <soc/soc.h>

#include <memory>
#include <mutex>
#include <set>
#include <vector>

#include "ble_beacon_item.h"
#include "task.h"

namespace bfox_receiver_system {

class BeaconReceiveTask final : public Task {
 public:
  static constexpr const char* kTaskName = "BeaconReceiveTask";
  static constexpr int kPriority = Task::kPriorityLow;
  static constexpr int kCoreId = tskNO_AFFINITY;
  static constexpr int64_t kBeaconExpiryMs = 2000;  // entries unseen for 2s are removed

 private:
  static BeaconReceiveTask* instance_;

 public:
  BeaconReceiveTask(const uint8_t target_proximity_uuid[16],
                    const uint16_t target_major_id);

  void Initialize() override;

  void Update() override;

  void EventGap(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t* param);

  std::vector<BleBeaconItem> GetRSSISortedItems();

 private:
  static void EventGapStatic(esp_gap_ble_cb_event_t event,
                             esp_ble_gap_cb_param_t* param);

  bool IsIBeaconPacket(const uint8_t* adv_data,
                       const uint8_t adv_data_len) const;

 private:
  std::mutex beacon_items_mutex_;
  std::set<BleBeaconItem> ble_beacon_items_;

  uint8_t target_proximity_uuid_[16];
  uint16_t target_major_id_;
};

using BeaconReceiveTaskUniquePtr = std::unique_ptr<BeaconReceiveTask>;

}  // namespace bfox_receiver_system

#endif  // BFOX_RECEIVER_MAIN_BEACON_RECEIVE_TASK_H_
