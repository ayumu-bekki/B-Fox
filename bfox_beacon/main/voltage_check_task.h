#ifndef BFOX_BEACON_MAIN_VOLTAGE_CHECK_TASK_H_
#define BFOX__BEACON_MAIN_VOLTAGE_CHECK_TASK_H_
// ESP32 B-Fox Beacon
// (C)2025 bekki.jp

#include <soc/soc.h>

#include <memory>

#include "task.h"

namespace bfox_beacon_system {

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

}  // namespace bfox_beacon_system

#endif  // BFOX_BEACON_MAIN_VOLTAGE_CHECK_TASK_H_
