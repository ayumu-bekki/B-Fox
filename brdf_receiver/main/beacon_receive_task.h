#ifndef BLE_RECEIVE_TASK_H_
#define BLE_RECEIVE_TASK_H_
// ESP32 BRDF Receiver
// (C)2025 bekki.jp

// Include ----------------------
#include <memory>
#include <mutex>
#include <set>
#include <vector>

#include <esp_gap_ble_api.h>
#include <soc/soc.h>

#include "ble_beacon_item.h"
#include "task.h"

namespace brdf_receiver_system {

class BeaconReceiveTask final : public Task {
 public:
  static constexpr char *const kTaskName = (char *)"BeaconReceiveTask";
  static constexpr int kPriority = Task::kPriorityLow;
  static constexpr int kCoreId = tskNO_AFFINITY;

 public:
  static BeaconReceiveTask *instance_;

 public:
  BeaconReceiveTask(const uint8_t target_proximity_uuid[16],
                    const uint16_t target_major_id);

  void Initialize() override;

  void Update() override;

  void EventGap(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);

  std::vector<BleBeaconItem> GetSSRISortedItems();

 private:
  static void EventGapStatic(esp_gap_ble_cb_event_t event,
                             esp_ble_gap_cb_param_t *param);

  bool IsIBeaconPacket(const uint8_t *const adv_data,
                       const uint8_t adv_data_len) const;

 private:
  std::mutex beacon_items_mutex_;
  std::set<BleBeaconItem> ble_beacon_items_;

  uint8_t target_proximity_uuid_[16];
  uint16_t target_major_id_;
};

using BeaconReceiveTaskUniquePtr = std::unique_ptr<BeaconReceiveTask>;

}  // namespace brdf_receiver_system

#endif  // BLE_RECEIVE_TASK_H_
