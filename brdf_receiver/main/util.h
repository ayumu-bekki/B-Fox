#ifndef BRDF_RECEIVER_MAIN_UTIL_H_
#define BRDF_RECEIVER_MAIN_UTIL_H_
// ESP32 BRDF Receiver
// (C)2025 bekki.jp

// Include ----------------------
#include <chrono>
#include <string>
#include <vector>

namespace brdf_receiver_system {
namespace util {

/// Sleep
void SleepMillisecond(const uint32_t sleep_milliseconds);

/// Split Text
std::vector<std::string> SplitString(const std::string& str, const char delim);

}  // namespace util
}  // namespace brdf_receiver_system

#endif  // BRDF_RECEIVER_MAIN_UTIL_H_
