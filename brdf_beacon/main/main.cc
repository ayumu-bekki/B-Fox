// ESP32 BRDF Beacon
// (C)2025 bekki.jp

// Include ----------------------
#include "brdf_beacon.h"

/// Entry Point
extern "C" void app_main() {
  std::make_shared<BrdfBeaconSystem::BrdfBeacon>()->Start();
}
