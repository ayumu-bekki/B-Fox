// ESP32 BRDF Beacon
// (C)2025 bekki.jp

#include "task.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

namespace brdf_beacon_system {

Task::Task() = default;

Task::Task(const std::string& task_name, const int32_t priority,
           const int core_id)
    : status_(kReady),
      task_name_(task_name),
      priority_(priority),
      core_id_(core_id) {}

Task::~Task() { Stop(); }

void Task::Start() {
  if (status_ != kReady) {
    return;
  }
  status_ = kRun;
  xTaskCreatePinnedToCore(this->Listener, task_name_.c_str(), kTaskStackDepth,
                          this, priority_, nullptr, core_id_);
}

void Task::Stop() {
  if (status_ != kRun) {
    return;
  }
  status_ = kEnd;
}

void Task::Run() {
  Initialize();
  while (status_ == kRun) {
    Update();
  }
  Stop();
}

void Task::Listener(void* const param) {
  if (param) {
    static_cast<Task*>(param)->Run();
  }
  vTaskDelete(nullptr);
}

}  // namespace brdf_beacon_system
