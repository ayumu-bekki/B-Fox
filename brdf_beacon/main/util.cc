// ESP32 BRDF Beacon
// (C)2025 bekki.jp

#include "util.h"

#include <cmath>
#include <iomanip>
#include <sstream>

#include <esp_sntp.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <lwip/err.h>
#include <lwip/sys.h>

#include "gpio_control.h"
#include "logger.h"

namespace brdf_beacon_system {
namespace util {

/// Sleep
void SleepMillisecond(const uint32_t sleep_milliseconds) {
  TickType_t last_wake_time = xTaskGetTickCount();
  vTaskDelayUntil(&last_wake_time, sleep_milliseconds / portTICK_PERIOD_MS);
}

std::vector<std::string> SplitString(const std::string& str, const char delim) {
  std::vector<std::string> elements;
  std::stringstream ss(str);
  std::string item;
  while (getline(ss, item, delim)) {
    if (!item.empty()) {
      elements.push_back(item);
    }
  }
  return elements;
}

}  // namespace util
}  // namespace brdf_beacon_system
