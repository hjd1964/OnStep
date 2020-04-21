#pragma once
#include <U8g2lib.h> // https://github.com/olikraus/u8g2/wiki/u8g2install
#include "u8g2_ext_event.h"
#include "Constants.h"
#include "Locales.h"
#include "Config.h"
#include "Locale.h"
#include "Pinmap.h"

class U8G2_EXT : public U8G2
{
public:
  bool UserInterfaceCatalog(Pad *extPad, const char *title);
  bool UserInterfaceUserCatalog(Pad *extPad, const char *title);
  uint8_t UserInterfaceMessage(Pad *extPad, const char *title1, const char *title2, const char *title3, const char *buttons);
#if DISPLAY_WRAP_MENUS == ON
  uint8_t UserInterfaceSelectionList(Pad *extPad, const char *title, uint8_t start_pos, const char *sl, bool wrap=true);
#else
  uint8_t UserInterfaceSelectionList(Pad *extPad, const char *title, uint8_t start_pos, const char *sl, bool wrap=false);
#endif
  uint8_t UserInterfaceInputValueBoolean(Pad *extPad, const char *title, boolean *value);
  uint8_t UserInterfaceInputValueInteger(Pad *extPad, const char *title, const char *pre, uint8_t *value, uint8_t lo, uint8_t hi, uint8_t digits, const char *post);
  uint8_t UserInterfaceInputValueFloat(Pad *extPad, const char *title, const char *pre, float *value, float lo, float hi, uint8_t len, uint8_t dec, const char *post);
  uint8_t UserInterfaceInputValueDMS(Pad *extPad, const char *title, long *value, long lo, long hi, uint8_t digits1, char* symb1, char* symb2, char* symb3, char* symb_plus, char* symb_minus, bool display_seconds);
  uint8_t UserInterfaceInputValueDate(Pad *extPad, const char *title, uint8_t& year, uint8_t& month, uint8_t& day);
  uint8_t UserInterfaceInputValueRA(Pad *extPad, long *value);
  uint8_t UserInterfaceInputValueDec(Pad *extPad, long *value);
  uint8_t UserInterfaceInputValueTime(Pad *extPad, long *value, bool hrs24);
  uint8_t UserInterfaceInputValueLatitude(Pad *extPad, long *value);
  uint8_t UserInterfaceInputValueLongitude(Pad *extPad, long *value);
  uint8_t DrawFwNumeric(uint8_t x, uint8_t y, const char* text);
  uint8_t GetFwNumericWidth(const char* text);
};

class U8G2_EXT_SH1106_128X64_NONAME_1_HW_I2C : public U8G2_EXT {
public: U8G2_EXT_SH1106_128X64_NONAME_1_HW_I2C(const u8g2_cb_t *rotation, uint8_t reset = U8X8_PIN_NONE, uint8_t clock = U8X8_PIN_NONE, uint8_t data = U8X8_PIN_NONE) : U8G2_EXT() {
  u8g2_Setup_sh1106_i2c_128x64_noname_1(&u8g2, rotation, u8x8_byte_arduino_hw_i2c, u8x8_gpio_and_delay_arduino);
  u8x8_SetPin_HW_I2C(getU8x8(), reset, clock, data);
};
};

class U8G2_EXT_SSD1306_128X64_NONAME_F_HW_I2C : public U8G2_EXT {
public: U8G2_EXT_SSD1306_128X64_NONAME_F_HW_I2C(const u8g2_cb_t *rotation, uint8_t reset = U8X8_PIN_NONE, uint8_t clock = U8X8_PIN_NONE, uint8_t data = U8X8_PIN_NONE) : U8G2_EXT() {
  u8g2_Setup_ssd1306_i2c_128x64_noname_f(&u8g2, rotation, u8x8_byte_arduino_hw_i2c, u8x8_gpio_and_delay_arduino);
  u8x8_SetPin_HW_I2C(getU8x8(), reset, clock, data);
}
};

//U8G2_SSD1306_128X64_NONAME_F_SW_I2C display(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);

// Initializes big 2,3" display from diymore.cc
class U8G2_EXT_SSD1309_128X64_NONAME_F_HW_I2C : public U8G2_EXT {
public: U8G2_EXT_SSD1309_128X64_NONAME_F_HW_I2C(const u8g2_cb_t *rotation, uint8_t reset = DISPLAY_RESET_PIN, uint8_t clock = SCL, uint8_t data = SDA) : U8G2_EXT() { 
  u8g2_Setup_ssd1309_i2c_128x64_noname0_f(&u8g2, rotation, u8x8_byte_arduino_hw_i2c, u8x8_gpio_and_delay_arduino); 
  u8x8_SetPin_HW_I2C(getU8x8(), reset, clock, data);
}
};
