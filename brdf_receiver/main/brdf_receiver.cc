// ESP32 BRDF Receiver
// (C)2025 bekki.jp

#include "brdf_receiver.h"

#include <cstring>
#include <memory>

#include "driver/gpio.h"
#include "driver/rtc_io.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_gap_ble_api.h"
#include "esp_sleep.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "file_system.h"
#include "gpio_control.h"
#include "i2c_util.h"
#include "logger.h"
#include "receiver_setting.h"
#include "st7032.h"
#include "util.h"
#include "version.h"
#include "xiao_esp32c6_pin.h"

namespace brdf_receiver_system {

constexpr float kBatteryDischargeLimit = 3.2f;
constexpr int kIndicatorRssiNum = 6;
const int kIndicatorRssiTargets[kIndicatorRssiNum] = {INT_MIN, -100, -80,
                                                      -70,     -60,  -50};

static const uint8_t kTargetProximityUuid[16] = {
    0xC6, 0x5B, 0x2C, 0x5D,             // C65B2C5D
    0x9E, 0x53,                         // 9E53
    0x46, 0xEC,                         // 46EC
    0x8B, 0x8E,                         // 8B8E
    0x54, 0xD9, 0xE2, 0xF2, 0x11, 0x88  // 54D9E2F21188
};

constexpr int32_t kSearchModeSleepThreshold = 15;
constexpr i2c_port_t kI2cPortNo = I2C_NUM_0;

BrdfReceiver::BrdfReceiver()
    : gpio_watcher_(),
      st7032_(),
      beacon_receive_task_(),
      receiver_status_(ReceiverStatus::kSearchMode),
      major_(0),
      sleep_count_(0) {}

BrdfReceiver::~BrdfReceiver() = default;

void BrdfReceiver::Start() {
  // Initialize Log
  logger::InitializeLogLevel();

  ESP_LOGI(kTag, "Startup BRDF Receiver. Version:%s",
           std::string(GIT_VERSION).c_str());

  // Initialize NVS
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
      ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_LOGE(kTag, "NVS Flash Error");
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  // Get battery voltage
  static const int32_t kVoltageAdcCheckRound = 10;
  const uint32_t adc_voltage =
      gpio::GetAdcVoltage(ADC_CHANNEL_0, kVoltageAdcCheckRound);
  float voltage = 2.0f * adc_voltage / 1000.0f;

  if (voltage <= kBatteryDischargeLimit) {
    ESP_LOGW(kTag, "Battery Voltage is LOW. %4.2fV", voltage);

    st7032_.SetCursor(0, 0);
    st7032_.Printf("LowBattery:%4.2fV", voltage);

    util::SleepMillisecond(3000);
    st7032_.Clear();
    esp_deep_sleep_start();
  }

  // Mount File System
  file_system::Mount();

  // Read Setting
  ReceiverSetting setting;
  setting.Load();
  major_ = setting.GetMajor();

  // Initialize I2C
  i2c_util::InitializeMaster(kI2cPortNo, xiao_esp32c6_pin::SDA,
                            xiao_esp32c6_pin::SCL);

  // ST7032 (LCD Display)
  st7032_.Setup(kI2cPortNo, ST7032::kI2cDefaultAddr, 16, 2);
  st7032_.SetContrast(40);

  // Initial LCD display
  st7032_.SetCursor(0, 0);
  st7032_.Print("BRDF Receiver   ");
  st7032_.SetCursor(0, 1);
  st7032_.Printf(" Maj:%d Bat:%4.2fV", major_, voltage);

  // Setting to wake up from DeepSleep when D1 is LOW
  constexpr gpio_num_t kWakeupGpio = GPIO_NUM_1;
  rtc_gpio_init(kWakeupGpio);
  rtc_gpio_set_direction(kWakeupGpio, RTC_GPIO_MODE_INPUT_ONLY);
  rtc_gpio_pullup_en(kWakeupGpio);
  rtc_gpio_pulldown_dis(kWakeupGpio);
  esp_sleep_enable_ext1_wakeup((1ULL << kWakeupGpio), ESP_EXT1_WAKEUP_ANY_LOW);

  // Use Ext Antenna
  gpio::InitOutput(xiao_esp32c6_pin::WIFI_ENABLE, false);
  util::SleepMillisecond(100);
  gpio::InitOutput(xiao_esp32c6_pin::WIFI_ANT_CONFIG, true);

  // Set Button Event
  gpio_watcher_.AddMonitor(
      GpioInputWatchTask::GpioInfo(
          xiao_esp32c6_pin::D7, std::bind(&BrdfReceiver::OnSetMajorButton, this),
          std::bind(&BrdfReceiver::OnSetMajorLongButton, this)),
      GpioInputWatchTask::kPullUpResistorEnable);
  gpio_watcher_.Start();

  // Ble Receive Task
  beacon_receive_task_ =
      std::make_unique<BeaconReceiveTask>(kTargetProximityUuid, major_);
  if (!beacon_receive_task_) {
    return;
  }
  beacon_receive_task_->Start();

  ESP_LOGI(kTag, "Activation Complete BRDF Receiver System.");

  util::SleepMillisecond(2000);  // Initial wait time

  while (true) {
    if (receiver_status_ == ReceiverStatus::kSearchMode) {
      BeaconSearchMode();
    } else if (receiver_status_ == ReceiverStatus::kSettingMode) {
      SettingMode();
    } else if (receiver_status_ == ReceiverStatus::kSettingFinishMode) {
      SettingFinishMode();
    }
  }
}

void BrdfReceiver::BeaconSearchMode() {
  // Sleep after a certain number of iterations
  ++sleep_count_;
  ESP_LOGI(kTag, "Cnt:%d", sleep_count_);
  if (kSearchModeSleepThreshold <= sleep_count_) {
    ESP_LOGI(kTag, "Sleep...");
    st7032_.Clear();
    esp_deep_sleep_start();
  }

  // Get and display iBeacon information
  std::vector<BleBeaconItem> ble_beacon_list =
      beacon_receive_task_->GetSSRISortedItems();
  if (ble_beacon_list.empty()) {
    st7032_.SetCursor(0, 0);
    st7032_.Print("NO SIGNAL       ");
    st7032_.SetCursor(0, 1);
    st7032_.Print("                ");
  } else {
    for (int bleIdx = 0; bleIdx < 2; ++bleIdx) {
      st7032_.SetCursor(0, bleIdx);
      if (ble_beacon_list.size() <= bleIdx) {
        st7032_.Print("                ");
      } else {
        BleBeaconItem info = ble_beacon_list[bleIdx];

        st7032_.Printf("%d|", info.minor);
        for (int indicator_idx = 0; indicator_idx < kIndicatorRssiNum;
             ++indicator_idx) {
          if (kIndicatorRssiTargets[indicator_idx] < info.rssi) {
            st7032_.Printf("#");
          } else {
            st7032_.Printf(" ");
          }
        }
        st7032_.Printf("|%-4ddBm", info.rssi);
      }
    }
  }

  util::SleepMillisecond(2000);
}

void BrdfReceiver::SettingMode() {
  // Sleep after a certain number of iterations
  ++sleep_count_;
  ESP_LOGI(kTag, "Cnt:%d", sleep_count_);
  if (100 <= sleep_count_) {
    st7032_.SetCursor(0, 0);
    st7032_.Print("Restart...      ");
    st7032_.SetCursor(0, 1);
    st7032_.Print("                ");

    util::SleepMillisecond(1900);
    ESP_LOGI(kTag, "Sleep...");
    st7032_.Clear();
    util::SleepMillisecond(100);
    esp_restart();
  }

  st7032_.SetCursor(0, 0);
  st7032_.Print("Setting Mode    ");
  st7032_.SetCursor(0, 1);
  st7032_.Printf(" Major:%d        ", major_);

  util::SleepMillisecond(100);
}

void BrdfReceiver::SettingFinishMode() {
  ReceiverSetting setting;
  setting.SetMajor(major_);
  setting.Save();

  st7032_.SetCursor(0, 0);
  st7032_.Printf("Saved Major:%d   ", major_);
  st7032_.SetCursor(0, 1);
  st7032_.Print(" Restart...     ");

  util::SleepMillisecond(1900);
  ESP_LOGI(kTag, "Restart");
  util::SleepMillisecond(100);
  esp_restart();
}

void BrdfReceiver::OnSetMajorButton() {
  ESP_LOGI(kTag, "OnSetMajorButton");
  if (receiver_status_ == ReceiverStatus::kSearchMode) {
    sleep_count_ = 0;
    receiver_status_ = ReceiverStatus::kSettingMode;
  } else if (receiver_status_ == ReceiverStatus::kSettingMode) {
    sleep_count_ = 0;
    major_ = (major_ + 1) % 10;
  }
}

void BrdfReceiver::OnSetMajorLongButton() {
  ESP_LOGI(kTag, "OnSetMajorLongButton");
  if (receiver_status_ == ReceiverStatus::kSettingMode) {
    receiver_status_ = ReceiverStatus::kSettingFinishMode;
  }
}

}  // namespace brdf_receiver_system
