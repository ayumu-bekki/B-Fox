#ifndef GPIO_H_
#define GPIO_H_
// ESP32 BRDF Beacon
// (C)2025 bekki.jp

// Include ----------------------
#include <driver/gpio.h>
#include <stdint.h>

namespace BrdfBeaconSystem::GPIO {

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

}  // namespace BrdfBeaconSystem::GPIO

#endif  // GPIO_H_
