// ESP32 BRDF Receiver
// (C)2025 bekki.jp

// Include ----------------------
#include "task.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

namespace brdf_receiver_system {

Task::Task() = default;

Task::Task(const std::string& taskName, const int32_t priority,
           const int coreId)
    : m_Status(kReady),
      m_TaskName(taskName),
      m_Priority(priority),
      m_CoreId(coreId) {}

Task::~Task() { Stop(); }

void Task::Start() {
  if (m_Status != kReady) {
    return;
  }
  m_Status = kRun;
  xTaskCreatePinnedToCore(this->Listener, m_TaskName.c_str(), kTaskStackDepth,
                          this, m_Priority, nullptr, m_CoreId);
}

void Task::Stop() {
  if (m_Status != kRun) {
    return;
  }
  m_Status = kEnd;
}

void Task::Run() {
  Initialize();
  while (m_Status == kRun) {
    Update();
  }
  Stop();
}

void Task::Listener(void* const pParam) {
  if (pParam) {
    static_cast<Task*>(pParam)->Run();
  }
  vTaskDelete(nullptr);
}

}  // namespace brdf_receiver_system
