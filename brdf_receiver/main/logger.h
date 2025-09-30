#ifndef BRDF_RECEIVER_MAIN_LOGGER_H_
#define BRDF_RECEIVER_MAIN_LOGGER_H_
// ESP32 BRDF Receiver
// (C)2025 bekki.jp

// Include ----------------------
#include <esp_log.h>

// systen Loglevel Redefine
#undef LOG_LOCAL_LEVEL
#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE

namespace brdf_receiver_system {

/// Application Log Tag
static constexpr char kTag[] = "BrdfReceiver";

namespace logger {

void InitializeLogLevel();

}  // namespace logger
}  // namespace brdf_receiver_system

#endif  // BRDF_RECEIVER_MAIN_LOGGER_H_
