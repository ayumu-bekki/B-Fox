// ESP32 BRDF Receiver
// (C)2025 bekki.jp

// Include ----------------------
#include "gpio_control.h"

#include <driver/gpio.h>
#include <esp_adc/adc_cali.h>
#include <esp_adc/adc_cali_scheme.h>
#include <esp_adc/adc_oneshot.h>

#include "logger.h"

namespace brdf_receiver_system {
namespace gpio {

/// Init GPIO ISR Service
void InitGpioIsrService() { gpio_install_isr_service(0); }

/// Reset GPIO
void Reset(const gpio_num_t gpio_number) { gpio_reset_pin(gpio_number); }

/// Init GPIO (Output)
void InitOutput(const gpio_num_t gpio_number, const bool level) {
  gpio_config_t io_conf_dir = {
      .pin_bit_mask = 1ull << gpio_number,
      .mode = GPIO_MODE_OUTPUT,
      .pull_up_en = GPIO_PULLUP_DISABLE,
      .pull_down_en = GPIO_PULLDOWN_DISABLE,
      .intr_type = GPIO_INTR_DISABLE,
  };
  gpio_config(&io_conf_dir);

  SetLevel(gpio_number, level);
}

/// Init GPIO (Input:Pulldown inner enable)
void InitInput(const gpio_num_t gpio_number) {
  // Input
  gpio_config_t io_input_conf = {
      .pin_bit_mask = 1ull << gpio_number,
      .mode = GPIO_MODE_INPUT,
      .pull_up_en = GPIO_PULLUP_ENABLE,  // IO34-IO39 have no internal
                                         // pull-up/pull-down resistors
      .pull_down_en = GPIO_PULLDOWN_DISABLE,
      .intr_type = GPIO_INTR_ANYEDGE,
  };
  gpio_config(&io_input_conf);
}

/// Set GPIO Level (Output)
void SetLevel(const gpio_num_t gpio_number, const bool level) {
  gpio_set_level(gpio_number, level);
}

/// Gett GPIO Level (Input)
bool GetLevel(const gpio_num_t gpio_number) {
  return gpio_get_level(gpio_number) != 0;
}

/// Get ADC Voltage (Input) [mV]
uint32_t GetAdcVoltage(const int32_t adc_channel_no, const int32_t round) {
  adc_oneshot_unit_handle_t adc_handle;
  adc_oneshot_unit_init_cfg_t adcInitConfig = {
      .unit_id = ADC_UNIT_1,
      .clk_src = static_cast<adc_oneshot_clk_src_t>(ADC_DIGI_CLK_SRC_DEFAULT),
      .ulp_mode = ADC_ULP_MODE_DISABLE,
  };
  adc_oneshot_new_unit(&adcInitConfig, &adc_handle);

  adc_oneshot_chan_cfg_t adc_config = {
      .atten = ADC_ATTEN_DB_12,
      .bitwidth = ADC_BITWIDTH_12,
  };
  adc_oneshot_config_channel(
      adc_handle, static_cast<adc_channel_t>(adc_channel_no), &adc_config);

  adc_cali_handle_t adc_cali_handle = nullptr;
  adc_cali_curve_fitting_config_t cali_config = {
      .unit_id = ADC_UNIT_1,
      .chan = static_cast<adc_channel_t>(adc_channel_no),
      .atten = ADC_ATTEN_DB_12,
      .bitwidth = ADC_BITWIDTH_DEFAULT,
  };
  esp_err_t ret =
      adc_cali_create_scheme_curve_fitting(&cali_config, &adc_cali_handle);
  if (ret != ESP_OK) {
    adc_oneshot_del_unit(adc_handle);
    return 0;
  }

  uint32_t sum_voltage = 0;
  for (int32_t i = 0; i < round; ++i) {
    int32_t adcValue = 0;
    adc_oneshot_read(adc_handle, static_cast<adc_channel_t>(adc_channel_no),
                     reinterpret_cast<int*>(&adcValue));
    int32_t adcVoltage = 0;
    adc_cali_raw_to_voltage(adc_cali_handle, adcValue,
                            reinterpret_cast<int*>(&adcVoltage));
    sum_voltage += adcVoltage;
  }

  // Shutdown
  adc_oneshot_del_unit(adc_handle);
  adc_cali_delete_scheme_curve_fitting(adc_cali_handle);

  return sum_voltage / round;
}

}  // namespace gpio
}  // namespace brdf_receiver_system
