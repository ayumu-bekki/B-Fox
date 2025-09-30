#ifndef BRDF_BEACON_MAIN_VOLTAGE_CHECK_TASK_H_
#define BRDF_BEACON_MAIN_VOLTAGE_CHECK_TASK_H_
// ESP32 BRDF Beacon
// (C)2025 bekki.jp

#include <soc/soc.h>

#include <memory>

#include "task.h"

namespace brdf_beacon_system {

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

}  // namespace brdf_beacon_system

#endif  // BRDF_BEACON_MAIN_VOLTAGE_CHECK_TASK_H_
