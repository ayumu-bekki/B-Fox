#ifndef BleRDF_BEACON_MAIN_UTIL_H_
#define BleRDF_BEACON_MAIN_UTIL_H_
// ESP32 BleRDF Beacon
// (C)2025 bekki.jp

#include <chrono>
#include <string>
#include <vector>

namespace blerdf_beacon_system {
namespace util {

/// Sleep
void SleepMillisecond(const uint32_t sleep_milliseconds);

/// Split Text
std::vector<std::string> SplitString(const std::string& str, const char delim);

}  // namespace util
}  // namespace blerdf_beacon_system

#endif  // BleRDF_BEACON_MAIN_UTIL_H_
