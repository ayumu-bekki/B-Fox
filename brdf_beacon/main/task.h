#ifndef TASK_H_
#define TASK_H_
// ESP32 BRDF Beacon
// (C)2025 bekki.jp

// Include ----------------------
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <string>

namespace BrdfBeaconSystem {

/// FreeRTOS xTask Wrap
class Task {
 public:
  enum TaskStatus {
    TASK_STATUS_READY,
    TASK_STATUS_RUN,
    TASK_STATUS_END,
  };

  static constexpr int32_t TASK_STAC_DEPTH = 8192;

  /// Task Priority
  static constexpr int32_t PRIORITY_TOP = (configMAX_PRIORITIES)-1;
  static constexpr int32_t PRIORITY_LOW = 0;
  static constexpr int32_t PRIORITY_NORMAL = PRIORITY_TOP - 4;
  static constexpr int32_t PRIORITY_HIGH = PRIORITY_TOP - 3;

 private:
  Task();

 public:
  Task(const std::string& taskName, const int32_t priority, const int coreId);
  virtual ~Task();

  /// Start Task
  void Start();

  /// Stop Task
  void Stop();

  /// Initialize (Called when the Start function is executed.)
  virtual void Initialize() {}

  /// (override) sub class processing
  virtual void Update() = 0;

 public:
  /// Task Running
  void Run();

  /// Task Listener
  static void Listener(void* const pParam);

 protected:
  /// Task Status
  TaskStatus m_Status;

  /// Task Name
  std::string m_TaskName;

  /// Task Priority
  int32_t m_Priority;

  /// Use Core Id
  int32_t m_CoreId;
};

}  // namespace BrdfBeaconSystem

#endif  // TASK_H_
