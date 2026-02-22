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

// NimBLE Includes
#include "host/ble_hs.h"
#include "host/util/util.h"
#include "services/gatt/ble_svc_gatt.h"

namespace bfox_beacon_system {

// Deep Sleep command code
constexpr uint8_t kDeepSleepCommand = 0x01;

// NimBLE UUID Definitions (Little Endian arrays as provided in original code)
static const ble_uuid128_t gatt_svr_svc_bfox_uuid =
    BLE_UUID128_INIT(0xDF, 0xF0, 0xC0, 0xD8, 0x86, 0x18, 0xA7, 0xB0, 0xA0, 0x4E, 0x31, 0x91, 0x7C, 0xD6, 0x7F, 0x34);

static const ble_uuid128_t gatt_svr_chr_voltage_uuid =
    BLE_UUID128_INIT(0x05, 0x09, 0x77, 0xF0, 0xB7, 0x4F, 0x75, 0xB6, 0xA3, 0x46, 0xBA, 0x41, 0x46, 0x4A, 0xBF, 0x53);

static const ble_uuid128_t gatt_svr_chr_setting_uuid =
    BLE_UUID128_INIT(0x20, 0x02, 0xF7, 0x34, 0x0C, 0x8D, 0x83, 0xA4, 0x99, 0x4C, 0x35, 0x1B, 0xD5, 0x09, 0x6A, 0x09);

static const ble_uuid128_t gatt_svr_chr_sleep_uuid =
    BLE_UUID128_INIT(0xC1, 0x88, 0x0D, 0xA5, 0xBC, 0xBB, 0x30, 0x9A, 0x18, 0x4C, 0x0E, 0x65, 0x7E, 0x6A, 0xF2, 0x0C);

BleVoltageCharacteristic::BleVoltageCharacteristic(
    const BFoxBeaconInterfaceWeakPtr bfox_beacon_interface)
    : bfox_beacon_interface_(bfox_beacon_interface) {}

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

BleBeaconSettingCharacteristic::BleBeaconSettingCharacteristic(
    const BFoxBeaconInterfaceWeakPtr bfox_beacon_interface)
    : bfox_beacon_interface_(bfox_beacon_interface) {}

void BleBeaconSettingCharacteristic::Write(
    const std::vector<uint8_t>* const data) {
  if (data->size() <= 0) {
    return;
  }

  // [1:device_name_length][x:device_name][2:major][2:minor][2:measured_power][2:tx_power][2:adv_interval_ms]
  const uint8_t device_name_length = *data->data();
  if ((device_name_length + 11) != data->size()) {
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
  const uint16_t adv_interval_ms = *reinterpret_cast<const uint16_t*>(
      data->data() + 1 + device_name_length + 8);

  ESP_LOGI(TAG, "Write Beacon Setting");
  ESP_LOGI(TAG, " Name:%s", device_name.c_str());
  ESP_LOGI(TAG, " Major:%u", major);
  ESP_LOGI(TAG, " Minor:%u", minor);
  ESP_LOGI(TAG, " MesuredPower:%d", measured_power);
  ESP_LOGI(TAG, " TxPower:%d", tx_power);
  ESP_LOGI(TAG, " AdvIntervalMs:%u", adv_interval_ms);

  // Set to BeaconSetting
  BeaconSetting setting;
  setting.SetDeviceName(device_name);
  setting.SetMajor(major);
  setting.SetMinor(minor);
  setting.SetMeasuredPower(measured_power);
  setting.SetTxPower(tx_power);
  setting.SetAdvIntervalMs(adv_interval_ms);
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

  // [1:device_name_length][x:device_name][2:major][2:minor][2:measured_power][2:tx_power][2:adv_interval_ms]
  const uint8_t device_name_length = setting->GetDeviceName().length();
  std::vector<uint8_t> payload(device_name_length + 11);

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
  *reinterpret_cast<uint16_t*>(payload.data() + 1 + device_name_length + 8) =
      setting->GetAdvIntervalMs();

  data->insert(data->end(), payload.begin(), payload.end());
}

BleDeepSleepCharacteristic::BleDeepSleepCharacteristic(
    const BFoxBeaconInterfaceWeakPtr bfox_beacon_interface)
    : bfox_beacon_interface_(bfox_beacon_interface) {}

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

// NimBLE static instance pointer for callback access
static BleBFoxService* g_ble_bfox_service_inst = nullptr;

BleBFoxService::BleBFoxService(
    const BFoxBeaconInterfaceWeakPtr bfox_beacon_interface)
    : voltage_char_(std::make_shared<BleVoltageCharacteristic>(bfox_beacon_interface)),
      setting_char_(std::make_shared<BleBeaconSettingCharacteristic>(bfox_beacon_interface)),
      sleep_char_(std::make_shared<BleDeepSleepCharacteristic>(bfox_beacon_interface)) {
  g_ble_bfox_service_inst = this;
}

int BleBFoxService::GattSvrChrAccessStatic(uint16_t conn_handle, uint16_t attr_handle,
                                           struct ble_gatt_access_ctxt* ctxt,
                                           void* arg) {
  if (g_ble_bfox_service_inst) {
    return g_ble_bfox_service_inst->GattSvrChrAccess(conn_handle, attr_handle, ctxt, arg);
  }
  return BLE_ATT_ERR_UNLIKELY;
}

int BleBFoxService::GattSvrChrAccess(uint16_t conn_handle, uint16_t attr_handle,
                                     struct ble_gatt_access_ctxt* ctxt, void* arg) {
  const ble_uuid_t* uuid = ctxt->chr->uuid;

  if (ble_uuid_cmp(uuid, &gatt_svr_chr_voltage_uuid.u) == 0) {
    if (ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR) {
      std::vector<uint8_t> data;
      voltage_char_->Read(&data);
      int rc = os_mbuf_append(ctxt->om, data.data(), data.size());
      return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
    }
  } else if (ble_uuid_cmp(uuid, &gatt_svr_chr_setting_uuid.u) == 0) {
    if (ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR) {
      std::vector<uint8_t> data;
      setting_char_->Read(&data);
      int rc = os_mbuf_append(ctxt->om, data.data(), data.size());
      return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
    } else if (ctxt->op == BLE_GATT_ACCESS_OP_WRITE_CHR) {
      uint16_t len = OS_MBUF_PKTLEN(ctxt->om);
      std::vector<uint8_t> data(len);
      int rc = ble_hs_mbuf_to_flat(ctxt->om, data.data(), len, NULL);
      if (rc == 0) {
        setting_char_->Write(&data);
      }
      return rc == 0 ? 0 : BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
    }
  } else if (ble_uuid_cmp(uuid, &gatt_svr_chr_sleep_uuid.u) == 0) {
    if (ctxt->op == BLE_GATT_ACCESS_OP_WRITE_CHR) {
      uint16_t len = OS_MBUF_PKTLEN(ctxt->om);
      std::vector<uint8_t> data(len);
      int rc = ble_hs_mbuf_to_flat(ctxt->om, data.data(), len, NULL);
      if (rc == 0) {
        sleep_char_->Write(&data);
      }
      return rc == 0 ? 0 : BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
    }
  }

  return BLE_ATT_ERR_UNLIKELY;
}

// NimBLE GATT service definition array
// Must be static to persist throughout NimBLE's lifecycle
static const struct ble_gatt_svc_def gatt_svr_svcs[] = {
    {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = &gatt_svr_svc_bfox_uuid.u,
        .characteristics = (struct ble_gatt_chr_def[]) {
            {
                // Voltage Char
                .uuid = &gatt_svr_chr_voltage_uuid.u,
                .access_cb = BleBFoxService::GattSvrChrAccessStatic,
                .flags = BLE_GATT_CHR_F_READ,
            },
            {
                // Setting Char
                .uuid = &gatt_svr_chr_setting_uuid.u,
                .access_cb = BleBFoxService::GattSvrChrAccessStatic,
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE,
            },
            {
                // Deep Sleep Char
                .uuid = &gatt_svr_chr_sleep_uuid.u,
                .access_cb = BleBFoxService::GattSvrChrAccessStatic,
                .flags = BLE_GATT_CHR_F_WRITE,
            },
            {
                0, // No more characteristics in this service
            }
        },
    },
    {
        0, // No more services
    },
};

const struct ble_gatt_svc_def* BleBFoxService::GetServiceDefs() {
  return gatt_svr_svcs;
}

}  // namespace bfox_beacon_system
