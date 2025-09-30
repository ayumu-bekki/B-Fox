#ifndef BleRDF_RECEIVER_MAIN_GPTIMER_H_
#define BleRDF_RECEIVER_MAIN_GPTIMER_H_
// ESP32 BleRDF Receiver
// (C)2025 bekki.jp

// Include ----------------------
#include <driver/gptimer.h>

#include "logger.h"

namespace blerdf_receiver_system {

class GPTimer {
 public:
  GPTimer() : gptimer_(nullptr) {}

  ~GPTimer() { Destroy(); }

  void Create(const uint32_t resolution, gptimer_alarm_cb_t function,
              void* const user_data) {
    // Create Timer
    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = resolution,
        .intr_priority = 0,
        .flags{.intr_shared = 1u, .allow_pd = 1u, .backup_before_sleep = 1u}};
    gptimer_new_timer(&timer_config, &gptimer_);

    // SetCallback
    gptimer_event_callbacks_t gptimer_callback = {.on_alarm = function};
    gptimer_register_event_callbacks(gptimer_, &gptimer_callback, user_data);

    // Enable
    gptimer_enable(gptimer_);
  }

  void Destroy() {
    if (gptimer_) {
      gptimer_disable(gptimer_);
      gptimer_del_timer(gptimer_);
      gptimer_ = nullptr;
    }
  }

  void Start(const uint64_t wait_count) const {
    if (!gptimer_) {
      return;
    }

    gptimer_alarm_config_t alarm_config = {.alarm_count = wait_count,
                                           .reload_count = 0ull,
                                           .flags{.auto_reload_on_alarm = 1u}};
    gptimer_set_alarm_action(gptimer_, &alarm_config);
    gptimer_start(gptimer_);
  }

  void Stop() const {
    if (!gptimer_) {
      return;
    }
    gptimer_stop(gptimer_);
  }

 private:
  // Mutable to allow const methods to call ESP-IDF driver functions
  mutable gptimer_handle_t gptimer_;
};

}  // namespace blerdf_receiver_system

#endif  // BleRDF_RECEIVER_MAIN_GPTIMER_H_
