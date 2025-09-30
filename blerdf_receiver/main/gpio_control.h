#ifndef BleRDF_RECEIVER_MAIN_GPIO_CONTROL_H_
#define BleRDF_RECEIVER_MAIN_GPIO_CONTROL_H_
// ESP32 BleRDF Receiver
// (C)2025 bekki.jp

// Include ----------------------
#include <driver/gpio.h>
#include <stdint.h>

namespace blerdf_receiver_system {
namespace gpio {

/// Init GPIO ISR Service
void InitGpioIsrService();

/// Reset GPIO
void Reset(const gpio_num_t gpio_number);

/// Init GPIO (Output)
void InitOutput(const gpio_num_t gpio_number, const bool level = false);

/// Init GPIO (Input:Pulldown inner enable)
void InitInput(const gpio_num_t gpio_number);

/// Set GPIO Level (Output)
void SetLevel(const gpio_num_t gpio_number, const bool level);

/// Gett GPIO Level (Input)
bool GetLevel(const gpio_num_t gpio_number);

/// Get ADC Voltage (Input) [mV]
uint32_t GetAdcVoltage(const int32_t adc_channel_no, const int32_t round = 1);

}  // namespace gpio
}  // namespace blerdf_receiver_system

#endif  // BleRDF_RECEIVER_MAIN_GPIO_CONTROL_H_
