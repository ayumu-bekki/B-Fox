// ESP32 BRDF Beacon
// (C)2025 bekki.jp

#include "brdf_beacon.h"

/// Entry Point
extern "C" void app_main() {
  std::make_shared<brdf_beacon_system::BrdfBeacon>()->Start();
}
