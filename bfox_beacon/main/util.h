#ifndef BFOX_BEACON_MAIN_UTIL_H_
#define BFOX__BEACON_MAIN_UTIL_H_
// ESP32 B-Fox Beacon
// (C)2025 bekki.jp

#include <chrono>
#include <string>
#include <vector>

namespace bfox_beacon_system {
namespace util {

/// Sleep
void SleepMillisecond(const uint32_t sleep_milliseconds);

/// Split Text
std::vector<std::string> SplitString(const std::string& str, const char delim);

}  // namespace util
}  // namespace bfox_beacon_system

#endif  // BFOX_BEACON_MAIN_UTIL_H_
