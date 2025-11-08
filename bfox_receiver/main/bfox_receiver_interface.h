#ifndef BFOX_RECEIVER_MAIN_BFOX_RECEIVER_INTERFACE_H_
#define BFOX_RECEIVER_MAIN_BFOX_RECEIVER_INTERFACE_H_
// ESP32 B-Fox Receiver
// (C)2025 bekki.jp

// Include ----------------------
#include <memory>

namespace bfox_receiver_system {

class BFoxReceiverInterface {
 public:
  virtual ~BFoxReceiverInterface() = default;
};

using BFoxReceiverInterfaceSharedPtr = std::shared_ptr<BFoxReceiverInterface>;
using BFoxReceiverInterfaceConstSharedPtr =
    std::shared_ptr<const BFoxReceiverInterface>;
using BFoxReceiverInterfaceWeakPtr = std::weak_ptr<BFoxReceiverInterface>;
using BFoxReceiverInterfaceConstWeakPtr =
    std::weak_ptr<const BFoxReceiverInterface>;

}  // namespace bfox_receiver_system

#endif  // BFOX_RECEIVER_MAIN_BFOX_RECEIVER_INTERFACE_H_
