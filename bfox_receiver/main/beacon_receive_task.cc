// ESP32 B-Fox Receiver
// (C)2025 bekki.jp

// Include ----------------------
#include "beacon_receive_task.h"

#include <esp_timer.h>
#include <freertos/FreeRTOS.h>

#include <algorithm>
#include <cstring>

#include "gpio_control.h"
#include "ibeacon.h"
#include "logger.h"
#include "util.h"

// NimBLE Includes
#include "host/ble_hs.h"
#include "host/util/util.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"

namespace bfox_receiver_system {

BeaconReceiveTask* BeaconReceiveTask::instance_ = nullptr;

BeaconReceiveTask::BeaconReceiveTask(const uint8_t target_proximity_uuid[16],
                                     const uint16_t target_major_id)
    : Task(kTaskName, kPriority, kCoreId),
      beacon_items_mutex_(),
      ble_beacon_items_(),
      target_proximity_uuid_(),
      target_major_id_(target_major_id) {
  std::memcpy(target_proximity_uuid_, target_proximity_uuid,
              sizeof(target_proximity_uuid_));
}

void BeaconReceiveTask::Initialize() {
  instance_ = this;

  int rc = nimble_port_init();
  if (rc != 0) {
    ESP_LOGE(kTag, "nimble_port_init failed: %d", rc);
    return;
  }

  ble_hs_cfg.sync_cb = OnSyncStatic;
  ble_hs_cfg.store_status_cb = ble_store_util_status_rr;

  nimble_port_freertos_init(HostTaskStatic);
}

void BeaconReceiveTask::Update() { util::SleepMillisecond(2000); }

std::vector<BleBeaconItem> BeaconReceiveTask::GetRSSISortedItems() {
  const int64_t now_ms = esp_timer_get_time() / 1000;
  std::vector<BleBeaconItem> ble_beacon_list;
  {
    std::scoped_lock lock(beacon_items_mutex_);
    // Remove entries not seen within the expiry window
    for (auto it = ble_beacon_items_.begin(); it != ble_beacon_items_.end();) {
      if ((now_ms - it->last_seen_ms) > kBeaconExpiryMs) {
        it = ble_beacon_items_.erase(it);
      } else {
        ++it;
      }
    }
    ble_beacon_list.reserve(ble_beacon_items_.size());
    ble_beacon_list.assign(ble_beacon_items_.begin(), ble_beacon_items_.end());
  }
  std::sort(ble_beacon_list.begin(), ble_beacon_list.end(),
            [](const BleBeaconItem& a, const BleBeaconItem& b) {
              return a.rssi > b.rssi;
            });
  return ble_beacon_list;
}

void BeaconReceiveTask::HostTaskStatic(void* param) {
  if (instance_) {
    instance_->HostTask();
  }
}

void BeaconReceiveTask::HostTask() {
  ESP_LOGI(kTag, "NimBLE host task started");
  nimble_port_run();
  nimble_port_freertos_deinit();
}

void BeaconReceiveTask::OnSyncStatic() {
  if (instance_) {
    instance_->OnSync();
  }
}

void BeaconReceiveTask::OnSync() {
  ESP_LOGI(kTag, "NimBLE host synced, starting scan");

  struct ble_gap_disc_params disc_params;
  std::memset(&disc_params, 0, sizeof(disc_params));
  disc_params.filter_duplicates = 0; // Receive all updates for accurate RSSI and expiry
  disc_params.passive = 1;
  disc_params.itvl = 0x00A0; // 160 * 0.625ms = 100ms
  disc_params.window = 0x00A0; // 160 * 0.625ms = 100ms (continuous scan)

  uint8_t own_addr_type;
  int rc = ble_hs_id_infer_auto(0, &own_addr_type);
  if (rc != 0) {
    ESP_LOGE(kTag, "ble_hs_id_infer_auto failed: %d", rc);
    return;
  }

  rc = ble_gap_disc(own_addr_type, BLE_HS_FOREVER, &disc_params, GapEventStatic, nullptr);
  if (rc != 0) {
    ESP_LOGE(kTag, "Scanning start failed, error %d", rc);
  } else {
    ESP_LOGI(kTag, "Scanning start successfully");
  }
}

int BeaconReceiveTask::GapEventStatic(struct ble_gap_event* event, void* arg) {
  if (instance_ != nullptr) {
    return instance_->GapEvent(event, arg);
  }
  return 0;
}

int BeaconReceiveTask::GapEvent(struct ble_gap_event* event, void* arg) {
  if (event->type == BLE_GAP_EVENT_DISC) {
    if (IsIBeaconPacket(event->disc.data, event->disc.length_data)) {
      const BleIBeacon* ibeacon_data =
          reinterpret_cast<const BleIBeacon*>(event->disc.data);
      const uint16_t major = EndianChangeU16(ibeacon_data->ibeacon_vendor.major);
      const uint16_t minor = EndianChangeU16(ibeacon_data->ibeacon_vendor.minor);

      if (std::memcmp(ibeacon_data->ibeacon_vendor.proximity_uuid,
                      target_proximity_uuid_,
                      sizeof(target_proximity_uuid_)) != 0 ||
          major != target_major_id_) {
        return 0;
      }

      BleBeaconItem item = {.minor = minor,
                            .rssi = event->disc.rssi,
                            .last_seen_ms = esp_timer_get_time() / 1000};

      {
        std::scoped_lock lock(beacon_items_mutex_);

        auto [it, inserted] = ble_beacon_items_.insert(item);
        if (!inserted) {
          ble_beacon_items_.erase(it);
          ble_beacon_items_.insert(item);
        }
      }
    }
  }
  return 0;
}

bool BeaconReceiveTask::IsIBeaconPacket(const uint8_t* adv_data,
                                        const uint8_t adv_data_len) const {
  if (adv_data != nullptr && adv_data_len == 0x1e) {
    if (!std::memcmp(adv_data, reinterpret_cast<const uint8_t*>(&kIBeaconHeader),
                     sizeof(kIBeaconHeader))) {
      return true;
    }
  }
  return false;
}

}  // namespace bfox_receiver_system
