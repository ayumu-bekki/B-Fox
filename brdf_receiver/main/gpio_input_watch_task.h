#ifndef GPIO_INPUT_WATCH_TASK_H_
#define GPIO_INPUT_WATCH_TASK_H_
// (C)2024 bekki.jp
// GPIO input watch task

// Include ----------------------
#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>

#include <functional>
#include <vector>

#include "gptimer.h"
#include "logger.h"
#include "message_queue.h"
#include "task.h"

namespace brdf_receiver_system {

class GpioInputWatchTask final : public Task {
 public:
  static constexpr std::string_view kTaskName = "GpioInputWatchTask";
  static constexpr int32_t kPriority = Task::kPriorityLow;
  static constexpr int32_t kCoreId = PRO_CPU_NUM;

 private:
  static constexpr int32_t kEdgeCounter = 3;
  static constexpr int32_t kLongEdgeCounter = 100;

 public:
  enum GpioPullUpDown {
    kNone,
    kPullUpResistorEnable,
    kPullDownResistorEnable,
  };

  class GpioInfo {
   public:
    enum Status : int32_t {
      kStatusDisable,
      kStatusEnable,
      kStatusEnableLong,
    };

   public:
    GpioInfo(gpio_num_t gpio_no, std::function<void()> on_up,
             std::function<void()> on_long_up)
        : gpio_no_(gpio_no),
          on_up_(on_up),
          on_long_up_(on_long_up),
          input_counter_(0),
          status_(kStatusDisable) {}

    gpio_num_t GetGpioNo() const { return gpio_no_; }

    void Check() {
      if (status_ == kStatusDisable) {
        if (gpio_get_level(gpio_no_) == 0) {
          if (input_counter_ < 0) {
            input_counter_ = 0;
          }
          ++input_counter_;
        } else {
          if (kEdgeCounter <= input_counter_) {
            status_ = kStatusEnable;
            if (on_up_) {
              on_up_();
            }
          }
          input_counter_ = 0;
        }
        if (kLongEdgeCounter <= input_counter_) {
          input_counter_ = 0;
          status_ = kStatusEnableLong;
          if (on_long_up_) {
            on_long_up_();
          }
        }
      } else {
        if (gpio_get_level(gpio_no_) != 0) {
          status_ = kStatusDisable;
        }
      }
    }

   private:
    gpio_num_t gpio_no_;
    std::function<void()> on_up_;
    std::function<void()> on_long_up_;
    int input_counter_;
    Status status_;
  };

 public:
  GpioInputWatchTask()
      : Task(std::string(kTaskName).c_str(), kPriority, kCoreId),
        message_queue_(),
        gptimer_(),
        gpio_list_() {
    // Create MessageQueue
    constexpr int32_t MESSAGE_QUEUE_SIZE = 10;
    if (!message_queue_.Create(MESSAGE_QUEUE_SIZE)) {
      ESP_LOGE(kTag, "Creating queue failed");
    }

    // Create Timer
    constexpr uint32_t GPIO_WATCH_RESOLUTION = 1000000u;  // 1us
    gptimer_.Create(GPIO_WATCH_RESOLUTION, &GpioInputWatchTask::TimerCallback,
                    &message_queue_);
  }

  ~GpioInputWatchTask() {
    gptimer_.Destroy();
    message_queue_.Destroy();
  }

  void Initialize() override {
    ESP_LOGI(kTag, "Start GpioInputWatchTask");
    gpio_install_isr_service(0);
  }

  void Update() override {
    gptimer_.Start(5000);  // 5ms
    int event_type = 0;
    while (true) {
      constexpr int32_t QUEUE_RECEIVE_LIMIT =
          1000 / portTICK_PERIOD_MS;  // 最大待機tick
      if (message_queue_.ReceiveWait(&event_type, QUEUE_RECEIVE_LIMIT)) {
        if (event_type == 1) {
          for (auto&& gpio_info : gpio_list_) {
            gpio_info.Check();
          }
        }
      }
    }
  }

  void AddMonitor(GpioInfo gpio_info,
                  GpioPullUpDown gpio_pullupdown = GpioPullUpDown::kNone) {
    // Setting GPIO Input
    // 注意:IO34～IO39は内部プルアップ/プルダウン抵抗は無し
    gpio_config_t io_input_conf = {
        .pin_bit_mask = 1ull << gpio_info.GetGpioNo(),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en =
            (gpio_pullupdown == kPullUpResistorEnable ? GPIO_PULLUP_ENABLE
                                                      : GPIO_PULLUP_DISABLE),
        .pull_down_en = (gpio_pullupdown == kPullDownResistorEnable
                             ? GPIO_PULLDOWN_ENABLE
                             : GPIO_PULLDOWN_DISABLE),
        .intr_type = GPIO_INTR_ANYEDGE,
    };
    gpio_config(&io_input_conf);

    gpio_list_.emplace_back(std::move(gpio_info));
  }

 private:
  static bool TimerCallback(gptimer_handle_t timer,
                            const gptimer_alarm_event_data_t* event_data,
                            void* message_queue) {
    MessageQueue<int>* const queue =
        static_cast<MessageQueue<int>*>(message_queue);
    return queue->SendFromISR(1);
  }

 private:
  MessageQueue<int> message_queue_;
  GPTimer gptimer_;
  std::vector<GpioInfo> gpio_list_;
};

}  // namespace brdf_receiver_system

#endif  // GPIO_INPUT_WATCH_TASK_H_

// EOF
