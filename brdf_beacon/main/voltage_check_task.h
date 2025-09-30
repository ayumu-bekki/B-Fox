#ifndef VOLTAGE_CHECKER_TASK_H_
#define VOLTAGE_CHECKER_TASK_H_
// ESP32 BRDF Beacon
// (C)2025 bekki.jp

// Include ----------------------
#include <soc/soc.h>

#include <memory>

#include "task.h"

namespace BrdfBeaconSystem {

class VoltageCheckTask final : public Task {
 public:
  static constexpr char *const TASK_NAME = (char *)"VoltageCheckTask";
  static constexpr int PRIORITY = Task::PRIORITY_LOW;
  static constexpr int CORE_ID = tskNO_AFFINITY;

 public:
  VoltageCheckTask();

  void Initialize() override;

  void Update() override;

  float GetVoltage() const;

 private:
  float voltage_;
};

using VoltageCheckTaskUniquePtr = std::unique_ptr<VoltageCheckTask>;

}  // namespace BrdfBeaconSystem

#endif  // VOLTAGE_CHECKER_TASK_H_
