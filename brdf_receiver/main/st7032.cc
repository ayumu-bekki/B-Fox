#include "st7032.h"

#include <memory>

#include <rom/ets_sys.h>

namespace brdf_receiver_system {

ST7032::ST7032()
    : i2c_port_(),
      i2c_address_(),
      display_function_(),
      display_control_(),
      display_mode_(),
      num_lines_(),
      cur_line_() {}

// Initialize
void ST7032::Setup(i2c_port_t i2c_port, const uint8_t address,
                   const uint8_t cols, const uint8_t lines,
                   const uint8_t charsize) {
  i2c_port_ = i2c_port;
  i2c_address_ = address;

  display_function_ = kLcd8bitmode | kLcd1line | kLcd5x8dots;

  if (lines > 1) {
    display_function_ |= kLcd2line;
  }
  num_lines_ = lines;
  cur_line_ = 0;

  // for some 1 line displays you can select a 10 pixel high font
  if ((charsize != 0) && (lines == 1)) {
    display_function_ |= kLcd5x10dots;
  }

  // Wait for power on
  vTaskDelay(40 / portTICK_PERIOD_MS);

  // finally, set # lines, font size, etc.
  NormalFunctionSet();

  ExtendFunctionSet();
  Command(kLcdExSetbiasosc | kLcdBias1_5 | kLcdOsc183hz);
  Command(kLcdExFollowercontrol | kLcdFollowerOn | kLcdRab2_00);
  vTaskDelay(200 / portTICK_PERIOD_MS); // 200ms
  NormalFunctionSet();

  // turn the display on with no cursor or blinking default
  display_control_ = 0x00;  // LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
  SetDisplayControl(kLcdDisplayon | kLcdCursoroff | kLcdBlinkoff);

  Clear();

  // Initialize to default text direction (for romance languages)
  display_mode_ = 0x00;  // LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
  SetEntryMode(kLcdEntryleft | kLcdEntryshiftdecrement);
}

void ST7032::SetContrast(uint8_t cont) {
  ExtendFunctionSet();
  Command(kLcdExContrastsetl | (cont & 0x0f));
  Command(kLcdExPowiconcontrasth | kLcdIconOn | kLcdBoostOn |
          ((cont >> 4) & 0x03));
  NormalFunctionSet();
}

void ST7032::Clear() {
  Command(kLcdClearDisplay);
  ets_delay_us(2000);
}


void ST7032::SetCursor(uint8_t col, uint8_t row) {
  const int row_offsets[] = {0x00, 0x40, 0x14, 0x54};
  if (row > num_lines_) {
    row = num_lines_ - 1;
  }
  Command(kLcdSetDDRRamAddr | (col + row_offsets[row]));
}

void ST7032::Print(const std::string &s) {
  Write(reinterpret_cast<const uint8_t *>(s.c_str()), s.length());
}

void ST7032::Print(const char str[]) {
  const size_t len = strlen(str);
  return Write(reinterpret_cast<const uint8_t *>(str), len);
}

void ST7032::Printf(const char *format, ...) {
  va_list arg;
  va_start(arg, format);
  Vprintf(format, arg);
  va_end(arg);
}

void ST7032::Vprintf(const char *format, va_list arg) {
  constexpr size_t STACK_BUF_SIZE = 64;
  char stack_buf[STACK_BUF_SIZE] = {};
    
  va_list copy;
  va_copy(copy, arg);
  const int len = vsnprintf(stack_buf, STACK_BUF_SIZE, format, copy);
  va_end(copy);

  if (len < 0) {
    return;
  }

  if (len < STACK_BUF_SIZE) {
    // If the stack buffer is sufficient
    Write(reinterpret_cast<uint8_t*>(stack_buf), len);
  } else {
    // If dynamic allocation is required
    auto heap_buf = std::make_unique<char[]>(len + 1);
    vsnprintf(heap_buf.get(), len + 1, format, arg);
    Write(reinterpret_cast<uint8_t*>(heap_buf.get()), len);
  }
}


void ST7032::Command(const uint8_t value) {
  i2c_cmd_handle_t cmd = i2c_cmd_link_create();
  i2c_master_start(cmd);

  i2c_master_write_byte(cmd, i2c_address_ << 1 | I2C_MASTER_WRITE, true);
  i2c_master_write_byte(cmd, 0x00, true);
  i2c_master_write_byte(cmd, value, true);

  i2c_master_stop(cmd);

  i2c_master_cmd_begin(i2c_port_, cmd, 1000 / portTICK_PERIOD_MS);
  i2c_cmd_link_delete(cmd);

  ets_delay_us(27);  // 26.3us
}

void ST7032::Write(const uint8_t *buffer, size_t size) {
  while (size--) {
    Write(*buffer++);
  }
}

void ST7032::Write(const uint8_t value) {
  i2c_cmd_handle_t cmd = i2c_cmd_link_create();
  i2c_master_start(cmd);

  i2c_master_write_byte(cmd, i2c_address_ << 1 | I2C_MASTER_WRITE, true);
  i2c_master_write_byte(cmd, 0x40, true);
  i2c_master_write_byte(cmd, value, true);

  i2c_master_stop(cmd);

  i2c_master_cmd_begin(i2c_port_, cmd, 1000 / portTICK_PERIOD_MS);
  i2c_cmd_link_delete(cmd);

  ets_delay_us(27);  // 26.3us
}

void ST7032::SetDisplayControl(uint8_t setBit) {
  display_control_ |= setBit;
  Command(kLcdDisplayControl | display_control_);
}

void ST7032::SetEntryMode(uint8_t setBit) {
  display_mode_ |= setBit;
  Command(kLcdEntryModeSet | display_mode_);
}

void ST7032::NormalFunctionSet() {
  Command(kLcdFunctionSet | display_function_);
}

void ST7032::ExtendFunctionSet() {
  Command(kLcdFunctionSet | display_function_ | kLcdExInstruction);
}


}  // namespace brdf_receiver_system
