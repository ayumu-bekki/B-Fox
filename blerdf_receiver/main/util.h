#ifndef BleRDF_RECEIVER_MAIN_UTIL_H_
#define BleRDF_RECEIVER_MAIN_UTIL_H_
// ESP32 BleRDF Receiver
// (C)2025 bekki.jp

// Include ----------------------
#include <chrono>
#include <string>
#include <vector>

namespace blerdf_receiver_system {
namespace util {

/// Sleep
void SleepMillisecond(const uint32_t sleep_milliseconds);

/// Split Text
std::vector<std::string> SplitString(const std::string& str, const char delim);

}  // namespace util
}  // namespace blerdf_receiver_system

#endif  // BleRDF_RECEIVER_MAIN_UTIL_H_
