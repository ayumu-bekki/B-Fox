// ESP32 BRDF Beacon
// (C)2025 bekki.jp

// Include ----------------------
#include "util.h"

#include <esp_sntp.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <lwip/err.h>
#include <lwip/sys.h>

#include <cmath>
#include <iomanip>
#include <sstream>

#include "gpio_control.h"
#include "logger.h"

namespace BrdfBeaconSystem {
namespace Util {

/// Sleep
void SleepMillisecond(const uint32_t sleep_milliseconds) {
  TickType_t lastWakeTime = xTaskGetTickCount();
  vTaskDelayUntil(&lastWakeTime, sleep_milliseconds / portTICK_PERIOD_MS);
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

}  // namespace Util
}  // namespace BrdfBeaconSystem
