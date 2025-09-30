// ESP32 BRDF Receiver
// (C)2025 bekki.jp

// Include ----------------------
#include "beacon_receive_task.h"

#include <algorithm>
#include <cstring>

#include <esp_bt.h>
#include <esp_bt_defs.h>
#include <esp_bt_main.h>
#include <esp_gap_ble_api.h>
#include <esp_gatt_defs.h>
#include <esp_gattc_api.h>
#include <freertos/FreeRTOS.h>

#include "gpio_control.h"
#include "ibeacon.h"
#include "logger.h"
#include "util.h"

namespace brdf_receiver_system {

BeaconReceiveTask *BeaconReceiveTask::instance_ = nullptr;

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
  esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT);

  esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
  esp_bt_controller_init(&bt_cfg);
  esp_bt_controller_enable(ESP_BT_MODE_BLE);

  esp_bluedroid_init();
  esp_bluedroid_enable();

  instance_ = this;
  esp_err_t status =
      esp_ble_gap_register_callback(&BeaconReceiveTask::EventGapStatic);
  if (status != ESP_OK) {
    ESP_LOGE(kTag, "gap register error: %s", esp_err_to_name(status));
    return;
  }

  esp_ble_scan_params_t ble_scan_params = {
      .scan_type = BLE_SCAN_TYPE_PASSIVE,
      .own_addr_type = BLE_ADDR_TYPE_PUBLIC,
      .scan_filter_policy = BLE_SCAN_FILTER_ALLOW_ALL,
      .scan_interval = 0xA0,
      .scan_window = 0x30,
      .scan_duplicate = BLE_SCAN_DUPLICATE_DISABLE};
  esp_ble_gap_set_scan_params(&ble_scan_params);
}

void BeaconReceiveTask::Update() { util::SleepMillisecond(2000); }

std::vector<BleBeaconItem> BeaconReceiveTask::GetSSRISortedItems() {
  std::vector<BleBeaconItem> ble_beacon_list;
  {
    std::scoped_lock lock(beacon_items_mutex_);
    ble_beacon_list.insert(ble_beacon_list.end(), ble_beacon_items_.begin(),
                           ble_beacon_items_.end());
    ble_beacon_items_.clear();
  }
  std::sort(ble_beacon_list.begin(), ble_beacon_list.end(),
            [](const BleBeaconItem &a, const BleBeaconItem &b) {
              return a.rssi > b.rssi;
            });
  return ble_beacon_list;
}

void BeaconReceiveTask::EventGap(esp_gap_ble_cb_event_t event,
                                 esp_ble_gap_cb_param_t *param) {
  if (event == ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT) {
    // the unit of the duration is second, 0 means scan permanently
    uint32_t duration = 0;
    esp_ble_gap_start_scanning(duration);
  } else if (event == ESP_GAP_BLE_SCAN_START_COMPLETE_EVT) {
    esp_err_t err = param->scan_start_cmpl.status;
    if (err != ESP_BT_STATUS_SUCCESS) {
      ESP_LOGE(kTag, "Scanning start failed, error %s", esp_err_to_name(err));
    } else {
      ESP_LOGI(kTag, "Scanning start successfully");
    }
  } else if (event == ESP_GAP_BLE_SCAN_RESULT_EVT) {
    if (param->scan_rst.search_evt == ESP_GAP_SEARCH_INQ_RES_EVT) {
      if (IsIBeaconPacket(param->scan_rst.ble_adv,
                          param->scan_rst.adv_data_len)) {
        const BleIBeacon *const ibeacon_data =
            reinterpret_cast<const BleIBeacon *>(param->scan_rst.ble_adv);
        const uint16_t major =
            endian_change_u16(ibeacon_data->ibeacon_vendor.major);
        const uint16_t minor =
            endian_change_u16(ibeacon_data->ibeacon_vendor.minor);
/*
        ESP_LOGI(kTag, "iBeacon > major:0x%04x(%d) minor:0x%04x(%d) measured_power:%ddBm rssi:%ddBm", 
                 major, major, minor, minor,
                 ibeacon_data->ibeacon_vendor.measured_power,
                 param->scan_rst.rssi);
*/
        if (std::memcmp(ibeacon_data->ibeacon_vendor.proximity_uuid,
                        target_proximity_uuid_,
                        sizeof(target_proximity_uuid_)) != 0 ||
            major != target_major_id_) {
          return;
        }

        BleBeaconItem item = {.minor = minor, .rssi = param->scan_rst.rssi};

        {
          std::scoped_lock lock(beacon_items_mutex_);

          auto it = ble_beacon_items_.find(item);
          if (it != ble_beacon_items_.end()) {
            ble_beacon_items_.erase(it);
          }
          ble_beacon_items_.insert(item);
        }
      }
    }
  } else if (event == ESP_GAP_BLE_SCAN_STOP_COMPLETE_EVT) {
    esp_err_t err = param->scan_stop_cmpl.status;
    if (err != ESP_BT_STATUS_SUCCESS) {
      ESP_LOGE(kTag, "Scanning stop failed, error %s", esp_err_to_name(err));
    } else {
      ESP_LOGI(kTag, "Scanning stop successfully");
    }
  }
}

bool BeaconReceiveTask::IsIBeaconPacket(const uint8_t *const adv_data,
                                        const uint8_t adv_data_len) const {
  if (adv_data != nullptr && adv_data_len == 0x1e) {
    if (!memcmp(adv_data, reinterpret_cast<const uint8_t *>(&IBEACON_HEADER),
                sizeof(IBEACON_HEADER))) {
      return true;
    }
  }
  return false;
}

void BeaconReceiveTask::EventGapStatic(esp_gap_ble_cb_event_t event,
                                       esp_ble_gap_cb_param_t *param) {
  // Since esp_ble_gap_cb_param_t does not have UserData, pass it to instance_.
  if (instance_ != nullptr) {
    instance_->EventGap(event, param);
  }
}

}  // namespace brdf_receiver_system
