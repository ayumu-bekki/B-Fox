// ESP32 BRDF Beacon
// (C)2025 bekki.jp

// Include ----------------------
#include "brdf_beacon.h"

#include <driver/gpio.h>
#include <driver/rtc_io.h>
#include <esp_adc/adc_cali.h>
#include <esp_adc/adc_cali_scheme.h>
#include <esp_gap_ble_api.h>
#include <esp_sleep.h>
#include <esp_system.h>
#include <nvs_flash.h>

#include <cstring>
#include <memory>

#include "beacon_setting.h"
#include "ble_device.h"
#include "ble_services.h"
#include "file_system.h"
#include "gpio_control.h"
#include "ibeacon.h"
#include "logger.h"
#include "util.h"
#include "version.h"
#include "xiao_esp32c6_pin.h"

namespace BrdfBeaconSystem {

// PROXIMITY_UUID (UUID for BRDF Receiver), for iBeacon data, 128-bit, Big Endian
constexpr uint8_t BRDF_IBEACON_PROXIMITY_UUID[ESP_UUID_LEN_128] = {
    0xC6, 0x5B, 0x2C, 0x5D,             // C65B2C5D
    0x9E, 0x53,                         // 9E53
    0x46, 0xEC,                         // 46EC
    0x8B, 0x8E,                         // 8B8E
    0x54, 0xD9, 0xE2, 0xF2, 0x11, 0x88  // 54D9E2F21188
};

BrdfBeacon::BrdfBeacon() : voltage_check_task_(), setting_() {}

BrdfBeacon::~BrdfBeacon() = default;

void BrdfBeacon::Start() {
  // Initialize Log
  Logger::InitializeLogLevel();

  ESP_LOGI(TAG, "Startup BRDF Beacon. Version:%s",
           std::string(GIT_VERSION).c_str());

  // Monitoring LED Init
  GPIO::InitOutput(static_cast<gpio_num_t>(CONFIG_MONITORING_OUTPUT_GPIO_NO),
                   false);

  // Battery voltage monitoring
  voltage_check_task_ = std::make_unique<VoltageCheckTask>();
  if (!voltage_check_task_) {
    esp_deep_sleep_start();
  }
  voltage_check_task_->Start();

  // Use Ext Antenna
  GPIO::InitOutput(XIAO_ESP32_PIN::WIFI_ENABLE, false);
  Util::SleepMillisecond(100);
  GPIO::InitOutput(XIAO_ESP32_PIN::WIFI_ANT_CONFIG, true);

  // Initialize NVS
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
      ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_LOGE(TAG, "NVS Flash Error");
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  // Mount File System
  FileSystem::Mount();

  // Load Setting
  setting_ = std::make_shared<BeaconSetting>();
  setting_->Load();

  ESP_LOGI(TAG, "Start BLE GATT name:%s major:%d minor:%d",
           setting_->GetDeviceName().c_str(), setting_->GetMajor(),
           setting_->GetMinor());

  // Bluetooth GATT Server
  CreateBLEService();

  ESP_LOGI(TAG, "Activation Complete BRDF Beacon System.");
  GPIO::SetLevel(static_cast<gpio_num_t>(CONFIG_MONITORING_OUTPUT_GPIO_NO),
                 true);

  while (true) {
    Util::SleepMillisecond(1000);
  }
}

void BrdfBeacon::CreateBLEService() {
  // BLE Set Coded Phy
  esp_err_t res = esp_ble_gap_set_preferred_default_phy(
      ESP_BLE_GAP_PHY_CODED_PREF_MASK, ESP_BLE_GAP_PHY_CODED_PREF_MASK);
  if (res == ESP_OK) {
    ESP_LOGI(TAG, "BLE Coded Phy Enable");
  } else {
    ESP_LOGI(TAG, "BLE Coded Phy Disable");
  }

  // Create BleVoltageCharacteristic : 53bf4a46-41ba-46a3-b675-4fb7f0770905
  constexpr esp_gatt_char_prop_t voltage_char_property =
      ESP_GATT_CHAR_PROP_BIT_READ;
  constexpr uint8_t TIME_CHARACTERISTIC_UUID_RAW[ESP_UUID_LEN_128] = { // 128bit Little Endian
      0x05, 0x09, 0x77, 0xF0, 0xB7, 0x4F, 0x75, 0xB6,
      0xA3, 0x46, 0xBA, 0x41, 0x46, 0x4A, 0xBF, 0x53};
  esp_bt_uuid_t voltage_characteristic_uuid = {.len = ESP_UUID_LEN_128,
                                               .uuid = {.uuid128 = {}}};
  std::memcpy(voltage_characteristic_uuid.uuid.uuid128,
              TIME_CHARACTERISTIC_UUID_RAW, ESP_UUID_LEN_128);
  BleCharacteristicInterfaceSharedPtr ble_voltage_characteristic =
      std::make_shared<BleVoltageCharacteristic>(
          voltage_characteristic_uuid, voltage_char_property, weak_from_this());

  // Create BleBeaconSettingCharacteristic
  constexpr esp_gatt_char_prop_t beacon_setting_char_property =
      ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_WRITE;
  // 096a09d5-1b35-4c99-a483-8d0c34f70220
  constexpr uint8_t DEMO_CHARACTERISTIC_UUID_RAW[ESP_UUID_LEN_128] = { // 128bit Little Endian
      0x20, 0x02, 0xF7, 0x34, 0x0C, 0x8D, 0x83, 0xA4,
      0x99, 0x4C, 0x35, 0x1B, 0xD5, 0x09, 0x6A, 0x09};
  esp_bt_uuid_t beacon_setting_characteristic_uuid = {.len = ESP_UUID_LEN_128,
                                                      .uuid = {.uuid128 = {}}};
  std::memcpy(beacon_setting_characteristic_uuid.uuid.uuid128,
              DEMO_CHARACTERISTIC_UUID_RAW, ESP_UUID_LEN_128);
  BleCharacteristicInterfaceSharedPtr ble_beacon_setting_characteristic =
      std::make_shared<BleBeaconSettingCharacteristic>(
          beacon_setting_characteristic_uuid, beacon_setting_char_property,
          weak_from_this());

  // Create BleBRDFService 347fd67c-9131-4ea0-b0a7-1886d8c0f0df
  constexpr uint8_t SERVICE_UUID_RAW[ESP_UUID_LEN_128] = { // 128bit Little Endian
      0xDF, 0xF0, 0xC0, 0xD8, 0x86, 0x18, 0xA7, 0xB0,
      0xA0, 0x4E, 0x31, 0x91, 0x7C, 0xD6, 0x7F, 0x34};

  esp_bt_uuid_t service_uuid = {.len = ESP_UUID_LEN_128,
                                .uuid = {.uuid128 = {}}};
  std::memcpy(service_uuid.uuid.uuid128, SERVICE_UUID_RAW, ESP_UUID_LEN_128);
  BleServiceInterfaceSharedPtr ble_brdf_service =
      std::make_shared<BleBRDFService>(0, service_uuid, 8);
  ble_brdf_service->AddCharacteristic(ble_voltage_characteristic);
  ble_brdf_service->AddCharacteristic(ble_beacon_setting_characteristic);

  // create iBeacon Adv data
  BleIBeacon ibeacon_adv_data =
      CreateiBeaconAttr(BRDF_IBEACON_PROXIMITY_UUID, setting_->GetMajor(),
                        setting_->GetMinor(), setting_->GetMeasuredPower());

  // Start Bletooth Low Energy
  BleDevice *const ble_device = BleDevice::GetInstance();
  ble_device->Initialize(setting_->GetDeviceName(), ibeacon_adv_data);
  ble_device->AddService(ble_brdf_service);
  ble_device->StartAdvertising();

  esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_DEFAULT,
                       setting_->GetEspTxPowerLevel());
}

float BrdfBeacon::GetBatteryVoltage() const {
  if (voltage_check_task_) {
    return voltage_check_task_->GetVoltage();
  }
  return 0.0f;
}

BeaconSettingConstWeakPtr BrdfBeacon::GetSetting() const { return setting_; }

}  // namespace BrdfBeaconSystem
