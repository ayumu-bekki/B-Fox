// ESP32 BleRDF Beacon
// (C)2025 bekki.jp

#include "voltage_check_task.h"

#include <esp_adc/adc_oneshot.h>
#include <esp_sleep.h>

#include <iomanip>
#include <sstream>

#include "gpio_control.h"
#include "logger.h"
#include "util.h"

namespace blerdf_beacon_system {

constexpr float kBatteryDischargeLimit = 3.2f;

VoltageCheckTask::VoltageCheckTask()
    : Task(kTaskName, kPriority, kCoreId), voltage_(0.0f) {}

void VoltageCheckTask::Initialize() {}

void VoltageCheckTask::Update() {
  static const int32_t kVoltageAdcCheckRound = 10;
  const uint32_t adc_voltage =
      gpio::GetAdcVoltage(ADC_CHANNEL_0, kVoltageAdcCheckRound);
  voltage_ = 2.0f * adc_voltage / 1000.0f;
  // ESP_LOGI(TAG, "Battery Voltage %4.2fV", voltage_);

  if (voltage_ <= kBatteryDischargeLimit) {
    ESP_LOGW(TAG, "Battery Voltage is LOW. %4.2fV", voltage_);
    esp_deep_sleep_start();
  }

  static const int32_t kNextCheckMillisecond = 60 * 1000;
  util::SleepMillisecond(kNextCheckMillisecond);
}

float VoltageCheckTask::GetVoltage() const { return voltage_; }

}  // namespace blerdf_beacon_system
