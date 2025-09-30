// ESP32 BRDF Beacon
// (C)2025 bekki.jp

// Include ----------------------
#include "voltage_check_task.h"

#include <esp_adc/adc_oneshot.h>
#include <esp_sleep.h>

#include <iomanip>
#include <sstream>

#include "gpio_control.h"
#include "logger.h"
#include "util.h"

namespace BrdfBeaconSystem {

constexpr float BATTERY_DISCHARGE_LIMIT = 3.2f;

VoltageCheckTask::VoltageCheckTask()
    : Task(TASK_NAME, PRIORITY, CORE_ID), voltage_(0.0f) {}

void VoltageCheckTask::Initialize() {}

void VoltageCheckTask::Update() {
  static const int32_t VOLTAGE_ADC_CHECK_ROUND = 10;
  const uint32_t adc_voltage =
      GPIO::GetAdcVoltage(ADC_CHANNEL_0, VOLTAGE_ADC_CHECK_ROUND);
  voltage_ = 2.0f * adc_voltage / 1000.0f;
  // ESP_LOGI(TAG, "Battery Voltage %4.2fV", voltage_);

  if (voltage_ <= BATTERY_DISCHARGE_LIMIT) {
    ESP_LOGW(TAG, "Battery Voltage is LOW. %4.2fV", voltage_);
    esp_deep_sleep_start();
  }

  static const int32_t NEXT_CHECK_MILLISECOND = 60 * 1000;
  Util::SleepMillisecond(NEXT_CHECK_MILLISECOND);
}

float VoltageCheckTask::GetVoltage() const { return voltage_; }

}  // namespace BrdfBeaconSystem
