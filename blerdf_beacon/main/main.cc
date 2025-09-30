// ESP32 BleRDF Beacon
// (C)2025 bekki.jp

#include "blerdf_beacon.h"

/// Entry Point
extern "C" void app_main() {
  std::make_shared<blerdf_beacon_system::BleRDFBeacon>()->Start();
}
