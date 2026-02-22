// ESP32 B-Fox Beacon
// (C)2025 bekki.jp

#include <driver/gpio.h>
#include <driver/rtc_io.h>
#include <esp_adc/adc_cali.h>
#include <esp_adc/adc_cali_scheme.h>
#include <esp_bt.h>
#include <esp_sleep.h>
#include <esp_system.h>
#include <nvs_flash.h>

#include <cstring>
#include <memory>

#include "beacon_setting.h"
#include "bfox_beacon.h"
#include "ble_device.h"
#include "ble_services.h"
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

static BleBFoxService* g_ble_bfox_service_ptr = nullptr;

void BFoxBeacon::CreateBLEService() {
  // Create BleBFoxService
  g_ble_bfox_service_ptr = new BleBFoxService(weak_from_this());

  // create iBeacon Adv data
  BleIBeacon ibeacon_adv_data =
      CreateIBeaconAttr(kBFoxIBeaconProximityUuid, setting_->GetMajor(),
                        setting_->GetMinor(), setting_->GetMeasuredPower());

  // Start Bluetooth Low Energy (NimBLE)
  BleDevice* const ble_device = BleDevice::GetInstance();
  ble_device->Initialize(setting_->GetDeviceName(), ibeacon_adv_data);
  ble_device->RegisterServices(g_ble_bfox_service_ptr->GetServiceDefs());
  
  // Set TX power (controller-level API, works with NimBLE)
  esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_DEFAULT,
                       setting_->GetEspTxPowerLevel());
  
  // Start NimBLE host task
  ble_device->StartHost();
}

float BFoxBeacon::GetBatteryVoltage() const {
  if (voltage_check_task_) {
    return voltage_check_task_->GetVoltage();
  }
  return 0.0f;
}

BeaconSettingConstWeakPtr BFoxBeacon::GetSetting() const { return setting_; }

}  // namespace bfox_beacon_system
