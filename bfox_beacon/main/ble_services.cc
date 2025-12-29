// ESP32 B-Fox Beacon
// (C)2025 bekki.jp

#include "ble_services.h"

#include <esp_sleep.h>
#include <esp_system.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <algorithm>
#include <cstring>

#include "beacon_setting.h"
#include "bfox_beacon.h"
#include "gpio_control.h"
#include "logger.h"
#include "util.h"

namespace bfox_beacon_system {

// Deep Sleep command code
constexpr uint8_t kDeepSleepCommand = 0x01;

BleVoltageCharacteristic::BleVoltageCharacteristic(
    esp_bt_uuid_t characteristic_uuid, esp_gatt_char_prop_t property,
    const BFoxBeaconInterfaceWeakPtr bfox_beacon_interface)
    : BleCharacteristicInterface(),
      characteristic_uuid_(characteristic_uuid),
      property_(property),
      bfox_beacon_interface_(bfox_beacon_interface) {}

void BleVoltageCharacteristic::Read(std::vector<uint8_t>* const data) {
  BFoxBeaconInterfaceSharedPtr bfox_beacon = bfox_beacon_interface_.lock();
  if (!bfox_beacon) {
    return;
  }

  // int16_t  LittleEndian
  std::vector<uint8_t> payload(2);
  int16_t* const view = reinterpret_cast<int16_t*>(payload.data());
  *view = static_cast<int16_t>(bfox_beacon->GetBatteryVoltage() * 100);
  data->insert(data->begin(), payload.begin(), payload.end());
}

void BleVoltageCharacteristic::SetHandle(const uint16_t handle) {
  handle_ = handle;
}

uint16_t BleVoltageCharacteristic::GetHandle() const { return handle_; }

esp_bt_uuid_t BleVoltageCharacteristic::GetUuid() const {
  return characteristic_uuid_;
}

esp_gatt_char_prop_t BleVoltageCharacteristic::GetProperty() const {
  return property_;
}

BleBeaconSettingCharacteristic::BleBeaconSettingCharacteristic(
    esp_bt_uuid_t characteristic_uuid, esp_gatt_char_prop_t property,
    const BFoxBeaconInterfaceWeakPtr bfox_beacon_interface)
    : BleCharacteristicInterface(),
      characteristic_uuid_(characteristic_uuid),
      property_(property),
      bfox_beacon_interface_(bfox_beacon_interface) {}

void BleBeaconSettingCharacteristic::Write(
    const std::vector<uint8_t>* const data) {
  // esp_log_buffer_hex(TAG, data->data(), data->size());
  if (data->size() <= 0) {
    return;
  }

  // [1:device_name_length][x:device_name][2:major][2:minor][2:measured_power][2:tx_power]
  const uint8_t device_name_length = *data->data();
  if ((device_name_length + 9) != data->size()) {
    ESP_LOGE(TAG, "BleBeaconSettingCharacteristic Invalid Data Length");
    return;
  }

  const std::string device_name(data->data() + 1,
                                data->data() + 1 + device_name_length);
  const uint16_t major =
      *reinterpret_cast<const uint16_t*>(data->data() + 1 + device_name_length);
  const uint16_t minor = *reinterpret_cast<const uint16_t*>(
      data->data() + 1 + device_name_length + 2);
  const int16_t measured_power = *reinterpret_cast<const int16_t*>(
      data->data() + 1 + device_name_length + 4);
  const int16_t tx_power = *reinterpret_cast<const int16_t*>(
      data->data() + 1 + device_name_length + 6);

  ESP_LOGI(TAG, "Write Beacon Setting");
  ESP_LOGI(TAG, " Name:%s", device_name.c_str());
  ESP_LOGI(TAG, " Major:%u", major);
  ESP_LOGI(TAG, " Minor:%u", minor);
  ESP_LOGI(TAG, " MesuredPower:%d", measured_power);
  ESP_LOGI(TAG, " TxPower:%d", tx_power);

  // Set to BeaconSetting
  BeaconSetting setting;
  setting.SetDeviceName(device_name);
  setting.SetMajor(major);
  setting.SetMinor(minor);
  setting.SetMeasuredPower(measured_power);
  setting.SetTxPower(tx_power);
  setting.Save();

  // Restart after 3 seconds
  xTaskCreate(
      // Lambda expression with no capture
      [](void* pvParameter) {
        ESP_LOGI(TAG, "Starting restart task. Waiting for 3 seconds...");
        vTaskDelay(pdMS_TO_TICKS(3000));
        ESP_LOGI(TAG, "Restarting now!");
        esp_restart();  // Restart the system
      },
      "restart_task", 2048, nullptr, 1, nullptr);
}

void BleBeaconSettingCharacteristic::Read(std::vector<uint8_t>* const data) {
  BFoxBeaconInterfaceSharedPtr bfox_beacon = bfox_beacon_interface_.lock();
  if (!bfox_beacon) {
    return;
  }

  BeaconSettingConstSharedPtr setting = bfox_beacon->GetSetting().lock();
  if (!setting) {
    return;
  }

  // [1:device_name_length][x:device_name][2:major][2:minor][2:measured_power][2:tx_power]
  const uint8_t device_name_length = setting->GetDeviceName().length();
  std::vector<uint8_t> payload(device_name_length + 9);

  *reinterpret_cast<uint8_t*>(payload.data()) = device_name_length;
  std::memcpy(reinterpret_cast<uint8_t*>(payload.data() + 1),
              setting->GetDeviceName().c_str(), device_name_length);
  *reinterpret_cast<uint16_t*>(payload.data() + 1 + device_name_length) =
      setting->GetMajor();
  *reinterpret_cast<uint16_t*>(payload.data() + 1 + device_name_length + 2) =
      setting->GetMinor();
  *reinterpret_cast<int16_t*>(payload.data() + 1 + device_name_length + 4) =
      setting->GetMeasuredPower();
  *reinterpret_cast<int16_t*>(payload.data() + 1 + device_name_length + 6) =
      setting->GetTxPower();

  data->insert(data->end(), payload.begin(), payload.end());
}

void BleBeaconSettingCharacteristic::SetHandle(const uint16_t handle) {
  handle_ = handle;
}

uint16_t BleBeaconSettingCharacteristic::GetHandle() const { return handle_; }

esp_bt_uuid_t BleBeaconSettingCharacteristic::GetUuid() const {
  return characteristic_uuid_;
}

esp_gatt_char_prop_t BleBeaconSettingCharacteristic::GetProperty() const {
  return property_;
}

BleDeepSleepCharacteristic::BleDeepSleepCharacteristic(
    esp_bt_uuid_t characteristic_uuid, esp_gatt_char_prop_t property,
    const BFoxBeaconInterfaceWeakPtr bfox_beacon_interface)
    : BleCharacteristicInterface(),
      characteristic_uuid_(characteristic_uuid),
      property_(property),
      bfox_beacon_interface_(bfox_beacon_interface) {}

void BleDeepSleepCharacteristic::Write(const std::vector<uint8_t>* const data) {
  if (data->size() <= 0) {
    return;
  }

  // Check for Deep Sleep command
  const uint8_t command = *data->data();
  if (command == kDeepSleepCommand) {
    ESP_LOGI(TAG, "Deep Sleep command received. Entering Deep Sleep mode...");

    // Turn off monitoring LED before entering Deep Sleep
    gpio::SetLevel(kMonitoringLedPin, false);

    // Enter Deep Sleep immediately
    esp_deep_sleep_start();
  } else {
    ESP_LOGW(TAG, "Unknown Deep Sleep command: 0x%02x", command);
  }
}

void BleDeepSleepCharacteristic::SetHandle(const uint16_t handle) {
  handle_ = handle;
}

uint16_t BleDeepSleepCharacteristic::GetHandle() const { return handle_; }

esp_bt_uuid_t BleDeepSleepCharacteristic::GetUuid() const {
  return characteristic_uuid_;
}

esp_gatt_char_prop_t BleDeepSleepCharacteristic::GetProperty() const {
  return property_;
}

BleBFoxService::BleBFoxService(const uint16_t app_id,
                               esp_bt_uuid_t service_uuid,
                               const uint16_t handle_num)
    : BleServiceInterface(),
      app_id_(app_id),
      gatts_if_(0),
      gatts_handle_num_(
          handle_num)  // Number of required handles (number of attributes)
      ,
      service_uuid_(service_uuid),
      characteristics_() {}

void BleBFoxService::GattsEvent(esp_gatts_cb_event_t event,
                                esp_gatt_if_t gatts_if,
                                esp_ble_gatts_cb_param_t* param) {
  if (event == ESP_GATTS_REG_EVT) {
    ESP_LOGI(TAG, "REGISTER_APP_EVT, status %d, app_id %d", param->reg.status,
             param->reg.app_id);

    esp_gatt_srvc_id_t service_id = {
        .id = {.uuid = service_uuid_, .inst_id = 0x00}, .is_primary = true};
    esp_ble_gatts_create_service(gatts_if, &service_id, gatts_handle_num_);

  } else if (event == ESP_GATTS_READ_EVT) {
    ESP_LOGI(TAG, "GATT_READ_EVT, conn_id %d, trans_id %" PRIu32 ", handle %d",
             param->read.conn_id, param->read.trans_id, param->read.handle);

    for (const auto& bleCharacteristic : characteristics_) {
      if (bleCharacteristic->GetHandle() == param->read.handle) {
        std::vector<uint8_t> data;
        bleCharacteristic->Read(&data);

        esp_gatt_rsp_t rsp = {
            .attr_value = {.value = {},
                           .handle = param->read.handle,
                           .offset = 0,
                           .len = static_cast<uint16_t>(data.size()),
                           .auth_req = 0}};
        std::memcpy(rsp.attr_value.value, data.data(), data.size());
        esp_ble_gatts_send_response(gatts_if, param->read.conn_id,
                                    param->read.trans_id, ESP_GATT_OK, &rsp);
      }
    }

  } else if (event == ESP_GATTS_WRITE_EVT) {
    ESP_LOGI(TAG,
             "GATT_WRITE_EVT, conn_id %d, trans_id %" PRIu32
             ", handle %d, need_resp %d",
             param->write.conn_id, param->write.trans_id, param->write.handle,
             param->write.need_rsp ? 1 : 0);

    for (const auto& bleCharacteristic : characteristics_) {
      if (bleCharacteristic->GetHandle() == param->write.handle) {
        std::vector<uint8_t> data(param->write.value,
                                  param->write.value + param->write.len);
        bleCharacteristic->Write(&data);
      }
    }

    if (param->write.need_rsp) {
      esp_ble_gatts_send_response(gatts_if, param->write.conn_id,
                                  param->write.trans_id, ESP_GATT_OK, nullptr);
    }

  } else if (event == ESP_GATTS_MTU_EVT) {
    ESP_LOGI(TAG, "ESP_GATTS_MTU_EVT, MTU %d", param->mtu.mtu);

  } else if (event == ESP_GATTS_CREATE_EVT) {
    ESP_LOGI(TAG, "CREATE_SERVICE_EVT, status %d,  service_handle %d",
             param->create.status, param->create.service_handle);
    const uint16_t service_handle = param->create.service_handle;
    esp_ble_gatts_start_service(service_handle);

    for (const auto& bleCharacteristic : characteristics_) {
      esp_bt_uuid_t uuid = bleCharacteristic->GetUuid();
      esp_err_t add_char_ret = esp_ble_gatts_add_char(
          service_handle, &uuid, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
          bleCharacteristic->GetProperty(), nullptr, nullptr);
      if (add_char_ret) {
        ESP_LOGE(TAG, "add char failed, error code =%x", add_char_ret);
      }
    }

  } else if (event == ESP_GATTS_ADD_CHAR_EVT) {
    ESP_LOGI(TAG, "GATTS_ADD_CHAR_EVT, status %d, service_handle %d",
             param->start.status, param->start.service_handle);
    if (param->start.status != 0) {
      ESP_LOGE(TAG, "Failed Add Char Event");
      return;
    }

    for (const auto& bleCharacteristic : characteristics_) {
      esp_bt_uuid_t uuid = bleCharacteristic->GetUuid();
      if (std::memcmp(&uuid, &param->add_char.char_uuid,
                      sizeof(param->add_char.char_uuid)) == 0) {
        ESP_LOGI(TAG, "Set Characteristic Handle, handle:%d",
                 param->start.service_handle);
        bleCharacteristic->SetHandle(param->start.service_handle);
      }
    }

  } else if (event == ESP_GATTS_START_EVT) {
    ESP_LOGI(TAG, "SERVICE_START_EVT, status %d, service_handle %d",
             param->start.status, param->start.service_handle);

  } else if (event == ESP_GATTS_CONNECT_EVT) {
    ESP_LOGI(TAG,
             "ESP_GATTS_CONNECT_EVT, conn_id %d, remote "
             "%02x:%02x:%02x:%02x:%02x:%02x:",
             param->connect.conn_id, param->connect.remote_bda[0],
             param->connect.remote_bda[1], param->connect.remote_bda[2],
             param->connect.remote_bda[3], param->connect.remote_bda[4],
             param->connect.remote_bda[5]);

    esp_ble_conn_update_params_t conn_params = {
        .bda = {},        // Bluetooth device address
        .min_int = 0x10,  // Min connection interval 0x10 * 1.25ms = 20ms
        .max_int = 0x20,  // Max connection interval 0x20 * 1.25ms = 40ms
        .latency = 0,     // Slave latency for the connection in number of
                          // connection events. Range: 0x0000 to 0x01F3
        .timeout = 1000,  // Supervision timeout for the LE Link. (Range:0 -
                          // 3200) (X * 10msec) 1000 = 10s
    };
    std::memcpy(conn_params.bda, param->connect.remote_bda,
                sizeof(esp_bd_addr_t));
    esp_ble_gap_update_conn_params(&conn_params);

  } else if (event == ESP_GATTS_DISCONNECT_EVT) {
    ESP_LOGI(TAG, "ESP_GATTS_DISCONNECT_EVT, disconnect reason 0x%x",
             param->disconnect.reason);
    BleDevice::GetInstance()->StartAdvertising();

  } else if (event == ESP_GATTS_CONF_EVT) {
    ESP_LOGI(TAG, "ESP_GATTS_CONF_EVT, status %d attr_handle %d",
             param->conf.status, param->conf.handle);
    if (param->conf.status != ESP_GATT_OK) {
      // esp_log_buffer_hex(TAG, param->conf.value, param->conf.len);
    }

  } else if (event == ESP_GATTS_RESPONSE_EVT) {
    // ESP_LOGI(TAG, "GATTS_RESPONSE_EVT, status %d service_handle %d",
    // param->conf.status, param->start.service_handle);
  } else {
    ESP_LOGI(TAG, "EVENT %d, status %d, service_handle %d", event,
             param->start.status, param->start.service_handle);
  }
}

void BleBFoxService::AddCharacteristic(
    BleCharacteristicInterfaceSharedPtr bleCharacteristic) {
  characteristics_.push_back(bleCharacteristic);
}

void BleBFoxService::SetGattsIf(const uint16_t gatts_if) {
  gatts_if_ = gatts_if;
}

uint16_t BleBFoxService::GetAppId() const { return app_id_; }

uint16_t BleBFoxService::GetGattsIf() const { return gatts_if_; }

}  // namespace bfox_beacon_system
