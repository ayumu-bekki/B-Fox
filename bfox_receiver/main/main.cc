// ESP32 B-Fox Receiver
// (C)2025 bekki.jp

// Include ----------------------
#include "bfox_receiver.h"

/// Entry Point
extern "C" void app_main() {
  std::make_shared<bfox_receiver_system::BFoxReceiver>()->Start();
}
