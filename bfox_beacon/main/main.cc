// ESP32 B-Fox Beacon
// (C)2025 bekki.jp

#include "bfox_beacon.h"

/// Entry Point
extern "C" void app_main() {
  std::make_shared<bfox_beacon_system::BFoxBeacon>()->Start();
}
