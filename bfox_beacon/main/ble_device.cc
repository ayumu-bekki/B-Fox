// ESP32 B-Fox Beacon
// (C)2025 bekki.jp

#include "ble_device.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "logger.h"

// NimBLE Includes
#include "host/ble_hs.h"
#include "host/util/util.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"

namespace bfox_beacon_system {

BleDevice* BleDevice::this_ = nullptr;

BleDevice* BleDevice::GetInstance() {
  if (this_ == nullptr) {
    this_ = new BleDevice();
  }
  return this_;
}

BleDevice::BleDevice() : adv_interval_ms_(100) {}

void BleDevice::Initialize(const std::string& device_name,
                           BleIBeacon& ibeacon_adv_data) {
  device_name_ = device_name;
  ibeacon_adv_data_ = ibeacon_adv_data;

  // Initialize NimBLE port
  int rc = nimble_port_init();
  if (rc != 0) {
    ESP_LOGE(TAG, "nimble_port_init failed: %d", rc);
    return;
  }

  // Setup Host Callbacks
  ble_hs_cfg.reset_cb = OnResetStatic;
  ble_hs_cfg.sync_cb = OnSyncStatic;
  ble_hs_cfg.store_status_cb = ble_store_util_status_rr;

  // Initialize gap and gatt
  ble_svc_gap_init();
  ble_svc_gatt_init();

  rc = ble_svc_gap_device_name_set(device_name_.c_str());
  if (rc != 0) {
    ESP_LOGE(TAG, "failed to set device name: %d", rc);
  }
}

void BleDevice::RegisterServices(const struct ble_gatt_svc_def* svcs) {
  int rc = ble_gatts_count_cfg(svcs);
  if (rc != 0) {
    ESP_LOGE(TAG, "ble_gatts_count_cfg failed: %d", rc);
    return;
  }

  rc = ble_gatts_add_svcs(svcs);
  if (rc != 0) {
    ESP_LOGE(TAG, "ble_gatts_add_svcs failed: %d", rc);
    return;
  }
}

void BleDevice::StartHost() {
  nimble_port_freertos_init(HostTaskStatic);
}

void BleDevice::HostTaskStatic(void* param) {
  BleDevice::GetInstance()->HostTask();
}

void BleDevice::HostTask() {
  ESP_LOGI(TAG, "NimBLE host task started");
  nimble_port_run();
  nimble_port_freertos_deinit();
}

void BleDevice::OnSyncStatic() {
  BleDevice::GetInstance()->OnSync();
}

void BleDevice::OnSync() {
  ESP_LOGI(TAG, "NimBLE host synced");

  // We start advertising after sync
  RestartAdvertising();
}

void BleDevice::OnResetStatic(int reason) {
  BleDevice::GetInstance()->OnReset(reason);
}

void BleDevice::OnReset(int reason) {
  ESP_LOGE(TAG, "NimBLE host reset, reason: %d", reason);
}

int BleDevice::GapEventStatic(struct ble_gap_event* event, void* arg) {
  return BleDevice::GetInstance()->GapEvent(event, arg);
}

int BleDevice::GapEvent(struct ble_gap_event* event, void* arg) {
  switch (event->type) {
    case BLE_GAP_EVENT_CONNECT:
      ESP_LOGI(TAG, "BLE_GAP_EVENT_CONNECT status: %d",
               event->connect.status);
      // Connection state changed. Restart advertising to keep iBeacon alive
      // while connected.
      RestartAdvertising();
      break;

    case BLE_GAP_EVENT_DISCONNECT:
      ESP_LOGI(TAG, "BLE_GAP_EVENT_DISCONNECT reason: %d",
               event->disconnect.reason);
      RestartAdvertising();
      break;

    case BLE_GAP_EVENT_ADV_COMPLETE:
      ESP_LOGI(TAG, "BLE_GAP_EVENT_ADV_COMPLETE reason: %d",
               event->adv_complete.reason);
      RestartAdvertising();
      break;

    case BLE_GAP_EVENT_SUBSCRIBE:
      ESP_LOGI(TAG, "BLE_GAP_EVENT_SUBSCRIBE conn_handle=%d",
               event->subscribe.conn_handle);
      break;

    case BLE_GAP_EVENT_MTU:
      ESP_LOGI(TAG, "BLE_GAP_EVENT_MTU conn_handle=%d mtu=%d",
               event->mtu.conn_handle, event->mtu.value);
      break;
  }
  return 0;
}

void BleDevice::StartAdvertising(uint16_t interval_ms) {
  adv_interval_ms_ = interval_ms;

  // Use the raw iBeacon structure directly, matching Bluedroid behavior exactly
  int rc = ble_gap_adv_set_data(reinterpret_cast<uint8_t*>(&ibeacon_adv_data_),
                                sizeof(ibeacon_adv_data_));
  if (rc != 0) {
    ESP_LOGE(TAG, "ble_gap_adv_set_data failed: %d", rc);
    return;
  }

  // Clear scan response data to prevent any interference with iBeacon strictness
  ble_gap_adv_rsp_set_data(nullptr, 0);

  // Begin advertising
  struct ble_gap_adv_params adv_params;
  memset(&adv_params, 0, sizeof(adv_params));
  adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
  adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;
  
  // interval_ms unit is 0.625ms (n * 0.625 = interval_ms) => n = interval_ms * 1.6
  uint16_t n = static_cast<uint16_t>(interval_ms * 1.6f);
  adv_params.itvl_min = n;
  adv_params.itvl_max = n;

  // Check if own address is available before starting
  uint8_t own_addr_type;
  rc = ble_hs_id_infer_auto(0, &own_addr_type);
  if (rc != 0) {
    ESP_LOGE(TAG, "ble_hs_id_infer_auto failed: %d", rc);
    return;
  }

  rc = ble_gap_adv_start(own_addr_type, NULL, BLE_HS_FOREVER,
                         &adv_params, GapEventStatic, NULL);
  if (rc != 0) {
    if (rc == BLE_HS_EALREADY) {
      // Already advertising, no need to log as error
      return;
    }
    // If connectable advertising fails (e.g. max connections reached), fallback to non-connectable
    ESP_LOGW(TAG, "Connectable adv failed (%d). Fallback to non-connectable.", rc);
    adv_params.conn_mode = BLE_GAP_CONN_MODE_NON;
    adv_params.disc_mode = BLE_GAP_DISC_MODE_NON;
    rc = ble_gap_adv_start(own_addr_type, NULL, BLE_HS_FOREVER,
                           &adv_params, GapEventStatic, NULL);
    if (rc != 0) {
      ESP_LOGE(TAG, "ble_gap_adv_start fallback failed: %d", rc);
      return;
    }
  }
  
  ESP_LOGI(TAG, "Started NimBLE advertising");
}

void BleDevice::RestartAdvertising() {
  StartAdvertising(adv_interval_ms_);
}

}  // namespace bfox_beacon_system
