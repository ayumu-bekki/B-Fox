#ifndef BFOX_BEACON_MAIN_MESSAGE_QUEUE_H_
#define BFOX__BEACON_MAIN_MESSAGE_QUEUE_H_
// ESP32 B-Fox Beacon
// (C)2025 bekki.jp

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

namespace bfox_beacon_system {

template <typename T>
class MessageQueue {
 public:
  MessageQueue() : queue_(nullptr) {}

  virtual ~MessageQueue() { Destroy(); }

  bool Create(const int32_t queue_size = 1) {
    if (queue_) {
      return true;
    }
    queue_ = xQueueCreate(queue_size, sizeof(T));
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

  bool ReceiveWait(T* const receive_data, const int32_t max_wait_millisecond) {
    if (!queue_) {
      return false;
    }
    return xQueueReceive(queue_, receive_data,
                         pdMS_TO_TICKS(max_wait_millisecond));
  }

  bool ReceiveNonBlock(T* const receive_data) {
    if (!queue_) {
      return false;
    }
    return xQueueReceive(queue_, receive_data, 0);
  }

  bool ReceiveBlock(T* const receive_data) {
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

}  // namespace bfox_beacon_system

#endif  // BFOX_BEACON_MAIN_MESSAGE_QUEUE_H_
