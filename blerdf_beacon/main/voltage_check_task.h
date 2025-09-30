#ifndef BleRDF_BEACON_MAIN_VOLTAGE_CHECK_TASK_H_
#define BleRDF_BEACON_MAIN_VOLTAGE_CHECK_TASK_H_
// ESP32 BleRDF Beacon
// (C)2025 bekki.jp

#include <soc/soc.h>

#include <memory>

#include "task.h"

namespace blerdf_beacon_system {

class VoltageCheckTask final : public Task {
 public:
  static constexpr const char kTaskName[] = "VoltageCheckTask";
  static constexpr int kPriority = Task::kPriorityLow;
  static constexpr int kCoreId = tskNO_AFFINITY;

 public:
  VoltageCheckTask();

  void Initialize() override;

  void Update() override;

  float GetVoltage() const;

 private:
  float voltage_;
};

using VoltageCheckTaskUniquePtr = std::unique_ptr<VoltageCheckTask>;

}  // namespace blerdf_beacon_system

#endif  // BleRDF_BEACON_MAIN_VOLTAGE_CHECK_TASK_H_
