// ESP32 B-Fox Beacon
// (C)2025 bekki.jp

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
#include "bfox_beacon.h"
#include "ble_device.h"
#include "ble_services.h"
#include "file_system.h"
#include "gpio_control.h"
#include "ibeacon.h"
#include "logger.h"
#include "util.h"
#include "version.h"
#include "xiao_esp32c6_pin.h"

namespace bfox_beacon_system {

// kA0 Input Enable

// Unused GPIO Pins
const gpio_num_t kUnusedGpioPins[] = {
    xiao_esp32c6_pin::kD1,   xiao_esp32c6_pin::kD2,
    xiao_esp32c6_pin::kMtms, xiao_esp32c6_pin::kMtdi,
    xiao_esp32c6_pin::kMtck, xiao_esp32c6_pin::kMtdo,
    xiao_esp32c6_pin::kBoot, xiao_esp32c6_pin::kLedBuiltin,
    xiao_esp32c6_pin::kTx,   xiao_esp32c6_pin::kMosi,
    xiao_esp32c6_pin::kSck,  xiao_esp32c6_pin::kMiso,
    xiao_esp32c6_pin::kSS,   xiao_esp32c6_pin::kSda,
    xiao_esp32c6_pin::kScl,
};

// PROXIMITY_UUID (UUID for B-Fox Receiver), for iBeacon data, 128-bit, Big
// Endian
constexpr uint8_t kBFoxIBeaconProximityUuid[ESP_UUID_LEN_128] = {
    0xC6, 0x5B, 0x2C, 0x5D,             // C65B2C5D
    0x9E, 0x53,                         // 9E53
    0x46, 0xEC,                         // 46EC
    0x8B, 0x8E,                         // 8B8E
    0x54, 0xD9, 0xE2, 0xF2, 0x11, 0x88  // 54D9E2F21188
};

BFoxBeacon::BFoxBeacon() : voltage_check_task_(), setting_() {}

BFoxBeacon::~BFoxBeacon() = default;

void BFoxBeacon::Start() {
  // Initialize Log
  logger::InitializeLogLevel();

  ESP_LOGI(TAG, "Startup bfox Beacon. Version:%s",
           std::string(kGitVersion).c_str());

  // Monitoring LED Init
  gpio::InitOutput(kMonitoringLedPin, false);

  // Battery voltage monitoring
  voltage_check_task_ = std::make_unique<VoltageCheckTask>();
  if (!voltage_check_task_) {
    esp_deep_sleep_start();
  }
  voltage_check_task_->Start();

  // Use Ext Antenna
  gpio::InitOutput(xiao_esp32c6_pin::kWifiEnable,
                   false);  // Activate RF switch control (kWifiEnable = False)
  util::SleepMillisecond(100);
  gpio::InitOutput(xiao_esp32c6_pin::kWifiAntConfig,
                   true);  // Use external antenna (KWifiAntConfig = True)

  // Set unused GPIOs to input with pull-up for stability
  for (const auto& gpio_num : kUnusedGpioPins) {
    gpio_set_direction(gpio_num, GPIO_MODE_INPUT);
    gpio_set_pull_mode(gpio_num, GPIO_PULLDOWN_ONLY);
  }

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
  file_system::Mount();

  // Load Setting
  setting_ = std::make_shared<BeaconSetting>();
  setting_->Load();

  ESP_LOGI(TAG, "Start BLE GATT name:%s major:%d minor:%d",
           setting_->GetDeviceName().c_str(), setting_->GetMajor(),
           setting_->GetMinor());

  // Bluetooth GATT Server
  CreateBLEService();

  ESP_LOGI(TAG, "Activation Complete bfox Beacon System.");

  while (true) {
    gpio::SetLevel(kMonitoringLedPin, true);
    util::SleepMillisecond(100);
    gpio::SetLevel(kMonitoringLedPin, false);

    util::SleepMillisecond(1900);
  }
}

void BFoxBeacon::CreateBLEService() {
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
  constexpr uint8_t kVoltageCharacteristicUuidRaw[ESP_UUID_LEN_128] = {
      // 128bit Little Endian
      0x05, 0x09, 0x77, 0xF0, 0xB7, 0x4F, 0x75, 0xB6,
      0xA3, 0x46, 0xBA, 0x41, 0x46, 0x4A, 0xBF, 0x53};
  esp_bt_uuid_t voltage_characteristic_uuid = {.len = ESP_UUID_LEN_128,
                                               .uuid = {.uuid128 = {}}};
  std::memcpy(voltage_characteristic_uuid.uuid.uuid128,
              kVoltageCharacteristicUuidRaw, ESP_UUID_LEN_128);
  BleCharacteristicInterfaceSharedPtr ble_voltage_characteristic =
      std::make_shared<BleVoltageCharacteristic>(
          voltage_characteristic_uuid, voltage_char_property, weak_from_this());

  // Create BleBeaconSettingCharacteristic
  constexpr esp_gatt_char_prop_t beacon_setting_char_property =
      ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_WRITE;
  // 096a09d5-1b35-4c99-a483-8d0c34f70220
  constexpr uint8_t kDemoCharacteristicUuidRaw[ESP_UUID_LEN_128] = {
      // 128bit Little Endian
      0x20, 0x02, 0xF7, 0x34, 0x0C, 0x8D, 0x83, 0xA4,
      0x99, 0x4C, 0x35, 0x1B, 0xD5, 0x09, 0x6A, 0x09};
  esp_bt_uuid_t beacon_setting_characteristic_uuid = {.len = ESP_UUID_LEN_128,
                                                      .uuid = {.uuid128 = {}}};
  std::memcpy(beacon_setting_characteristic_uuid.uuid.uuid128,
              kDemoCharacteristicUuidRaw, ESP_UUID_LEN_128);
  BleCharacteristicInterfaceSharedPtr ble_beacon_setting_characteristic =
      std::make_shared<BleBeaconSettingCharacteristic>(
          beacon_setting_characteristic_uuid, beacon_setting_char_property,
          weak_from_this());

  // Create BleDeepSleepCharacteristic : 0cf26a7e-650e-4c18-9a30-bbbca50d88c1
  constexpr esp_gatt_char_prop_t deep_sleep_char_property =
      ESP_GATT_CHAR_PROP_BIT_WRITE;
  constexpr uint8_t kDeepSleepCharacteristicUuidRaw[ESP_UUID_LEN_128] = {
      // 128bit Little Endian
      0xC1, 0x88, 0x0D, 0xA5, 0xBC, 0xBB, 0x30, 0x9A,
      0x18, 0x4C, 0x0E, 0x65, 0x7E, 0x6A, 0xF2, 0x0C};
  esp_bt_uuid_t deep_sleep_characteristic_uuid = {.len = ESP_UUID_LEN_128,
                                                  .uuid = {.uuid128 = {}}};
  std::memcpy(deep_sleep_characteristic_uuid.uuid.uuid128,
              kDeepSleepCharacteristicUuidRaw, ESP_UUID_LEN_128);
  BleCharacteristicInterfaceSharedPtr ble_deep_sleep_characteristic =
      std::make_shared<BleDeepSleepCharacteristic>(
          deep_sleep_characteristic_uuid, deep_sleep_char_property,
          weak_from_this());

  // Create BleBFoxService 347fd67c-9131-4ea0-b0a7-1886d8c0f0df
  constexpr uint8_t kServiceUuidRaw[ESP_UUID_LEN_128] = {
      // 128bit Little Endian
      0xDF, 0xF0, 0xC0, 0xD8, 0x86, 0x18, 0xA7, 0xB0,
      0xA0, 0x4E, 0x31, 0x91, 0x7C, 0xD6, 0x7F, 0x34};

  esp_bt_uuid_t service_uuid = {.len = ESP_UUID_LEN_128,
                                .uuid = {.uuid128 = {}}};
  std::memcpy(service_uuid.uuid.uuid128, kServiceUuidRaw, ESP_UUID_LEN_128);
  BleServiceInterfaceSharedPtr ble_BFOX_service =
      std::make_shared<BleBFoxService>(0, service_uuid, 10);
  ble_BFOX_service->AddCharacteristic(ble_voltage_characteristic);
  ble_BFOX_service->AddCharacteristic(ble_beacon_setting_characteristic);
  ble_BFOX_service->AddCharacteristic(ble_deep_sleep_characteristic);

  // create iBeacon Adv data
  BleIBeacon ibeacon_adv_data =
      CreateIBeaconAttr(kBFoxIBeaconProximityUuid, setting_->GetMajor(),
                        setting_->GetMinor(), setting_->GetMeasuredPower());

  // Start Bletooth Low Energy
  BleDevice* const ble_device = BleDevice::GetInstance();
  ble_device->Initialize(setting_->GetDeviceName(), ibeacon_adv_data);
  ble_device->AddService(ble_BFOX_service);
  ble_device->StartAdvertising();

  esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_DEFAULT,
                       setting_->GetEspTxPowerLevel());
}

float BFoxBeacon::GetBatteryVoltage() const {
  if (voltage_check_task_) {
    return voltage_check_task_->GetVoltage();
  }
  return 0.0f;
}

BeaconSettingConstWeakPtr BFoxBeacon::GetSetting() const { return setting_; }

}  // namespace bfox_beacon_system
