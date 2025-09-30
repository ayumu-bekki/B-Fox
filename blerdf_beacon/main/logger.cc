// ESP32 BleRDF Beacon
// (C)2025 bekki.jp

#include "logger.h"

namespace blerdf_beacon_system {
namespace logger {

void InitializeLogLevel() {
#if CONFIG_DEBUG != 0
  esp_log_level_set("*", ESP_LOG_INFO);
  esp_log_level_set(TAG, ESP_LOG_VERBOSE);
  ESP_LOGD(TAG, "DEBUG MODE");
#else
  esp_log_level_set("*", ESP_LOG_WARN);
  esp_log_level_set(TAG, ESP_LOG_INFO);
#endif
}

}  // namespace logger
}  // namespace blerdf_beacon_system
