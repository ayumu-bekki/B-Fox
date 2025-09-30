// ESP32 BRDF Receiver
// (C)2025 bekki.jp

// Include ----------------------
#include "brdf_receiver.h"

/// Entry Point
extern "C" void app_main() {
  std::make_shared<brdf_receiver_system::BrdfReceiver>()->Start();
}
