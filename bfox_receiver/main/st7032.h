#ifndef BFOX_RECEIVER_MAIN_ST7032_H_
#define BFOX_RECEIVER_MAIN_ST7032_H_

#include <driver/i2c.h>
#include <esp_log.h>

#include <cstring>
#include <memory>
#include <string>

namespace bfox_receiver_system {

class ST7032 {
 public:
  static constexpr uint8_t kI2cDefaultAddr = 0x3E;

  static constexpr uint8_t kLcdClearDisplay = 0x01;
  static constexpr uint8_t kLcdReturnHome = 0x02;
  static constexpr uint8_t kLcdEntryModeSet = 0x04;
  static constexpr uint8_t kLcdDisplayControl = 0x08;
  static constexpr uint8_t kLcdCursorShift = 0x10;
  static constexpr uint8_t kLcdFunctionSet = 0x20;
  static constexpr uint8_t kLcdSetCGRamAddr = 0x40;
  static constexpr uint8_t kLcdSetDDRRamAddr = 0x80;

  static constexpr uint8_t kLcdExSetbiasosc =
      0x10;  // Bias selection / Internal OSC frequency adjust
  static constexpr uint8_t kLcdExSeticonramaddr = 0x40;  // Set ICON RAM address
  static constexpr uint8_t kLcdExPowiconcontrasth =
      0x50;  // Power / ICON control / Contrast set(high byte)
  static constexpr uint8_t kLcdExFollowercontrol = 0x60;  // Follower control
  static constexpr uint8_t kLcdExContrastsetl = 0x70;  // Contrast set(low byte)

  static constexpr uint8_t kLcdEntryright = 0x00;
  static constexpr uint8_t kLcdEntryleft = 0x02;
  static constexpr uint8_t kLcdEntryshiftincrement = 0x01;
  static constexpr uint8_t kLcdEntryshiftdecrement = 0x00;

  static constexpr uint8_t kLcdDisplayon = 0x04;
  static constexpr uint8_t kLcdDisplayoff = 0x00;
  static constexpr uint8_t kLcdCursoron = 0x02;
  static constexpr uint8_t kLcdCursoroff = 0x00;
  static constexpr uint8_t kLcdBlinkon = 0x01;
  static constexpr uint8_t kLcdBlinkoff = 0x00;

  static constexpr uint8_t kLcd8bitmode = 0x10;
  static constexpr uint8_t kLcd4bitmode = 0x00;
  static constexpr uint8_t kLcd2line = 0x08;
  static constexpr uint8_t kLcd1line = 0x00;
  static constexpr uint8_t kLcd5x10dots = 0x04;
  static constexpr uint8_t kLcd5x8dots = 0x00;
  static constexpr uint8_t kLcdExInstruction =
      0x01;  // IS: instruction table select

  static constexpr uint8_t kLcdBias1_4 = 0x08;  // bias will be 1/4
  static constexpr uint8_t kLcdBias1_5 = 0x00;  // bias will be 1/5

  static constexpr uint8_t kLcdIconOn = 0x08;    // ICON display on
  static constexpr uint8_t kLcdIconOff = 0x00;   // ICON display off
  static constexpr uint8_t kLcdBoostOn = 0x04;   // booster circuit is turn on
  static constexpr uint8_t kLcdBoostOff = 0x00;  // booster circuit is turn off
  static constexpr uint8_t kLcdOsc122hz = 0x00;  // 122Hz@3.0V
  static constexpr uint8_t kLcdOsc131hz = 0x01;  // 131Hz@3.0V
  static constexpr uint8_t kLcdOsc144hz = 0x02;  // 144Hz@3.0V
  static constexpr uint8_t kLcdOsc161hz = 0x03;  // 161Hz@3.0V
  static constexpr uint8_t kLcdOsc183hz = 0x04;  // 183Hz@3.0V
  static constexpr uint8_t kLcdOsc221hz = 0x05;  // 221Hz@3.0V
  static constexpr uint8_t kLcdOsc274hz = 0x06;  // 274Hz@3.0V
  static constexpr uint8_t kLcdOsc347hz = 0x07;  // 347Hz@3.0V

  static constexpr uint8_t kLcdFollowerOn =
      0x08;  // internal follower circuit is turn on
  static constexpr uint8_t kLcdFollowerOff =
      0x00;  // internal follower circuit is turn off
  static constexpr uint8_t kLcdRab1_00 = 0x00;  // 1+(Rb/Ra)=1.00
  static constexpr uint8_t kLcdRab1_25 = 0x01;  // 1+(Rb/Ra)=1.25
  static constexpr uint8_t kLcdRab1_50 = 0x02;  // 1+(Rb/Ra)=1.50
  static constexpr uint8_t kLcdRab1_80 = 0x03;  // 1+(Rb/Ra)=1.80
  static constexpr uint8_t kLcdRab2_00 = 0x04;  // 1+(Rb/Ra)=2.00
  static constexpr uint8_t kLcdRab2_50 = 0x05;  // 1+(Rb/Ra)=2.50
  static constexpr uint8_t kLcdRab3_00 = 0x06;  // 1+(Rb/Ra)=3.00
  static constexpr uint8_t kLcdRab3_75 = 0x07;  // 1+(Rb/Ra)=3.75

 public:
  ST7032();

  void Setup(i2c_port_t i2c_port, const uint8_t address, const uint8_t cols,
             const uint8_t lines, const uint8_t charsize = kLcd5x8dots);

  void SetContrast(uint8_t cont);
  void Clear();
  void SetCursor(uint8_t col, uint8_t row);
  void Print(const std::string& s);
  void Print(const char str[]);
  void Printf(const char* format, ...);
  void Vprintf(const char* format, va_list arg);

  void Write(const uint8_t* buffer, size_t size);
  void Write(const uint8_t value);
  void Command(const uint8_t value);

 private:
  void SetDisplayControl(uint8_t setBit);
  void SetEntryMode(uint8_t setBit);
  void NormalFunctionSet();
  void ExtendFunctionSet();

 private:
  i2c_port_t i2c_port_;
  uint8_t i2c_address_;

  uint8_t display_function_;
  uint8_t display_control_;
  uint8_t display_mode_;

  uint8_t num_lines_;
  uint8_t cur_line_;
};

using ST7032UniquePtr = std::unique_ptr<ST7032>;
using ST7032SharedPtr = std::shared_ptr<ST7032>;

}  // namespace bfox_receiver_system

#endif  // BFOX_RECEIVER_MAIN_ST7032_H_
