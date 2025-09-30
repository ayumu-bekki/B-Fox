// ESP32 BRDF Beacon
// (C)2025 bekki.jp

// Include ----------------------
#include "task.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

namespace BrdfBeaconSystem {

Task::Task() = default;

Task::Task(const std::string& taskName, const int32_t priority,
           const int coreId)
    : m_Status(TASK_STATUS_READY),
      m_TaskName(taskName),
      m_Priority(priority),
      m_CoreId(coreId) {}

Task::~Task() { Stop(); }

void Task::Start() {
  if (m_Status != TASK_STATUS_READY) {
    return;
  }
  m_Status = TASK_STATUS_RUN;
  xTaskCreatePinnedToCore(this->Listener, m_TaskName.c_str(), TASK_STAC_DEPTH,
                          this, m_Priority, nullptr, m_CoreId);
}

void Task::Stop() {
  if (m_Status != TASK_STATUS_RUN) {
    return;
  }
  m_Status = TASK_STATUS_END;
}

void Task::Run() {
  Initialize();
  while (m_Status == TASK_STATUS_RUN) {
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

}  // namespace BrdfBeaconSystem
