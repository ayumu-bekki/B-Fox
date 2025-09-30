#ifndef LOGGER_H_
#define LOGGER_H_
// ESP32 BRDF Beacon
// (C)2025 bekki.jp

// Include ----------------------
#include <esp_log.h>

// systen Loglevel Redefine
#undef LOG_LOCAL_LEVEL
#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE

namespace BrdfBeaconSystem {

/// Application Log Tag
static constexpr char TAG[] = "BrdfBeacon";

namespace Logger {

void InitializeLogLevel();

}  // namespace Logger
}  // namespace BrdfBeaconSystem

#endif  // LOGGER_H_
