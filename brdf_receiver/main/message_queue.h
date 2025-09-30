#ifndef BRDF_RECEIVER_MAIN_MESSAGE_QUEUE_H_
#define BRDF_RECEIVER_MAIN_MESSAGE_QUEUE_H_
// ESP32 BRDF Receiver
// (C)2025 bekki.jp

// Include ----------------------
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

namespace brdf_receiver_system {

template <typename T>
class MessageQueue {
 public:
  MessageQueue() : queue_(nullptr) {}

  virtual ~MessageQueue() { Destroy(); }

  bool Create(const int32_t queueSize = 1) {
    if (queue_) {
      return true;
    }
    queue_ = xQueueCreate(queueSize, sizeof(T));
    if (!queue_) {
      return false;
    }
    return true;
  }

  void Destroy() {
    if (queue_) {
      xQueueReset(queue_);
      vQueueDelete(queue_);
      queue_ = nullptr;
    }
  }

  bool ReceiveWait(T* receive_data, const int32_t max_wait_millisecond) {
    if (!queue_) {
      return false;
    }
    return xQueueReceive(queue_, receive_data,
                         pdMS_TO_TICKS(max_wait_millisecond));
  }

  bool ReceiveNonBlock(T* receive_data) {
    if (!queue_) {
      return false;
    }
    return xQueueReceive(queue_, receive_data, 0);
  }

  bool ReceiveBlock(T* receive_data) {
    if (!queue_) {
      return false;
    }
    return xQueueReceive(queue_, receive_data, portMAX_DELAY);
  }

  bool Send(const T& data) {
    if (!queue_) {
      return false;
    }
    return xQueueSend(queue_, &data, 0) == pdTRUE;
  }

  bool SendFromISR(const T& data) {
    if (!queue_) {
      return false;
    }
    BaseType_t high_task_awoken = pdFALSE;
    xQueueSendFromISR(queue_, &data, &high_task_awoken);
    return (high_task_awoken == pdTRUE);
  }

 private:
  QueueHandle_t queue_;
};

}  // namespace brdf_receiver_system

#endif  // BRDF_RECEIVER_MAIN_MESSAGE_QUEUE_H_
