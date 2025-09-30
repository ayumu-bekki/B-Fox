#ifndef BRDF_BEACON_MAIN_UTIL_H_
#define BRDF_BEACON_MAIN_UTIL_H_
// ESP32 BRDF Beacon
// (C)2025 bekki.jp

#include <chrono>
#include <string>
#include <vector>

namespace brdf_beacon_system {
namespace util {

/// Sleep
void SleepMillisecond(const uint32_t sleep_milliseconds);

/// Split Text
std::vector<std::string> SplitString(const std::string& str, const char delim);

}  // namespace util
}  // namespace brdf_beacon_system

#endif  // BRDF_BEACON_MAIN_UTIL_H_
