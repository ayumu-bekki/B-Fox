// ESP32 B-Fox Receiver
// (C)2025 bekki.jp

#include <cstring>
#include <memory>

#include "bfox_receiver.h"
#include "driver/gpio.h"
#include "driver/rtc_io.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_gap_ble_api.h"
#include "esp_sleep.h"
#include "esp_system.h"
#include "file_system.h"
#include "gpio_control.h"
#include "i2c_util.h"
#include "logger.h"
#include "nvs_flash.h"
#include "receiver_setting.h"
#include "st7032.h"
#include "util.h"
#include "version.h"
#include "xiao_esp32c6_pin.h"

namespace bfox_receiver_system {

// kA0 Input Enable

// Wakeup GPIO Pin
constexpr gpio_num_t kWakeupGpio = xiao_esp32c6_pin::kD1;

// Major Change GPIO Pin
constexpr gpio_num_t kMajorChangeGpio = xiao_esp32c6_pin::kD10;

// Unused GPIO Pins
const gpio_num_t kUnusedGpioPins[] = {
    xiao_esp32c6_pin::kD2,         xiao_esp32c6_pin::kMtms,
    xiao_esp32c6_pin::kMtdi,       xiao_esp32c6_pin::kMtck,
    xiao_esp32c6_pin::kMtdo,       xiao_esp32c6_pin::kBoot,
    xiao_esp32c6_pin::kLedBuiltin, xiao_esp32c6_pin::kTx,
    xiao_esp32c6_pin::kRx,         xiao_esp32c6_pin::kMosi,
    xiao_esp32c6_pin::kSck,        xiao_esp32c6_pin::kMiso,
    xiao_esp32c6_pin::kSS,
};

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
constexpr int kLcdDisplayLines = 2;

BFoxReceiver::BFoxReceiver()
    : gpio_watcher_(),
      st7032_(),
      beacon_receive_task_(),
      receiver_status_(ReceiverStatus::kSearchMode),
      major_(0),
      sleep_count_(0) {}

BFoxReceiver::~BFoxReceiver() = default;

void BFoxReceiver::Start() {
  // Initialize Log
  logger::InitializeLogLevel();

  ESP_LOGI(kTag, "Startup B-Fox Receiver. Version:%s",
           std::string(kGitVersion).c_str());

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
  i2c_util::InitializeMaster(kI2cPortNo, xiao_esp32c6_pin::kSda,
                             xiao_esp32c6_pin::kScl);

  // ST7032 (LCD Display)
  st7032_.Setup(kI2cPortNo, ST7032::kI2cDefaultAddr, 16, 2);
  st7032_.SetContrast(40);

  // Initial LCD display
  st7032_.SetCursor(0, 0);
  st7032_.Print("B-Fox Receiver  ");
  st7032_.SetCursor(0, 1);
  st7032_.Printf(" Maj:%d Bat:%4.2fV", major_, voltage);

  // Setting to wake up from DeepSleep when D1 is LOW
  rtc_gpio_init(kWakeupGpio);
  rtc_gpio_set_direction(kWakeupGpio, RTC_GPIO_MODE_INPUT_ONLY);
  rtc_gpio_pullup_en(kWakeupGpio);
  rtc_gpio_pulldown_dis(kWakeupGpio);
  esp_sleep_enable_ext1_wakeup((1ULL << kWakeupGpio), ESP_EXT1_WAKEUP_ANY_LOW);

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

  // Set Button Event
  gpio_watcher_.AddMonitor(
      GpioInputWatchTask::GpioInfo(
          kMajorChangeGpio, std::bind(&BFoxReceiver::OnSetMajorButton, this),
          std::bind(&BFoxReceiver::OnSetMajorLongButton, this)),
      GpioInputWatchTask::GpioPullUpDown::kPullUpResistorEnable);
  gpio_watcher_.Start();

  // Ble Receive Task
  beacon_receive_task_ =
      std::make_unique<BeaconReceiveTask>(kTargetProximityUuid, major_);
  if (!beacon_receive_task_) {
    return;
  }
  beacon_receive_task_->Start();

  ESP_LOGI(kTag, "Activation Complete B-Fox Receiver System.");

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

void BFoxReceiver::BeaconSearchMode() {
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
      beacon_receive_task_->GetRSSISortedItems();
  if (ble_beacon_list.empty()) {
    st7032_.SetCursor(0, 0);
    st7032_.Print("NO SIGNAL       ");
    st7032_.SetCursor(0, 1);
    st7032_.Print("                ");
  } else {
    for (int bleIdx = 0; bleIdx < kLcdDisplayLines; ++bleIdx) {
      st7032_.SetCursor(0, bleIdx);
      if (ble_beacon_list.size() <= bleIdx) {
        st7032_.Print("                ");
      } else {
        const BleBeaconItem& info = ble_beacon_list[bleIdx];

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

void BFoxReceiver::SettingMode() {
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

void BFoxReceiver::SettingFinishMode() {
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

void BFoxReceiver::OnSetMajorButton() {
  ESP_LOGI(kTag, "OnSetMajorButton");
  if (receiver_status_ == ReceiverStatus::kSearchMode) {
    sleep_count_ = 0;
    receiver_status_ = ReceiverStatus::kSettingMode;
  } else if (receiver_status_ == ReceiverStatus::kSettingMode) {
    sleep_count_ = 0;
    major_ = (major_ + 1) % 10;
  }
}

void BFoxReceiver::OnSetMajorLongButton() {
  ESP_LOGI(kTag, "OnSetMajorLongButton");
  if (receiver_status_ == ReceiverStatus::kSettingMode) {
    receiver_status_ = ReceiverStatus::kSettingFinishMode;
  }
}

}  // namespace bfox_receiver_system
