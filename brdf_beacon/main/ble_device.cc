// ESP32 BRDF Beacon
// (C)2025 bekki.jp

// Include ----------------------
#include "ble_device.h"

#include "ibeacon.h"
#include "logger.h"

namespace BrdfBeaconSystem {

static void gap_event(esp_gap_ble_cb_event_t event,
                      esp_ble_gap_cb_param_t *param) {
  BleDevice::GetInstance()->GapEvent(event, param);
}

static void gatts_event(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if,
                        esp_ble_gatts_cb_param_t *param) {
  BleDevice::GetInstance()->GattsEvent(event, gatts_if, param);
}

BleDevice *BleDevice::this_ = nullptr;

BleDevice *BleDevice::GetInstance() {
  if (this_ == nullptr) {
    this_ = new BleDevice();
  }
  return this_;
}

BleDevice::BleDevice() : services_() {}

void BleDevice::Initialize(const std::string &device_name,
                           BleIBeacon &ibeacon_adv_data) {
  // Initialize Bluetooth
  ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));

  esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();

  esp_err_t ret = esp_bt_controller_init(&bt_cfg);
  if (ret) {
    ESP_LOGE(TAG, "%s initialize controller failed: %s", __func__,
             esp_err_to_name(ret));
    return;
  }

  ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
  if (ret) {
    ESP_LOGE(TAG, "%s enable controller failed: %s", __func__,
             esp_err_to_name(ret));
    return;
  }
  esp_bluedroid_config_t bluedroid_cfg = BT_BLUEDROID_INIT_CONFIG_DEFAULT();
  ret = esp_bluedroid_init_with_cfg(&bluedroid_cfg);
  if (ret) {
    ESP_LOGE(TAG, "%s init bluetooth failed: %s", __func__,
             esp_err_to_name(ret));
    return;
  }
  ret = esp_bluedroid_enable();
  if (ret) {
    ESP_LOGE(TAG, "%s enable bluetooth failed: %s", __func__,
             esp_err_to_name(ret));
    return;
  }

  esp_err_t set_dev_name_ret = esp_ble_gap_set_device_name(device_name.c_str());
  if (set_dev_name_ret) {
    ESP_LOGE(TAG, "set device name failed, error code = %x", set_dev_name_ret);
    return;
  }

  // GAP
  ret = esp_ble_gap_register_callback(gap_event);
  if (ret) {
    ESP_LOGE(TAG, "gap register error, error code = %x", ret);
    return;
  }

  // Set iBeacon Attribute Data
  esp_ble_gap_config_adv_data_raw(
      reinterpret_cast<uint8_t *>(&ibeacon_adv_data), sizeof(ibeacon_adv_data));

  // GATT
  static constexpr uint16_t LOCAL_MTU = 500;
  esp_err_t local_mtu_ret = esp_ble_gatt_set_local_mtu(LOCAL_MTU);
  if (local_mtu_ret) {
    ESP_LOGE(TAG, "set local  MTU failed, error code = %x", local_mtu_ret);
  }

  ret = esp_ble_gatts_register_callback(gatts_event);
  if (ret) {
    ESP_LOGE(TAG, "gatts register error, error code = %x", ret);
    return;
  }
}

void BleDevice::AddService(BleServiceInterfaceSharedPtr bleService) {
  services_.push_back(bleService);

  esp_err_t ret = esp_ble_gatts_app_register(bleService->GetAppId());
  if (ret) {
    ESP_LOGE(TAG, "gatts app register error, error code = %x", ret);
    return;
  }
}

void BleDevice::GapEvent(esp_gap_ble_cb_event_t event,
                         esp_ble_gap_cb_param_t *param) {
  if (event == ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT) {
  } else if (event == ESP_GAP_BLE_ADV_START_COMPLETE_EVT) {
    // advertising start complete event to indicate advertising start
    // successfully or failed
    if (param->adv_start_cmpl.status != ESP_BT_STATUS_SUCCESS) {
      ESP_LOGE(TAG, "Advertising start failed");
    }
  } else if (event == ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT) {
    if (param->adv_stop_cmpl.status != ESP_BT_STATUS_SUCCESS) {
      ESP_LOGE(TAG, "Advertising stop failed");
    } else {
      ESP_LOGI(TAG, "Stop adv successfully");
    }
  } else if (event == ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT) {
    ESP_LOGI(
        TAG,
        "update connection params status = %d, min_int = %d, max_int = "
        "%d,conn_int = %d,latency = %d, timeout = %d",
        param->update_conn_params.status, param->update_conn_params.min_int,
        param->update_conn_params.max_int, param->update_conn_params.conn_int,
        param->update_conn_params.latency, param->update_conn_params.timeout);
  } else {
    ESP_LOGI(TAG, "GAP Event %d", event);
  }
}

void BleDevice::GattsEvent(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if,
                           esp_ble_gatts_cb_param_t *param) {
  if (event == ESP_GATTS_REG_EVT) {
    if (param->reg.status == ESP_GATT_OK) {
      for (BleServiceInterfaceSharedPtr bleService : services_) {
        if (bleService->GetAppId() == param->reg.app_id) {
          bleService->SetGattsIf(gatts_if);
        }
      }
    } else {
      ESP_LOGI(TAG, "Reg app failed, app_id %04x, status %d", param->reg.app_id,
               param->reg.status);
      return;
    }
  }

  for (BleServiceInterfaceSharedPtr bleService : services_) {
    if (gatts_if == ESP_GATT_IF_NONE || gatts_if == bleService->GetGattsIf()) {
      bleService->GattsEvent(event, gatts_if, param);
    }
  }
}

void BleDevice::StartAdvertising() {
  // iBeacon Adv interval is 100ms (160(0xA0) * 0.625ms = 100ms)
  esp_ble_adv_params_t adv_params = {
      .adv_int_min = 0x00A0,  // Minimum Advertising Interval (range 0x0020 -
                              // 0x4000) N * 0.625ms
      .adv_int_max = 0x00A0,  // Maximum Advertising Interval (range 0x0020 -
                              // 0x4000) N * 0.625ms
      .adv_type = ADV_TYPE_IND,
      .own_addr_type = BLE_ADDR_TYPE_PUBLIC,
      .peer_addr = {},
      .peer_addr_type = BLE_ADDR_TYPE_PUBLIC,
      .channel_map = ADV_CHNL_ALL,
      .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
  };
  esp_ble_gap_start_advertising(&adv_params);
}

}  // namespace BrdfBeaconSystem
