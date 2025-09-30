#ifndef brdf_receiver_INTERFACE_H_
#define brdf_receiver_INTERFACE_H_
// ESP32 BRDF Receiver
// (C)2025 bekki.jp

// Include ----------------------
#include <chrono>
#include <memory>

#include "st7032.h"

namespace brdf_receiver_system {

class BrdfReceiverInterface {
 public:
  virtual ~BrdfReceiverInterface() = default;
};

using BrdfReceiverInterfaceSharedPtr = std::shared_ptr<BrdfReceiverInterface>;
using BrdfReceiverInterfaceConstSharedPtr =
    std::shared_ptr<const BrdfReceiverInterface>;
using BrdfReceiverInterfaceWeakPtr = std::weak_ptr<BrdfReceiverInterface>;
using BrdfReceiverInterfaceConstWeakPtr =
    std::weak_ptr<const BrdfReceiverInterface>;

}  // namespace brdf_receiver_system

#endif  // brdf_receiver_INTERFACE_H_
