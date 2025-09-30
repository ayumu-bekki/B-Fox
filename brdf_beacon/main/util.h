#ifndef UTIL_H_
#define UTIL_H_
// ESP32 BRDF Beacon
// (C)2025 bekki.jp

// Include ----------------------
#include <chrono>
#include <string>
#include <vector>

namespace BrdfBeaconSystem::Util {

/// Sleep
void SleepMillisecond(const uint32_t sleep_milliseconds);

/// Split Text
std::vector<std::string> SplitString(const std::string& str, const char delim);

}  // namespace BrdfBeaconSystem::Util

#endif  // UTIL_H_
