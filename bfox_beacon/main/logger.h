#ifndef BFOX_BEACON_MAIN_LOGGER_H_
#define BFOX__BEACON_MAIN_LOGGER_H_
// ESP32 B-Fox Beacon
// (C)2025 bekki.jp

#include <esp_log.h>

// system Loglevel Redefine
#undef LOG_LOCAL_LEVEL
#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE

namespace bfox_beacon_system {

/// Application Log Tag
inline constexpr char TAG[] = "BFoxBeacon";

namespace logger {

void InitializeLogLevel();

}  // namespace logger
}  // namespace bfox_beacon_system

#endif  // BFOX_BEACON_MAIN_LOGGER_H_
