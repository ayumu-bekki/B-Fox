#ifndef BFOX_RECEIVER_MAIN_UTIL_H_
#define BFOX_RECEIVER_MAIN_UTIL_H_
// ESP32 B-Fox Receiver
// (C)2025 bekki.jp

// Include ----------------------
#include <chrono>
#include <string>
#include <vector>

namespace bfox_receiver_system {
namespace util {

/// Sleep
void SleepMillisecond(const uint32_t sleep_milliseconds);

/// Split Text
std::vector<std::string> SplitString(const std::string& str, const char delim);

}  // namespace util
}  // namespace bfox_receiver_system

#endif  // BFOX_RECEIVER_MAIN_UTIL_H_
