#ifndef BleRDF_BEACON_MAIN_LOGGER_H_
#define BleRDF_BEACON_MAIN_LOGGER_H_
// ESP32 BleRDF Beacon
// (C)2025 bekki.jp

#include <esp_log.h>

// system Loglevel Redefine
#undef LOG_LOCAL_LEVEL
#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE

namespace blerdf_beacon_system {

/// Application Log Tag
inline constexpr char TAG[] = "BleRDFBeacon";

namespace logger {

void InitializeLogLevel();

}  // namespace logger
}  // namespace blerdf_beacon_system

#endif  // BleRDF_BEACON_MAIN_LOGGER_H_
