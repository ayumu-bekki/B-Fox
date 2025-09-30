#ifndef BleRDF_RECEIVER_MAIN_LOGGER_H_
#define BleRDF_RECEIVER_MAIN_LOGGER_H_
// ESP32 BleRDF Receiver
// (C)2025 bekki.jp

// Include ----------------------
#include <esp_log.h>

// systen Loglevel Redefine
#undef LOG_LOCAL_LEVEL
#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE

namespace blerdf_receiver_system {

/// Application Log Tag
static constexpr char kTag[] = "BleRDFReceiver";

namespace logger {

void InitializeLogLevel();

}  // namespace logger
}  // namespace blerdf_receiver_system

#endif  // BleRDF_RECEIVER_MAIN_LOGGER_H_
