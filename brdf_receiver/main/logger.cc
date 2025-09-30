// ESP32 BRDF Receiver
// (C)2025 bekki.jp

// Include ----------------------
#include "logger.h"

namespace brdf_receiver_system {
namespace logger {

void InitializeLogLevel() {
#if CONFIG_DEBUG != 0
  esp_log_level_set("*", ESP_LOG_INFO);
  esp_log_level_set(kTag, ESP_LOG_VERBOSE);
  ESP_LOGD(kTag, "DEBUG MODE");
#else
  esp_log_level_set("*", ESP_LOG_WARN);
  esp_log_level_set(kTag, ESP_LOG_INFO);
#endif
}

}  // namespace logger
}  // namespace brdf_receiver_system
