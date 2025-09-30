// ESP32 BleRDF Receiver
// (C)2025 bekki.jp

// Include ----------------------
#include "task.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "logger.h"

namespace blerdf_receiver_system {

Task::Task() = default;

Task::Task(const std::string& taskName, const int32_t priority,
           const int coreId)
    : status_(TaskStatus::kReady),
      task_name_(taskName),
      priority_(priority),
      core_id_(coreId) {}

Task::~Task() { Stop(); }

void Task::Start() {
  if (status_ != TaskStatus::kReady) {
    return;
  }
  status_ = TaskStatus::kRun;
  xTaskCreatePinnedToCore(this->Listener, task_name_.c_str(), kTaskStackDepth,
                          this, priority_, nullptr, core_id_);
}

void Task::Stop() {
  if (status_ != TaskStatus::kRun) {
    return;
  }
  status_ = TaskStatus::kEnd;
}

void Task::Run() {
  Initialize();
  while (status_ == TaskStatus::kRun) {
    Update();
  }
  Stop();
}

void Task::Listener(void* const pParam) {
  if (pParam) {
    static_cast<Task*>(pParam)->Run();
  } else {
    ESP_LOGE("Task", "Listener called with null parameter");
  }
  vTaskDelete(nullptr);
}

}  // namespace blerdf_receiver_system
