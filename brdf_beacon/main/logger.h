#ifndef BRDF_BEACON_MAIN_LOGGER_H_
#define BRDF_BEACON_MAIN_LOGGER_H_
// ESP32 BRDF Beacon
// (C)2025 bekki.jp

#include <esp_log.h>

// system Loglevel Redefine
#undef LOG_LOCAL_LEVEL
#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE

namespace brdf_beacon_system {

/// Application Log Tag
inline constexpr char TAG[] = "BrdfBeacon";

namespace logger {

void InitializeLogLevel();

}  // namespace logger
}  // namespace brdf_beacon_system

#endif  // BRDF_BEACON_MAIN_LOGGER_H_
