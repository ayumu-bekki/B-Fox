#ifndef BFOX_BEACON_MAIN_TASK_H_
#define BFOX__BEACON_MAIN_TASK_H_
// ESP32 B-Fox Beacon
// (C)2025 bekki.jp

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <string>

namespace bfox_beacon_system {

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
  Task(const std::string& task_name, const int32_t priority, const int core_id);
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
  static void Listener(void* const param);

 protected:
  /// Task Status
  TaskStatus status_;

  /// Task Name
  std::string task_name_;

  /// Task Priority
  int32_t priority_;

  /// Use Core Id
  int32_t core_id_;
};

}  // namespace bfox_beacon_system

#endif  // BFOX_BEACON_MAIN_TASK_H_
