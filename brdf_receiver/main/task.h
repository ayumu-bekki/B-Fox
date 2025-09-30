#ifndef TASK_H_
#define TASK_H_
// ESP32 BRDF Receiver
// (C)2025 bekki.jp

// Include ----------------------
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <string>

namespace brdf_receiver_system {

/// FreeRTOS xTask Wrap
class Task {
 public:
  enum TaskStatus {
    kReady,
    kRun,
    kEnd,
  };

  static constexpr int32_t kTaskStackDepth = 8192;

  /// Task Priority
  static constexpr int32_t kPriorityTop = (configMAX_PRIORITIES)-1;
  static constexpr int32_t kPriorityLow = 0;
  static constexpr int32_t kPriorityNormal = kPriorityTop - 4;
  static constexpr int32_t kPriorityHigh = kPriorityTop - 3;

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

}  // namespace brdf_receiver_system

#endif  // TASK_H_
