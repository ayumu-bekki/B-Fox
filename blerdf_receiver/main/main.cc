// ESP32 BleRDF Receiver
// (C)2025 bekki.jp

// Include ----------------------
#include "blerdf_receiver.h"

/// Entry Point
extern "C" void app_main() {
  std::make_shared<blerdf_receiver_system::BleRDFReceiver>()->Start();
}
