#ifndef BleRDF_RECEIVER_MAIN_BleRDF_RECEIVER_INTERFACE_H_
#define BleRDF_RECEIVER_MAIN_BleRDF_RECEIVER_INTERFACE_H_
// ESP32 BleRDF Receiver
// (C)2025 bekki.jp

// Include ----------------------
#include <memory>

namespace blerdf_receiver_system {

class BleRDFReceiverInterface {
 public:
  virtual ~BleRDFReceiverInterface() = default;
};

using BleRDFReceiverInterfaceSharedPtr = std::shared_ptr<BleRDFReceiverInterface>;
using BleRDFReceiverInterfaceConstSharedPtr =
    std::shared_ptr<const BleRDFReceiverInterface>;
using BleRDFReceiverInterfaceWeakPtr = std::weak_ptr<BleRDFReceiverInterface>;
using BleRDFReceiverInterfaceConstWeakPtr =
    std::weak_ptr<const BleRDFReceiverInterface>;

}  // namespace blerdf_receiver_system

#endif  // BleRDF_RECEIVER_MAIN_BleRDF_RECEIVER_INTERFACE_H_
