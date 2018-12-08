
#include "_EEPROM_ext.h"
#include "SmartController.h"
#include "LX200.h"

#define MY_BORDER_SIZE 1
#define icon_width 16
#define icon_height 16

#define teenastro_width 128
#define teenastro_height 68

static unsigned char wifi_bits[] U8X8_PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x80, 0x20, 0x40, 0x4e, 0x00, 0x11, 0x00, 0x04, 0x00, 0x04, 0x00, 0x04, 0xfe, 0x7f, 0x02, 0x40, 0xda, 0x5f, 0xda, 0x5f, 0x02, 0x40, 0xfe, 0x7f, 0x00, 0x00 };

static unsigned char align1_bits[] U8X8_PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x1c, 0x00, 0x1c, 0x00, 0x00, 0x00, 0xc0, 0x03, 0xc0, 0x00, 0x40, 0x01, 0x50, 0x02, 0x18, 0x04, 0x10, 0x08, 0x10, 0x10, 0x10, 0x20, 0x00, 0x00, 0x00, 0x00 };

static unsigned char align2_bits[] U8X8_PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x1c, 0x00, 0x1c, 0x00, 0x00, 0x00, 0xc0, 0x03, 0xc0, 0x00, 0x40, 0x01, 0x48, 0x02, 0x14, 0x04, 0x10, 0x08, 0x08, 0x10, 0x04, 0x20, 0x1c, 0x00, 0x00, 0x00 };

static unsigned char align3_bits[] U8X8_PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x1c, 0x00, 0x1c, 0x00, 0x00, 0x00, 0xc0, 0x03, 0xc0, 0x00, 0x40, 0x01, 0x40, 0x02, 0x1c, 0x04, 0x10, 0x08, 0x18, 0x10, 0x10, 0x20, 0x1c, 0x00, 0x00, 0x00 };

static unsigned char align4_bits[] U8X8_PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x1c, 0x00, 0x1c, 0x00, 0x00, 0x00, 0xc0, 0x03, 0xc0, 0x00, 0x40, 0x01, 0x40, 0x02, 0x14, 0x04, 0x14, 0x08, 0x1c, 0x10, 0x10, 0x20, 0x10, 0x00, 0x00, 0x00 };

static unsigned char align5_bits[] U8X8_PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x1c, 0x00, 0x1c, 0x00, 0x00, 0x00, 0xc0, 0x03, 0xc0, 0x00, 0x40, 0x01, 0x40, 0x02, 0x1c, 0x04, 0x04, 0x08, 0x1c, 0x10, 0x10, 0x20, 0x1c, 0x00, 0x00, 0x00 };

static unsigned char align6_bits[] U8X8_PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x1c, 0x00, 0x1c, 0x00, 0x00, 0x00, 0xc0, 0x03, 0xc0, 0x00, 0x40, 0x01, 0x40, 0x02, 0x1c, 0x04, 0x04, 0x08, 0x1c, 0x10, 0x14, 0x20, 0x1c, 0x00, 0x00, 0x00 };

static unsigned char align7_bits[] U8X8_PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x1c, 0x00, 0x1c, 0x00, 0x00, 0x00, 0xc0, 0x03, 0xc0, 0x00, 0x40, 0x01, 0x40, 0x02, 0x1c, 0x04, 0x10, 0x08, 0x08, 0x10, 0x08, 0x20, 0x04, 0x00, 0x00, 0x00 };

static unsigned char align8_bits[] U8X8_PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x1c, 0x00, 0x1c, 0x00, 0x00, 0x00, 0xc0, 0x03, 0xc0, 0x00, 0x40, 0x01, 0x40, 0x02, 0x1c, 0x04, 0x14, 0x08, 0x1c, 0x10, 0x14, 0x20, 0x1c, 0x00, 0x00, 0x00 };

static unsigned char align9_bits[] U8X8_PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x1c, 0x00, 0x1c, 0x00, 0x00, 0x00, 0xc0, 0x03, 0xc0, 0x00, 0x40, 0x01, 0x40, 0x02, 0x1c, 0x04, 0x14, 0x08, 0x1c, 0x10, 0x10, 0x20, 0x10, 0x00, 0x00, 0x00 };

static unsigned char home_bits[] U8X8_PROGMEM = {
  0x00, 0x02, 0x00, 0x07, 0x80, 0x0f, 0xc0, 0x1f, 0x80, 0x3f, 0x00, 0x7f, 0x00, 0x7e, 0x00, 0x7f, 0x80, 0xfb, 0xc0, 0xc1, 0xe0, 0x01, 0xbc, 0x49, 0x9e, 0x49, 0x9e, 0x79, 0x8c, 0x49, 0x80, 0x49 };

static unsigned char parked_bits[] U8X8_PROGMEM = {
  0x00, 0x00, 0xfe, 0x7f, 0x02, 0x40, 0x02, 0x40, 0xe2, 0x43, 0x62, 0x46, 0x62, 0x46, 0x62, 0x46, 0xe2, 0x43, 0x62, 0x40, 0x62, 0x40, 0x62, 0x40, 0x62, 0x40, 0x02, 0x40, 0xfe, 0x7f, 0x00, 0x00 };

static unsigned char parking_bits[] U8X8_PROGMEM = {
  0xff, 0xff, 0x01, 0x80, 0x01, 0x80, 0xf9, 0x80, 0x99, 0x81, 0x99, 0x81, 0x99, 0x81, 0xf9, 0x80, 0x19, 0x80, 0x99, 0x84, 0x99, 0x8d, 0x99, 0x9f, 0x81, 0x8d, 0x81, 0x84, 0x01, 0x80, 0xff, 0xff };

static unsigned char parkingFailed_bits[] U8X8_PROGMEM = {
  0xff, 0xff, 0x01, 0x80, 0x01, 0x80, 0xf9, 0x90, 0x99, 0x91, 0x99, 0x91, 0x99, 0x91, 0xf9, 0x90, 0x19, 0x90, 0xd9, 0x93, 0x59, 0x90, 0xd9, 0x91, 0x41, 0x80, 0x41, 0x90, 0x01, 0x80, 0xff, 0xff };

static unsigned char guiding_bits[] U8X8_PROGMEM = {
  0x00, 0x00, 0x80, 0x01, 0x80, 0x01, 0xc0, 0x03, 0x20, 0x04, 0x10, 0x08, 0x08, 0x10, 0x8e, 0x71, 0x8e, 0x71, 0x08, 0x10, 0x10, 0x08, 0x20, 0x04, 0xc0, 0x03, 0x80, 0x01, 0x80, 0x01, 0x00, 0x00 };

static unsigned char no_tracking_bits[] U8X8_PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x38, 0x1c, 0x7c, 0x3e, 0x7c, 0x3e, 0x7c, 0x3e, 0x7c, 0x3e, 0x7c, 0x3e, 0x7c, 0x3e, 0x7c, 0x3e, 0x7c, 0x3e, 0x7c, 0x3e, 0x7c, 0x3e, 0x7c, 0x3e, 0x38, 0x1c, 0x00, 0x00 };

static unsigned char tracking_bits[] U8X8_PROGMEM = {
  0x00, 0x00, 0x02, 0x00, 0x06, 0x00, 0x0e, 0x00, 0x1e, 0x00, 0x3e, 0x00, 0x7e, 0x00, 0xfe, 0x38, 0xfe, 0x44, 0x7e, 0x44, 0x3e, 0x20, 0x1e, 0x10, 0x0e, 0x10, 0x06, 0x00, 0x02, 0x10, 0x00, 0x00 };

static unsigned char tracking_S_bits[] U8X8_PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x06, 0x00, 0x0e, 0x00, 0x1e, 0x00, 0x3e, 0x00, 0x7e, 0x00, 0xfe, 0x38, 0x7e, 0x04, 0x3e, 0x04, 0x1e, 0x18, 0x0e, 0x20, 0x06, 0x20, 0x02, 0x1c, 0x00, 0x00 };

static unsigned char sleewing_bits[] U8X8_PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x02, 0x01, 0x06, 0x03, 0x0e, 0x07, 0x1e, 0x0f, 0x3e, 0x1f, 0x7e, 0x3f, 0xfe, 0x7f, 0x7e, 0x3f, 0x3e, 0x1f, 0x1e, 0x0f, 0x0e, 0x07, 0x06, 0x03, 0x02, 0x01, 0x00, 0x00 };
  
static unsigned char pec_play_bits[] U8X8_PROGMEM = {
  0x00, 0x00, 0x80, 0x07, 0x80, 0x08, 0x80, 0x08, 0x80, 0x08, 0x80, 0x08, 0x80, 0x07, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0xb8, 0x40, 0x44, 0xa2, 0x83, 0x9d, 0x00, 0x00, 0x00, 0x00 };

static unsigned char pec_record_bits[] U8X8_PROGMEM = {
  0x00, 0x00, 0x80, 0x07, 0x80, 0x08, 0x80, 0x08, 0x80, 0x08, 0x80, 0x08, 0x80, 0x07, 0x80, 0x00, 0x90, 0x04, 0xa0, 0x02, 0xc0, 0x01, 0xb8, 0x40, 0x44, 0xa2, 0x83, 0x9d, 0x00, 0x00, 0x00, 0x00 };

static unsigned char pec_wait_bits[] U8X8_PROGMEM = {
  0x00, 0x00, 0x80, 0x07, 0x80, 0x08, 0x80, 0x08, 0x80, 0x08, 0x98, 0x68, 0x98, 0x67, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0xb8, 0x40, 0x44, 0xa2, 0x83, 0x9d, 0x00, 0x00, 0x00, 0x00 };

static unsigned char W_bits[] U8X8_PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0xe0, 0x03, 0x10, 0x04, 0x08, 0x08, 0x24, 0x12, 0x22, 0x22, 0x22, 0x22, 0xa2, 0x22, 0xa2, 0x22, 0x42, 0x21, 0x44, 0x11, 0x08, 0x08, 0x10, 0x04, 0xe0, 0x03, 0x00, 0x00 };

static unsigned char E_bits[] U8X8_PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0xe0, 0x03, 0x10, 0x04, 0x08, 0x08, 0xe4, 0x13, 0x22, 0x20, 0x22, 0x20, 0xe2, 0x21, 0x22, 0x20, 0x22, 0x20, 0xe4, 0x13, 0x08, 0x08, 0x10, 0x04, 0xe0, 0x03, 0x00, 0x00 };

static unsigned char ErrDe_bits[] U8X8_PROGMEM = {
  0xff, 0xff, 0x00, 0x80, 0x0e, 0xb0, 0x02, 0xb0, 0x66, 0xb3, 0x22, 0xb1, 0x2e, 0xb1, 0x00, 0xb0, 0x1e, 0xb0, 0x22, 0xb0, 0xa2, 0xb3, 0xa2, 0xb2, 0xa2, 0x83, 0xa2, 0xb0, 0x9e, 0xb3, 0x00, 0x80 };

static unsigned char ErrHo_bits[] U8X8_PROGMEM = {
  0xff, 0xff, 0x00, 0x80, 0x0e, 0xb0, 0x02, 0xb0, 0x66, 0xb3, 0x22, 0xb1, 0x2e, 0xb1, 0x00, 0xb0, 0x22, 0xb0, 0x22, 0xb0, 0x22, 0xb3, 0xbe, 0xb4, 0xa2, 0x84, 0xa2, 0xb4, 0x22, 0xb3, 0x00, 0x80 };

static unsigned char ErrMe_bits[] U8X8_PROGMEM = {
  0xff, 0xff, 0x00, 0x80, 0x0e, 0xb0, 0x02, 0xb0, 0x66, 0xb3, 0x22, 0xb1, 0x2e, 0xb1, 0x00, 0xb0, 0x22, 0xb0, 0x36, 0xb0, 0xaa, 0xb3, 0xa2, 0xb2, 0xa2, 0x83, 0xa2, 0xb0, 0xa2, 0xb3, 0x00, 0x80 };

static unsigned char ErrMf_bits[] U8X8_PROGMEM = {
  0xff, 0xff, 0x00, 0x80, 0x0e, 0xb0, 0x02, 0xb0, 0x66, 0xb3, 0x22, 0xb1, 0x2e, 0xb1, 0x00, 0xb0, 0x22, 0xb0, 0x36, 0xb0, 0xaa, 0xb3, 0xa2, 0xb0, 0xa2, 0x81, 0xa2, 0xb0, 0xa2, 0xb0, 0x00, 0x80 };

static unsigned char ErrUp_bits[] U8X8_PROGMEM = {
  0xff, 0xff, 0x00, 0x80, 0x0e, 0xb0, 0x02, 0xb0, 0x66, 0xb3, 0x22, 0xb1, 0x2e, 0xb1, 0x00, 0xb0, 0x22, 0xb0, 0x22, 0xb0, 0xa2, 0xb3, 0xa2, 0xb2, 0xa2, 0x83, 0xa2, 0xb0, 0x9c, 0xb0, 0x00, 0x80 };

static const unsigned char teenastro_bits[] U8X8_PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x03,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x80, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00,
  0x00, 0x00, 0x00, 0x10, 0x00, 0x80, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x0f, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00,
  0x00, 0xe0, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x07, 0x00,
  0x00, 0x00, 0x00, 0x00, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0xe0, 0x07, 0x00, 0x00, 0x00, 0x01, 0x00, 0xfc, 0x01, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00,
  0xfe, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x07, 0x00,
  0x00, 0x00, 0x00, 0x00, 0xfe, 0x0f, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
  0x00, 0xe0, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0x3f, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00,
  0xfc, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x1f, 0x00,
  0x00, 0x00, 0x00, 0x00, 0xf0, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x80, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xff, 0x03, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0xff, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x7e, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x7f, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x07, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x80, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0x03, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfc, 0x0f,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0xf8, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x9c, 0x7f, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xfe,
  0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x06, 0xf8, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x0f, 0xe0, 0x07, 0x00, 0x00, 0x00,
  0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x1f, 0xc0,
  0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0xc0, 0x1e, 0x00, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x1e, 0x00, 0xfc, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x3c, 0x00,
  0xf0, 0x81, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x60, 0x2c, 0x00, 0x80, 0xc7, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x01, 0x00, 0x00, 0x00, 0x20, 0x68, 0x00, 0x00, 0xce, 0x3f, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x48, 0x00,
  0x00, 0xc0, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x10, 0xd8, 0x00, 0x00, 0xf0, 0x7f, 0x00, 0x00, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0xd8, 0x00, 0x00, 0xf0, 0x7f, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x98, 0x01,
  0x00, 0xf0, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x0c, 0x90, 0x01, 0x00, 0xf0, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x30, 0x01, 0x00, 0xf0, 0x3f, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x30, 0x03,
  0x00, 0xc0, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x02, 0x30, 0x07, 0x00, 0x80, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x30, 0x06, 0x00, 0x80, 0x7f, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x20, 0x04,
  0x00, 0x80, 0xff, 0x0f, 0xfe, 0x01, 0x00, 0x00, 0x1c, 0x00, 0x02, 0x00,
  0x80, 0x01, 0x20, 0x0c, 0x00, 0xc0, 0xff, 0x3f, 0xfe, 0x01, 0x00, 0x00,
  0x1c, 0x00, 0x03, 0x00, 0x80, 0x00, 0x20, 0x08, 0x00, 0xe0, 0xff, 0xff,
  0x30, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x03, 0x00, 0xc0, 0x00, 0x20, 0x18,
  0x00, 0xf0, 0xff, 0xff, 0x30, 0x38, 0x1c, 0x1b, 0x3e, 0x9c, 0xb7, 0x18,
  0x40, 0x00, 0x60, 0x30, 0x00, 0xfc, 0xff, 0xff, 0x30, 0x6c, 0x36, 0x3f,
  0x36, 0xbe, 0xf7, 0x3c, 0x60, 0x00, 0x40, 0x30, 0x80, 0xff, 0xff, 0xff,
  0x30, 0x6c, 0x36, 0x33, 0x36, 0x26, 0x33, 0x66, 0x20, 0x00, 0x40, 0x20,
  0xc0, 0x8f, 0xff, 0xff, 0x30, 0x7c, 0x3e, 0x33, 0x63, 0x1e, 0x33, 0x66,
  0x20, 0x00, 0x40, 0x60, 0xc0, 0x07, 0xff, 0xff, 0x30, 0x0c, 0x06, 0x33,
  0x7f, 0x38, 0x33, 0x66, 0x30, 0x00, 0x40, 0x40, 0xc0, 0x1f, 0xff, 0xff,
  0x30, 0x6c, 0x36, 0x33, 0x7f, 0x32, 0x33, 0x66, 0x10, 0x00, 0xc0, 0xc0,
  0xc0, 0x7f, 0xff, 0xff, 0x30, 0x6c, 0x36, 0xb3, 0xc1, 0x3e, 0x37, 0x3c,
  0x18, 0x00, 0xc0, 0xc0, 0xc0, 0xff, 0xff, 0xff, 0x30, 0x38, 0x1c, 0xb3,
  0xc1, 0x1c, 0x36, 0x1c, 0x0c, 0x00, 0xc0, 0x80, 0xc1, 0xff, 0xff, 0xff,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x00, 0xc0, 0x81,
  0xc1, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x0e, 0x00, 0xc0, 0x81, 0xf3, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x80, 0xc3, 0xf3, 0xff, 0xff, 0xff,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x80, 0xff,
  0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x07, 0xf8, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
  0x00, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

unsigned long display_blank_time = DISPLAY_BLANK_TIME;
unsigned long display_dim_time = DISPLAY_DIM_TIME;  

void gethms(const long& v, uint8_t& v1, uint8_t& v2, uint8_t& v3)
{
  v3 = v % 60;
  v2 = (v / 60) % 60;
  v1 = v / 3600;
}

/*
void getdms(const long& v, short& v1, uint8_t& v2, uint8_t& v3)
{
  v3 = abs(v) % 60;
  v2 = (abs(v) / 60) % 60;
  v1 = v / 3600;
}
*/

void secondsToFloat(const long& v, float& f)
{
  f = (double)v / 3600.0;
}

void getDegrees(const long& v, short& v1, uint8_t& v2, uint8_t& v3)
{
  v3 = abs(v) % 60;
  v2 = (abs(v) / 60) % 60;
  v1 = v / 3600;
}

void longRa2Ra(long Ra, int& h, int& m, int& s)
{
  h = Ra / 30;
  m = (Ra - h * 30) / 60;
  s = (Ra / 30) % 60;
}

void longDec2Dec(long Dec, int& deg, int& min)
{
  deg = Dec / 60;
  min = Dec % 60;
}

void SmartHandController::setup(const char version[], const int pin[7],const bool active[7], const int SerialBaud, const OLED model)
{
  if (strlen(version)<=19) strcpy(_version,version);
  
  telInfo.lastState = 0;
  buttonPad.setup( pin, active);
  
  //choose a 128x64 display supported by U8G2lib (if not listed below there are many many others in u8g2 library example Sketches)
  //U8G2_SH1106_128X64_NONAME_1_HW_I2C display(U8G2_R0);
  //U8G2_SSD1306_128X64_NONAME_F_SW_I2C display(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);   // All Boards without Reset of the Display

  if (model == OLED_SH1106)
    display = new U8G2_EXT_SH1106_128X64_NONAME_1_HW_I2C(U8G2_R0);
  else if (model == OLED_SSD1306)
    display = new U8G2_EXT_SSD1306_128X64_NONAME_F_HW_I2C(U8G2_R0);
  display->begin();
  drawIntro();
  tickButtons();

#ifdef DEBUG_ON
  DebugSer.begin(9600);
  delay(1000);
#endif
  Ser.begin(SerialBaud);
  for (int i = 0; i < 3; i++)
  {
    Ser.print(":#");
    delay(500);
    Ser.flush();
    delay(500);
  }
}
void SmartHandController::tickButtons()
{
  buttonPad.tickButtons();
}

void SmartHandController::update()
{
  tickButtons();
  unsigned long top = millis();

  // sleep and wake up display
  if (buttonPad.anyPressed()) {
    if (sleepDisplay) { display->setContrast(maxContrast); display->sleepOff(); sleepDisplay = false; lowContrast = false; buttonPad.clearAllPressed(); time_last_action = millis(); }
    if (lowContrast)  { display->setContrast(maxContrast); lowContrast = false; time_last_action = top; }
  } else if (sleepDisplay) return;
  if (display_blank_time && top - time_last_action > display_blank_time) { display->sleepOn(); sleepDisplay = true; return; }
  if (display_dim_time && top - time_last_action > display_dim_time && !lowContrast) { display->setContrast(0); lowContrast = true; return; }

  // power cycle reqd message
  if (powerCylceRequired) { display->setFont(u8g2_font_helvR12_tr); DisplayMessage("REBOOT", "DEVICE", 1000); return; }
  
  if (telInfo.align == Telescope::ALI_SELECT_STAR_1 || telInfo.align == Telescope::ALI_SELECT_STAR_2 || telInfo.align == Telescope::ALI_SELECT_STAR_3 || 
      telInfo.align == Telescope::ALI_SELECT_STAR_4 || telInfo.align == Telescope::ALI_SELECT_STAR_5 || telInfo.align == Telescope::ALI_SELECT_STAR_6 ||
      telInfo.align == Telescope::ALI_SELECT_STAR_7 || telInfo.align == Telescope::ALI_SELECT_STAR_8 || telInfo.align == Telescope::ALI_SELECT_STAR_9) {
    char message[10]="Star# ?";
    if (telInfo.align == Telescope::ALI_SELECT_STAR_1) strcpy(message,"Star #1");
    if (telInfo.align == Telescope::ALI_SELECT_STAR_2) strcpy(message,"Star #2");
    if (telInfo.align == Telescope::ALI_SELECT_STAR_3) strcpy(message,"Star #3");
    if (telInfo.align == Telescope::ALI_SELECT_STAR_4) strcpy(message,"Star #4");
    if (telInfo.align == Telescope::ALI_SELECT_STAR_5) strcpy(message,"Star #5");
    if (telInfo.align == Telescope::ALI_SELECT_STAR_6) strcpy(message,"Star #6");
    if (telInfo.align == Telescope::ALI_SELECT_STAR_7) strcpy(message,"Star #7");
    if (telInfo.align == Telescope::ALI_SELECT_STAR_8) strcpy(message,"Star #8");
    if (telInfo.align == Telescope::ALI_SELECT_STAR_9) strcpy(message,"Star #9");
    DisplayLongMessage("Select a Star", "From following list", "", message, -1);

    // bring up the list of stars so user can goto the alignment star
    if (!SelectStarAlign()) { DisplayMessage("Alignment", "Aborted", -1); telInfo.align = Telescope::ALI_OFF; return; }
    
    // mark this as the next alignment star
    telInfo.align = static_cast<Telescope::AlignState>(telInfo.align + 1);
  } else
  if (top - lastpageupdate > BACKGROUND_CMD_RATE/2) updateMainDisplay(page);
  
  if (telInfo.connected == false) DisplayMessage("Hand controler", "not connected", -1);

  // is a goto happening?
  if (telInfo.connected && (telInfo.getTrackingState() == Telescope::TRK_SLEWING || telInfo.getParkState() == Telescope::PRK_PARKING))
  {
    if (buttonPad.anyPressed()) {
      Ser.print(":Q#"); Ser.flush();
      if (telInfo.align != Telescope::ALI_OFF) telInfo.align = static_cast<Telescope::AlignState>(telInfo.align - 1); // try another align star?
      time_last_action = millis();
      display->sleepOff();
      return;
    }
  } 
  else // guide
  {
    buttonCommand = false;
    if (!moveEast  && buttonPad.e.isDown()) { moveEast = true;   Ser.write(ccMe); buttonCommand=true; } else
    if (moveEast   && buttonPad.e.isUp()  ) { moveEast = false;  Ser.write(ccQe); buttonCommand=true; buttonPad.e.clearPress(); }
    if (!moveWest  && buttonPad.w.isDown()) { moveWest = true;   Ser.write(ccMw); buttonCommand=true; } else
    if (moveWest   && buttonPad.w.isUp()  ) { moveWest = false;  Ser.write(ccQw); buttonCommand=true; buttonPad.w.clearPress(); }
    if (!moveNorth && buttonPad.n.isDown()) { moveNorth = true;  Ser.write(ccMn); buttonCommand=true; } else
    if (moveNorth  && buttonPad.n.isUp()  ) { moveNorth = false; Ser.write(ccQn); buttonCommand=true; buttonPad.n.clearPress(); }
    if (!moveSouth && buttonPad.s.isDown()) { moveSouth = true;  Ser.write(ccMs); buttonCommand=true; } else
    if (moveSouth  && buttonPad.s.isUp()  ) { moveSouth = false; Ser.write(ccQs); buttonCommand=true; buttonPad.s.clearPress(); }
    if (buttonCommand) { time_last_action = millis(); return; }
  }

  // handle fF button features
  static bool moveOut=false;
  static bool moveIn=false;
  if (telInfo.atHome()) {
    if (buttonPad.F.wasPressed()) { Ser.print(":B+#"); time_last_action = millis(); return; } else
    if (buttonPad.f.wasPressed()) { Ser.print(":B-#"); time_last_action = millis(); return; }
  } else {
    // should send the ":FA#" command to see if we have a focuser and revert to always use reticule brightness if not
    buttonCommand = false;
    if (!moveOut && buttonPad.F.isDown()) { moveOut = true;  Ser.print(":F+#"); buttonCommand=true; } else
    if (moveOut && buttonPad.F.isUp())    { moveOut = false; Ser.print(":FQ#"); buttonCommand=true; buttonPad.F.clearPress(); } else
    if (!moveIn && buttonPad.f.isDown())  { moveIn = true;   Ser.print(":F-#"); buttonCommand=true; } else
    if (moveIn && buttonPad.f.isUp())     { moveIn = false;  Ser.print(":FQ#"); buttonCommand=true; buttonPad.f.clearPress(); }
    if (buttonCommand) { time_last_action = millis(); return; }
  }

  // handle shift button features
  if (buttonPad.shift.isDown()) {
    if ((buttonPad.shift.timeDown()>1000) && telInfo.align == Telescope::ALI_OFF) { menuMain(); time_last_action = millis(); }                                                        // bring up the menus
  } else {
    // wait long enough that a double press can happen before picking up the press events
    if (buttonPad.shift.timeUp()>250) {
      if (buttonPad.shift.wasDoublePressed()) { menuSpeedRate(); time_last_action = millis(); } else                                                                                  // change guide rate
      if (telInfo.align == Telescope::ALI_OFF && buttonPad.shift.wasPressed(false)) { page++; if (page > 2) page = 0; time_last_action = millis(); } else                             // cycle through disp of Eq, Hor, Time
      if ((telInfo.align == Telescope::ALI_RECENTER_1 || telInfo.align == Telescope::ALI_RECENTER_2 || telInfo.align == Telescope::ALI_RECENTER_3 ||
           telInfo.align == Telescope::ALI_RECENTER_4 || telInfo.align == Telescope::ALI_RECENTER_5 || telInfo.align == Telescope::ALI_RECENTER_6 ||
           telInfo.align == Telescope::ALI_RECENTER_7 || telInfo.align == Telescope::ALI_RECENTER_8 || telInfo.align == Telescope::ALI_RECENTER_9) && 
           buttonPad.shift.wasPressed()) {                                             // add this align star
        if (telInfo.addStar()) { if (telInfo.align == Telescope::ALI_OFF) DisplayMessage("Alignment", "Success!", 2000); else DisplayMessage("Add Star", "Success!", 2000); } else DisplayMessage("Add Star", "Failed!", -1);
      }
    }
  }
}

void SmartHandController::updateMainDisplay( u8g2_uint_t page)
{
  u8g2_t *u8g2 = display->getU8g2();
  display->setFont(u8g2_font_helvR12_te);
  u8g2_uint_t line_height = u8g2_GetAscent(u8g2) - u8g2_GetDescent(u8g2) + MY_BORDER_SIZE;
  u8g2_uint_t step1 = u8g2_GetUTF8Width(u8g2, "44");
  u8g2_uint_t step2 = u8g2_GetUTF8Width(u8g2, "4") + 1;

  // get the status
  telInfo.connected = true;
  telInfo.updateSeq++;
  telInfo.updateTel();
  if (telInfo.connected == false) return;

  // detect align mode
  if (telInfo.hasTelStatus && telInfo.align != Telescope::ALI_OFF)
  {
    Telescope::TrackState curT = telInfo.getTrackingState();
    if (curT != Telescope::TRK_SLEWING && 
       (telInfo.align == Telescope::ALI_SLEW_STAR_1 || telInfo.align == Telescope::ALI_SLEW_STAR_2 || telInfo.align == Telescope::ALI_SLEW_STAR_3 || 
        telInfo.align == Telescope::ALI_SLEW_STAR_4 || telInfo.align == Telescope::ALI_SLEW_STAR_5 || telInfo.align == Telescope::ALI_SLEW_STAR_6 ||
        telInfo.align == Telescope::ALI_SLEW_STAR_7 || telInfo.align == Telescope::ALI_SLEW_STAR_8 || telInfo.align == Telescope::ALI_SLEW_STAR_9)
       ) telInfo.align = static_cast<Telescope::AlignState>(telInfo.align + 1);
    page = 3;
  }

  // update status info.
  if (page == 0)
    telInfo.updateRaDec();
  else
    if (page == 1)
      telInfo.updateAzAlt();
    else
      if (page == 2)
        telInfo.updateTime();
 
  // the graphics loop
  u8g2_FirstPage(u8g2);
  do
  {
    u8g2_uint_t x = u8g2_GetDisplayWidth(u8g2);
    int k = 0;

    // wifi status
    if (wifiOn) display->drawXBMP(0, 0, icon_width, icon_height, wifi_bits);

    // OnStep status
    if (telInfo.hasTelStatus) {
      Telescope::ParkState curP = telInfo.getParkState();
      Telescope::TrackState curT = telInfo.getTrackingState();
      if (curP == Telescope::PRK_PARKED)  { display->drawXBMP(x - icon_width, 0, icon_width, icon_height, parked_bits); x -= icon_width + 1; } else
      if (curP == Telescope::PRK_PARKING) { display->drawXBMP(x - icon_width, 0, icon_width, icon_height, parking_bits); x -= icon_width + 1; } else
      if (telInfo.atHome())               { display->drawXBMP(x - icon_width, 0, icon_width, icon_height, home_bits); x -= icon_width + 1;  } else 
      {
        if (curT == Telescope::TRK_SLEWING) { display->drawXBMP(x - icon_width, 0, icon_width, icon_height, sleewing_bits); x -= icon_width + 1; } else
        if (curT == Telescope::TRK_ON)      { display->drawXBMP(x - icon_width, 0, icon_width, icon_height, tracking_S_bits); x -= icon_width + 1; } else
        if (curT == Telescope::TRK_OFF)     { display->drawXBMP(x - icon_width, 0, icon_width, icon_height, no_tracking_bits); x -= icon_width + 1; }

        if (curP == Telescope::PRK_FAILED)  { display->drawXBMP(x - icon_width, 0, icon_width, icon_height, parkingFailed_bits); x -= icon_width + 1; }

        if (telInfo.hasPierInfo) {
          Telescope::PierState CurP = telInfo.getPierState();
          if (CurP == Telescope::PIER_E) { display->drawXBMP(x - icon_width, 0, icon_width, icon_height, E_bits); x -= icon_width + 1; } else 
          if (CurP == Telescope::PIER_W) { display->drawXBMP(x - icon_width, 0, icon_width, icon_height, W_bits); x -= icon_width + 1; }
        }

        if (telInfo.align != Telescope::ALI_OFF) {
          if (telInfo.aliMode == Telescope::ALIM_ONE)   { display->drawXBMP(x - icon_width, 0, icon_width, icon_height, align1_bits); x -= icon_width + 1; } else 
          if (telInfo.aliMode == Telescope::ALIM_TWO)   { display->drawXBMP(x - icon_width, 0, icon_width, icon_height, align2_bits); x -= icon_width + 1; } else
          if (telInfo.aliMode == Telescope::ALIM_THREE) { display->drawXBMP(x - icon_width, 0, icon_width, icon_height, align3_bits); x -= icon_width + 1; } else
          if (telInfo.aliMode == Telescope::ALIM_FOUR)  { display->drawXBMP(x - icon_width, 0, icon_width, icon_height, align4_bits); x -= icon_width + 1; } else
          if (telInfo.aliMode == Telescope::ALIM_FIVE)  { display->drawXBMP(x - icon_width, 0, icon_width, icon_height, align5_bits); x -= icon_width + 1; } else
          if (telInfo.aliMode == Telescope::ALIM_SIX)   { display->drawXBMP(x - icon_width, 0, icon_width, icon_height, align6_bits); x -= icon_width + 1; } else
          if (telInfo.aliMode == Telescope::ALIM_SEVEN) { display->drawXBMP(x - icon_width, 0, icon_width, icon_height, align7_bits); x -= icon_width + 1; } else
          if (telInfo.aliMode == Telescope::ALIM_EIGHT) { display->drawXBMP(x - icon_width, 0, icon_width, icon_height, align8_bits); x -= icon_width + 1; } else
          if (telInfo.aliMode == Telescope::ALIM_NINE ) { display->drawXBMP(x - icon_width, 0, icon_width, icon_height, align9_bits); x -= icon_width + 1; }
        }

        if (telInfo.isPecPlaying())   { display->drawXBMP(x - icon_width, 0, icon_width, icon_height, pec_play_bits);   x -= icon_width + 1; } else
        if (telInfo.isPecRecording()) { display->drawXBMP(x - icon_width, 0, icon_width, icon_height, pec_record_bits); x -= icon_width + 1; } else
        if (telInfo.isPecWaiting())   { display->drawXBMP(x - icon_width, 0, icon_width, icon_height, pec_wait_bits);   x -= icon_width + 1; }

        if (telInfo.isGuiding()) { display->drawXBMP(x - icon_width, 0, icon_width, icon_height, guiding_bits); x -= icon_width + 1; }
      }

      switch (telInfo.getError()) {
        case Telescope::ERR_MOTOR_FAULT: display->drawXBMP(x - icon_width, 0, icon_width, icon_height, ErrMf_bits); x -= icon_width + 1; break;
        case Telescope::ERR_ALT:         display->drawXBMP(x - icon_width, 0, icon_width, icon_height, ErrHo_bits); x -= icon_width + 1; break;
        case Telescope::ERR_DEC:         display->drawXBMP(x - icon_width, 0, icon_width, icon_height, ErrDe_bits); x -= icon_width + 1; break;
        case Telescope::ERR_UNDER_POLE:  display->drawXBMP(x - icon_width, 0, icon_width, icon_height, ErrUp_bits); x -= icon_width + 1; break;
        case Telescope::ERR_MERIDIAN:    display->drawXBMP(x - icon_width, 0, icon_width, icon_height, ErrMe_bits); x -= icon_width + 1; break;
        default: break;
      }
    }

    // show equatorial coordinates
    if (page == 0)
    {
      if (telInfo.hasInfoRa && telInfo.hasInfoDec) {
        char rs[20]; strcpy(rs,telInfo.TempRa); rs[2]=0; rs[5]=0;
        x = u8g2_GetDisplayWidth(u8g2);  u8g2_uint_t y = 36;
        u8g2_DrawUTF8(u8g2, 0, y, "RA"); display->drawRA( x, y, rs, &rs[3], &rs[6]);

        char ds[20]; strcpy(ds,telInfo.TempDec); ds[3]=0; ds[6]=0; char sds[2]="x"; sds[0]=ds[0];
        y += line_height + 4;
        u8g2_DrawUTF8(u8g2, 0, y, "Dec"); display->drawDec( x, y, sds, &ds[1], &ds[4], &ds[7]);
      }
    }

    // show horizon coordinates
    if (page == 1) {
      if (telInfo.hasInfoAz && telInfo.hasInfoAlt)
      {
        char zs[20]; strcpy(zs,telInfo.TempAz); zs[3]=0; zs[6]=0;
        u8g2_uint_t y = 36; u8g2_uint_t startpos = u8g2_GetUTF8Width(u8g2, "123456"); x = startpos; x = u8g2_GetDisplayWidth(u8g2);
        u8g2_DrawUTF8(u8g2, 0, y, "Az."); display->drawAz(x, y, zs, &zs[4], &zs[7]);

        char as[20]; strcpy(as,telInfo.TempAlt); as[3]=0; as[6]=0; char sas[2]="x"; sas[0]=as[0];
        y += line_height + 4; x = startpos; x = u8g2_GetDisplayWidth(u8g2);
        u8g2_DrawUTF8(u8g2, 0, y, "Alt."); display->drawDec( x, y, sas, &as[1], &as[4], &as[7]);
      }
    }
    
    // show time
    if (page == 2) {
      if (telInfo.hasInfoUTC && telInfo.hasInfoSidereal)
      {
        char us[20]; strcpy(us,telInfo.TempLocalTime); us[2]=0; us[5]=0;
        x = u8g2_GetDisplayWidth(u8g2);  u8g2_uint_t y = 36;
        u8g2_DrawUTF8(u8g2, 0, y, "Local"); display->drawRA( x, y, us, &us[3], &us[6]);

        char ss[20]; strcpy(ss,telInfo.TempSidereal); ss[2]=0; ss[5]=0;
        y += line_height + 4;
        u8g2_DrawUTF8(u8g2, 0, y, "Sidereal"); display->drawRA(x, y, ss, &ss[3], &ss[6]);
      }
    }

    // show align status
    if (page == 3) {
      u8g2_uint_t y = 36;

      char txt[20];
      if ((telInfo.align - 1) % 3 == 1) sprintf(txt, "Slew to Star %u", (telInfo.align - 1) / 3 + 1); else
      if ((telInfo.align - 1) % 3 == 2) sprintf(txt, "Recenter Star %u", (telInfo.align - 1) / 3 + 1);
      u8g2_DrawUTF8(u8g2, 0, y, txt);

      y += line_height + 4;
      u8g2_SetFont(u8g2, u8g2_font_unifont_t_greek);
      u8g2_DrawGlyph(u8g2, 0, y, 944 + cat_mgr.primaryId());

      const uint8_t* myfont = u8g2->font; u8g2_SetFont(u8g2, myfont);
      u8g2_DrawUTF8(u8g2, 16, y, cat_mgr.constellationStr());
    }

  } while (u8g2_NextPage(u8g2));
  lastpageupdate = millis();
}

void SmartHandController::drawIntro()
{
  display->firstPage();
  do {
    display->drawXBMP(0, 0, teenastro_width, teenastro_height, teenastro_bits);
  } while (display->nextPage());
  delay(2000);
}

void SmartHandController::drawLoad()
{
  display->firstPage();
  uint8_t x = 0;
  do {
    display->setFont(u8g2_font_helvR14_tr);
    x = (display->getDisplayWidth() - display->getStrWidth("Loading")) / 2;
    display->drawStr(x, display->getDisplayHeight()/2. - 14, "Loading");
    x = (display->getDisplayWidth() - display->getStrWidth(_version)) / 2;
    display->drawStr(x, display->getDisplayHeight() / 2. + 14, _version);
  } while (display->nextPage());
}

void SmartHandController::drawReady()
{
  display->firstPage();
  uint8_t x = 0;
  do {
    x = (display->getDisplayWidth() - display->getStrWidth("Ready!")) / 2;
    display->drawStr(x, display->getDisplayHeight() / 2, "Ready!");
  } while (display->nextPage());
  delay(500);
}

// Alt Main Menu (double click)
void SmartHandController::menuSpeedRate()
{
#ifndef SHC_ON
  char * string_list_Speed = "0.25x\n0.5x\n1.0x\n2.0x\n4.0x\n16.0x\n32.0x\n64.0x\n0.5 Max\nMax";
#else
  char * string_list_Speed = "0.25x\n0.5x\n1.0x\n2.0x\n4.0x\n8.0x\n24.0x\n48.0x\n0.5 Max\nMax";
#endif
  current_selection_speed = display->UserInterfaceSelectionList(&buttonPad, "Set Speed", current_selection_speed, string_list_Speed);
  if (current_selection_speed > 0)
  {
    char cmd[5]= ":Rn#";
    cmd[2] = '0' + current_selection_speed - 1;
    DisplayMessageLX200(SetLX200(cmd));
  }
}

// Main Menu
void SmartHandController::menuMain()
{
  current_selection_L0 = 1;
  while (current_selection_L0 != 0)
  {
    boolean sync=false;
    if (!telInfo.isMountAltAz()) {
      const char *string_list_main_UnParkedL0 = "Goto\n""Sync\n""Align\n""Parking\n""Tracking\n""PEC\n""Settings";
      current_selection_L0 = display->UserInterfaceSelectionList(&buttonPad, "Main Menu", current_selection_L0, string_list_main_UnParkedL0);
      switch (current_selection_L0) {
        case 1: menuSyncGoto(false); break;
        case 2: if (display->UserInterfaceInputValueBoolean(&buttonPad, "Sync Here?", &sync)) if (sync) DisplayMessageLX200(SetLX200(":CS#"),false); break;
        case 3: menuAlignment(); break;
        case 4: menuParking(); break;
        case 5: menuTracking(); break;
        case 6: menuPEC(); break;
        case 7: menuSettings(); break;
        default: break;
      }
    } else {
      const char *string_list_main_UnParkedL0 = "Goto\n""Sync\n""Align\n""Parking\n""Tracking\n""Settings";
      current_selection_L0 = display->UserInterfaceSelectionList(&buttonPad, "Main Menu", current_selection_L0, string_list_main_UnParkedL0);
      switch (current_selection_L0) {
        case 1: menuSyncGoto(false); break;
        case 2: if (display->UserInterfaceInputValueBoolean(&buttonPad, "Sync Here?", &sync)) if (sync) DisplayMessageLX200(SetLX200(":CS#"),false); break;
        case 3: menuAlignment(); break;
        case 4: menuParking(); break;
        case 5: menuTracking(); break;
        case 6: menuSettings(); break;
        default: break;
      }
    }
  }
}

#include "MenusSyncGoto.h"

bool SmartHandController::SelectStarAlign()
{
  if (!cat_mgr.canFilter()) { cat_mgr.setLat(telInfo.getLat()); cat_mgr.setLstT0(telInfo.getLstT0()); }
  cat_mgr.select(STAR);
  
  cat_mgr.filter(FM_ALIGN_ALL_SKY);

  cat_mgr.setIndex(0);
  if (cat_mgr.canFilter()) {
    if (display->UserInterfaceCatalog(&buttonPad, "Select Star")) {
      bool ok = DisplayMessageLX200(SyncSelectedStarLX200(cat_mgr.getIndex()),false);
      return ok;
    }
  }
  return false;
}

// Parking Menu
void SmartHandController::menuParking()
{
  Telescope::ParkState currentstate = telInfo.getParkState();
  if (currentstate == Telescope::PRK_PARKED) { }
  if (currentstate == Telescope::PRK_UNPARKED) { }

  current_selection_L1 = 1;
  while (current_selection_L1 != 0)
  {
    const char *string_list_SettingsL1 = "Park\n""Un-Park\n""Set-Park";
    current_selection_L1 = display->UserInterfaceSelectionList(&buttonPad, "Parking", current_selection_L1, string_list_SettingsL1);
    switch (current_selection_L1)
    {
    case 1: DisplayMessageLX200(SetLX200(":hP#"),false); break;
    case 2: DisplayMessageLX200(SetLX200(":hR#"),false); break;
    case 3: DisplayMessageLX200(SetLX200(":hQ#"),false); break;
    default: break;
    }
  }
}

// PEC Menu
void SmartHandController::menuPEC()
{
  current_selection_L1 = 1;
  while (current_selection_L1 != 0)
  {
    const char *string_list_SettingsL1 = "Play\n""Stop\n""Clear\n""Record\n""Write to NV";
    current_selection_L1 = display->UserInterfaceSelectionList(&buttonPad, "PEC", current_selection_L1, string_list_SettingsL1);
    switch (current_selection_L1)
    {
    case 1: DisplayMessageLX200(SetLX200(":$QZ+#"),true); current_selection_L0=0; current_selection_L1=0; DisplayMessage("PEC", "Playing", 1000); break;
    case 2: DisplayMessageLX200(SetLX200(":$QZ-#"),true); current_selection_L0=0; current_selection_L1=0; DisplayMessage("PEC", "Stopped", 1000); break;
    case 3: DisplayMessageLX200(SetLX200(":$QZZ#"),false); break;
    case 4: DisplayMessageLX200(SetLX200(":$QZ/#"),true); current_selection_L0=0; current_selection_L1=0; DisplayMessage("PEC", "Recording", 1000); break;
    case 5: DisplayMessageLX200(SetLX200(":$QZ!#"),false); break;
    default: break;
    }
  }
}

// Settings Menu
void SmartHandController::menuSettings()
{
  current_selection_L1 = 1;
#ifdef SHC_ON
  while (current_selection_L1 != 0)
  {
    if (telInfo.isMountGEM()) {
      const char *string_list_SettingsL1 = "Date/Time\n""Display\n""Buzzer\n""Meridian Flp\n""Configuration\n""Site";
      current_selection_L1 = display->UserInterfaceSelectionList(&buttonPad, "Settings", current_selection_L1, string_list_SettingsL1);
      switch (current_selection_L1)
      {
      case 1: menuLocalDateTime(); break;
      case 2: menuDisplay(); break;
      case 3: menuSound(); break;
      case 4: menuMeridianFlips(); break;
      case 5: menuMount(); break; // Configuration
      case 6: menuSite(); break;
      default: break;
      }
    } else {
      const char *string_list_SettingsL1 = "Date/Time\n""Display\n""Buzzer\n""Configuration\n""Site";
      current_selection_L1 = display->UserInterfaceSelectionList(&buttonPad, "Settings", current_selection_L1, string_list_SettingsL1);
      switch (current_selection_L1)
      {
      case 1: menuLocalDateTime(); break;
      case 2: menuDisplay(); break;
      case 3: menuSound(); break;
      case 4: menuMount(); break; // Configuration
      case 5: menuSite(); break;
      default: break;
      }
    }
  }
#else
  while (current_selection_L1 != 0)
  {
    const char *string_list_SettingsL1 = "Date/Time\n""Pier Side\n""Goto Speed\n""Display\n""Buzzer\n""Meridian Flp\n""Mount\n""Limits\n""Wifi\n""Site";
    current_selection_L1 = display->UserInterfaceSelectionList(&buttonPad, "Settings", current_selection_L1, string_list_SettingsL1);
    switch (current_selection_L1)
    {
    case 1:  menuLocalDateTime(); break;
    case 2:  menuPier(); break;
    case 3:  menuGotoSpeed(); break;
    case 4:  menuDisplay(); break;
    case 5:  menuSound(); break;
    case 6:  if (telInfo.isMountGEM()) menuMeridianFlips(); else DisplayMessage("Meridian flips", "are disabled", 1500); break;
    case 7:  menuMount(); break;
    case 8:  menuLimits(); break;
    case 9:  menuWifi(); break;
    case 10: menuSite(); break;
    default: break;
    }
  }
#endif
}

void SmartHandController::menuLocalDateTime()
{
  char out[20];
  // Date
  if (DisplayMessageLX200(GetLX200(":GC#", out)))
  {
    char* pEnd;
    uint8_t month = strtol(&out[0], &pEnd, 10);
    uint8_t day = strtol(&out[3], &pEnd, 10);
    uint8_t year = strtol(&out[6], &pEnd, 10);
    if (display->UserInterfaceInputValueDate(&buttonPad, "Local Date", year, month, day))
    {
      sprintf(out, ":SC%02d/%02d/%02d#", month, day, year); DisplayMessageLX200(SetLX200(out),false);
      // Time
      long value;
      boolean pmf=false;
      boolean dst=false;
      if (DisplayMessageLX200(GetTimeLX200(value))) {
        if (value>=43200) { value-=43200; pmf=true; }
        if (display->UserInterfaceInputValueTime(&buttonPad, &value)) {
          if (display->UserInterfaceInputValueBoolean(&buttonPad, "Local Time PM?", &pmf)) {
            if (pmf) value+=43200; // AM or PM?
            if (display->UserInterfaceInputValueBoolean(&buttonPad, "Local Time DST?", &dst)) {
              if (dst) value-=3600; // Dst?
              DisplayMessageLX200(SetTimeLX200(value),false);
            }
          }
        }
      }
    }
  }
}

void SmartHandController::menuAlignment()
{
  int maxAlignStars, thisStar, numStars;
  bool alignInProgress=false;

  if (!telInfo.getAlignStars(&maxAlignStars, &thisStar, &numStars)) { maxAlignStars=3; thisStar=1; numStars=1; }
  if ((thisStar>0) && (thisStar<=numStars)) alignInProgress=true;

  telInfo.aliMode = Telescope::ALIM_OFF;
  current_selection_L1 = 1;
  while (current_selection_L1 != 0)
  {
    int starsForAlign=0;
    bool showAlign=false;
    bool clearAlign=false;
    bool resetAlign=false;
  
    char string_list_AlignmentL1[120];
    if (alignInProgress) {
      strcpy(string_list_AlignmentL1,"Resume Align\n""Show Corr\n""Clear Corr\n""Reset Home");
      current_selection_L1 = display->UserInterfaceSelectionList(&buttonPad, "Alignment", current_selection_L1, string_list_AlignmentL1);
      switch (current_selection_L1) {
        case 1:
          switch (numStars) {
            case 1: telInfo.aliMode = Telescope::ALIM_ONE; break;
            case 2: telInfo.aliMode = Telescope::ALIM_TWO; break;
            case 3: telInfo.aliMode = Telescope::ALIM_THREE; break;
            case 4: telInfo.aliMode = Telescope::ALIM_FOUR; break;
            case 5: telInfo.aliMode = Telescope::ALIM_FIVE; break;
            case 6: telInfo.aliMode = Telescope::ALIM_SIX; break;
            case 7: telInfo.aliMode = Telescope::ALIM_SEVEN; break;
            case 8: telInfo.aliMode = Telescope::ALIM_EIGHT; break;
            case 9: telInfo.aliMode = Telescope::ALIM_NINE; break;
            defualt: break;
          }
          current_selection_L1 = 0; current_selection_L0 = 0; // Quit Menu
        break;
        case 2: showAlign=true; break;
        case 3: clearAlign=true; break;
        case 4: resetAlign=true; break;
        default: break;
      }
    } else
    if ((maxAlignStars==1) || (maxAlignStars==2)) {
      strcpy(string_list_AlignmentL1,"1-Star Align\n""Reset Home");
      current_selection_L1 = display->UserInterfaceSelectionList(&buttonPad, "Alignment", current_selection_L1, string_list_AlignmentL1);
      switch (current_selection_L1) {
        case 1: starsForAlign=1; break;
        case 2: resetAlign=true; break;
        default: break;
      }
    } else
    if (maxAlignStars==3) {
      strcpy(string_list_AlignmentL1,"1-Star Align\n""2-Star Align\n""3-Star Align\n""Show Model\n""Clear Model\n""Reset Home");
      current_selection_L1 = display->UserInterfaceSelectionList(&buttonPad, "Alignment", current_selection_L1, string_list_AlignmentL1);
      switch (current_selection_L1) {
        case 1: starsForAlign=1; break; case 2: starsForAlign=2; break; case 3: starsForAlign=3; break;
        case 4: showAlign=true; break;  case 5: clearAlign=true; break; case 6: resetAlign=true; break;
        default: break;
      }
    } else
    if ((maxAlignStars==4) || (maxAlignStars==5)) {
      strcpy(string_list_AlignmentL1,"1-Star Align\n""3-Star Align\n""4-Star Align\n""Show Model\n""Clear Model\n""Reset Home");
      current_selection_L1 = display->UserInterfaceSelectionList(&buttonPad, "Alignment", current_selection_L1, string_list_AlignmentL1);
      switch (current_selection_L1) {
        case 1: starsForAlign=1; break; case 2: starsForAlign=3; break; case 3: starsForAlign=4; break;
        case 4: showAlign=true; break;  case 5: clearAlign=true; break; case 6: resetAlign=true; break;
        default: break;
      }
    } else
    if (maxAlignStars==6) {
      strcpy(string_list_AlignmentL1,"1-Star Align\n""3-Star Align\n""4-Star Align\n""6-Star Align\n""Show Model\n""Clear Model\n""Reset Home");
      current_selection_L1 = display->UserInterfaceSelectionList(&buttonPad, "Alignment", current_selection_L1, string_list_AlignmentL1);
      switch (current_selection_L1) {
        case 1: starsForAlign=1; break; case 2: starsForAlign=3; break; case 3: starsForAlign=4; break; case 4: starsForAlign=6; break;
        case 5: showAlign=true; break;  case 6: clearAlign=true; break; case 7: resetAlign=true; break;
        default: break;
      }
    } else
    if (maxAlignStars==9) {
      strcpy(string_list_AlignmentL1,"1-Star Align\n""3-Star Align\n""6-Star Align\n""9-Star Align\n""Show Model\n""Clear Model\n""Reset Home");
      current_selection_L1 = display->UserInterfaceSelectionList(&buttonPad, "Alignment", current_selection_L1, string_list_AlignmentL1);
      switch (current_selection_L1) {
        case 1: starsForAlign=1; break; case 2: starsForAlign=3; break; case 3: starsForAlign=6; break; case 4: starsForAlign=9; break;
        case 5: showAlign=true; break;  case 6: clearAlign=true; break; case 7: resetAlign=true; break;
        default: break;
      }
    }
  
    // handle misc. resulting actions
    if (showAlign) {
      char r2[20]=""; char r3[20]=""; char r4[20]=""; char r5[20]=""; char r8[20]="";
      if ((GetLX200Trim(":GX02#",r2)==LX200VALUEGET) && (GetLX200Trim(":GX03#",r3)==LX200VALUEGET) && 
          (GetLX200Trim(":GX04#",r4)==LX200VALUEGET) && (GetLX200Trim(":GX05#",r5)==LX200VALUEGET) && 
          (GetLX200Trim(":GX08#",r8)==LX200VALUEGET)) {
        char s1[20]=""; strcat(s1,"PE:"); strcat(s1,r2); strcat(s1,", PZ:"); strcat(s1,r3);
        char s2[20]=""; strcat(s2,"DO (cone):"); strcat(s2,r4);
        char s3[20]=""; strcat(s3,"PD:"); strcat(s3,r5); strcat(s3,", TF:"); strcat(s3,r8);
        DisplayLongMessage("Align results (in \")", s1, s2, s3, -1);
      }
    } else  
    if (clearAlign) {
      if ((SetLX200(":SX02,0#")==LX200VALUESET) && (SetLX200(":SX03,0#")==LX200VALUESET) &&
          (SetLX200(":SX04,0#")==LX200VALUESET) && (SetLX200(":SX05,0#")==LX200VALUESET) &&
          (SetLX200(":SX06,0#")==LX200VALUESET) && (SetLX200(":SX07,0#")==LX200VALUESET) &&
          (SetLX200(":SX08,0#")==LX200VALUESET)) DisplayMessageLX200(LX200VALUESET,false); else DisplayMessageLX200(LX200SETVALUEFAILED,false);
    } else
    if (resetAlign) {
      current_selection_L1 = 0; current_selection_L0 = 0; // Quit Menu
      if (SetLX200(":hF#") == LX200VALUESET) DisplayMessage("Reset", "Move to Home", -1);
    } else
    if (starsForAlign>0) {
      switch (starsForAlign) {
        case 1: if (SetLX200(":A1#") == LX200VALUESET) telInfo.aliMode = Telescope::ALIM_ONE;   else DisplayMessage("Alignment", "Failed!", -1); break;
        case 2: if (SetLX200(":A2#") == LX200VALUESET) telInfo.aliMode = Telescope::ALIM_TWO;   else DisplayMessage("Alignment", "Failed!", -1); break;
        case 3: if (SetLX200(":A3#") == LX200VALUESET) telInfo.aliMode = Telescope::ALIM_THREE; else DisplayMessage("Alignment", "Failed!", -1); break;
        case 4: if (SetLX200(":A4#") == LX200VALUESET) telInfo.aliMode = Telescope::ALIM_FOUR;  else DisplayMessage("Alignment", "Failed!", -1); break;
        case 5: if (SetLX200(":A5#") == LX200VALUESET) telInfo.aliMode = Telescope::ALIM_FIVE;  else DisplayMessage("Alignment", "Failed!", -1); break;
        case 6: if (SetLX200(":A6#") == LX200VALUESET) telInfo.aliMode = Telescope::ALIM_SIX;   else DisplayMessage("Alignment", "Failed!", -1); break;
        case 7: if (SetLX200(":A7#") == LX200VALUESET) telInfo.aliMode = Telescope::ALIM_SEVEN; else DisplayMessage("Alignment", "Failed!", -1); break;
        case 8: if (SetLX200(":A8#") == LX200VALUESET) telInfo.aliMode = Telescope::ALIM_EIGHT; else DisplayMessage("Alignment", "Failed!", -1); break;
        case 9: if (SetLX200(":A9#") == LX200VALUESET) telInfo.aliMode = Telescope::ALIM_NINE;  else DisplayMessage("Alignment", "Failed!", -1); break;
      }
  #ifndef SHC_ON
      if (SetLX200(":R7#") == LX200VALUESET) DisplayMessage("Guide Rate", "64X Set", 1000);
  #else
      if (SetLX200(":R7#") == LX200VALUESET) DisplayMessage("Guide Rate", "48X Set", 1000);
  #endif
      current_selection_L1 = 0; current_selection_L0 = 0; // Quit Menu
    }
  }

  if (telInfo.aliMode!=Telescope::ALIM_OFF) {
    if (!telInfo.getAlignStars(&maxAlignStars, &thisStar, &numStars)) { maxAlignStars=3; thisStar=1; numStars=1; }
    switch (thisStar) {
      case 1: telInfo.align = Telescope::ALI_SELECT_STAR_1; break;
      case 2: telInfo.align = Telescope::ALI_SELECT_STAR_2; break;
      case 3: telInfo.align = Telescope::ALI_SELECT_STAR_3; break;
      case 4: telInfo.align = Telescope::ALI_SELECT_STAR_4; break;
      case 5: telInfo.align = Telescope::ALI_SELECT_STAR_5; break;
      case 6: telInfo.align = Telescope::ALI_SELECT_STAR_6; break;
      case 7: telInfo.align = Telescope::ALI_SELECT_STAR_7; break;
      case 8: telInfo.align = Telescope::ALI_SELECT_STAR_8; break;
      case 9: telInfo.align = Telescope::ALI_SELECT_STAR_9; break;
      default: telInfo.align = Telescope::ALI_SELECT_STAR_1; break;
    }
  }
}

#ifndef SHC_ON
void SmartHandController::menuPier()
{
  uint8_t choice = ((uint8_t)telInfo.getPierState());
  choice = display->UserInterfaceSelectionList(&buttonPad, "Set Side of Pier", choice, "East\nWest");
  bool ok = false;
  if (choice)
  {
    if (choice == 1)
      ok = DisplayMessageLX200(SetLX200(":SmE#"),false);
    else
      ok = DisplayMessageLX200(SetLX200(":SmW#"),false);
    if (ok)
    {
      DisplayMessage("Please Sync", "with a Target", 1000);
      menuSyncGoto(true);
      current_selection_L1 = 0;
      current_selection_L0 = 0;
      DisplayMessageLX200(SetLX200(":SmN#"),false);
    }
  }
}
#endif

void SmartHandController::menuDisplay()
{
  const char *string_list_Display = "Turn Off\nContrast\nDim Timeout\nBlank Timeout";
  current_selection_L2 = 1;
  while (current_selection_L2 != 0)
  {
    current_selection_L2 = display->UserInterfaceSelectionList(&buttonPad, "Display", current_selection_L2, string_list_Display);
    switch (current_selection_L2)
    {
    case 1:
      DisplayMessage("Press any key", "to turn on", 1500);
      sleepDisplay = true;
      display->sleepOn();
      current_selection_L2 = 0;
      current_selection_L1 = 0;
      current_selection_L0 = 0;
      break;
    case 2:
      menuContrast();
      break;
    case 3:
      menuDimTimeout();
      break;
    case 4:
      menuBlankTimeout();
      break;
    default:
      break;
    }
  }
}

void SmartHandController::menuSound()
{
  boolean sound = false;
  if (display->UserInterfaceInputValueBoolean(&buttonPad, "Buzzer On?", &sound)) {
    if (sound) DisplayMessageLX200(SetLX200(":SX97,1#"),false); else DisplayMessageLX200(SetLX200(":SX97,0#"),false);
  }
}

void SmartHandController::menuMeridianFlips()
{
  const char *string_list = "Now!\n""Automatic\n""Pause at Home";
  current_selection_L2 = 1;
  while (current_selection_L2 != 0)
  {
    boolean autoflip = false;
    boolean pause = false;
    current_selection_L2 = display->UserInterfaceSelectionList(&buttonPad, "Meridian Flip", current_selection_L2, string_list);
    switch (current_selection_L2)
    {
    case 1:
      DisplayMessageLX200(SetLX200(":MN#"),false);
      break;
    case 2:
      if (display->UserInterfaceInputValueBoolean(&buttonPad, "Auto Flip?", &autoflip)) {
        if (autoflip) DisplayMessageLX200(SetLX200(":SX95,1#"),false); else DisplayMessageLX200(SetLX200(":SX95,0#"),false);
      }
      break;
    case 3:
      if (display->UserInterfaceInputValueBoolean(&buttonPad, "Pause Flip?", &pause)) {
        if (pause) DisplayMessageLX200(SetLX200(":SX97,1#"),false); else DisplayMessageLX200(SetLX200(":SX97,0#"),false);
      }
      break;
    default:
      break;
    }
  }
}

void SmartHandController::menuTracking()
{
  Telescope::TrackState currentstate = telInfo.getTrackingState();

  if (telInfo.isMountAltAz()) {
    const char *string_list;
    current_selection_L2 = 1;
    while (current_selection_L2 != 0)
    {
      if (currentstate == Telescope::TRK_ON) {
        string_list = "Stop\n""Sidereal\n""Solar\n""Lunar\n""Rate Reset\n""Rate +\n""Rate -";
      } else {
        string_list = "Start\n""Sidereal\n""Solar\n""Lunar\n""Rate Reset\n""Rate +\n""Rate -";
      }
      current_selection_L2 = display->UserInterfaceSelectionList(&buttonPad, "Tracking", current_selection_L2, string_list);
      switch (current_selection_L2)
      {
      case 1:
        if (currentstate == Telescope::TRK_ON) {
          char out[20]=""; if (SetLX200(":Td#")== LX200VALUESET) { DisplayMessage("Tracking", "OFF", 500); currentstate=Telescope::TRK_OFF; } else DisplayMessage("Set State", "Failed", 1000);
        } else {
          char out[20]=""; if (SetLX200(":Te#")== LX200VALUESET) { DisplayMessage("Tracking", "ON", 500); currentstate=Telescope::TRK_ON; } else DisplayMessage("Set State", "Failed", 1000);
        }
      break;
      case 2: DisplayMessageLX200(SetLX200(":TQ#"),false); break;
      case 3: DisplayMessageLX200(SetLX200(":TS#"),false); break;
      case 4: DisplayMessageLX200(SetLX200(":TL#"),false); break;
      case 5: DisplayMessageLX200(SetLX200(":TR#"),false); break;
      case 6: DisplayMessageLX200(SetLX200(":T+#"),false); break;
      case 7: DisplayMessageLX200(SetLX200(":T-#"),false); break;
      }
    }
  } else {
    const char *string_list;
    current_selection_L2 = 1;
    while (current_selection_L2 != 0)
    {
      if (currentstate == Telescope::TRK_ON) {
        string_list = "Stop\n""Sidereal\n""Solar\n""Lunar\n""Comp Full\n""Comp Refr\n""Comp Off\n""Comp Sngl Ax\n""Comp Dual Ax\n""Rate Reset\n""Rate +0.02Hz\n""Rate -0.02Hz";
      } else {
        string_list = "Start\n""Sidereal\n""Solar\n""Lunar\n""Comp Full\n""Comp Refr\n""Comp Off\n""Comp Sngl Ax\n""Comp Dual Ax\n""Rate Reset\n""Rate +0.02Hz\n""Rate -0.02Hz";
      }
      current_selection_L2 = display->UserInterfaceSelectionList(&buttonPad, "Tracking", current_selection_L2, string_list);
      switch (current_selection_L2)
      {
      case 1:
        if (currentstate == Telescope::TRK_ON) {
          char out[20]=""; if (SetLX200(":Td#")== LX200VALUESET) { DisplayMessage("Tracking", "OFF", 500); currentstate=Telescope::TRK_OFF; } else DisplayMessage("Set State", "Failed", 1000);
        } else {
          char out[20]=""; if (SetLX200(":Te#")== LX200VALUESET) { DisplayMessage("Tracking", "ON", 500); currentstate=Telescope::TRK_ON; } else DisplayMessage("Set State", "Failed", 1000);
        }
      break;
      case 2:  DisplayMessageLX200(SetLX200(":TQ#"),false); break;
      case 3:  DisplayMessageLX200(SetLX200(":TS#"),false); break;
      case 4:  DisplayMessageLX200(SetLX200(":TL#"),false); break;
      case 5:  DisplayMessageLX200(SetLX200(":To#"),false); break;
      case 6:  DisplayMessageLX200(SetLX200(":Tr#"),false); break;
      case 7:  DisplayMessageLX200(SetLX200(":Tn#"),false); break;
      case 8:  DisplayMessageLX200(SetLX200(":T1#"),false); break;
      case 9:  DisplayMessageLX200(SetLX200(":T2#"),false); break;
      case 10: DisplayMessageLX200(SetLX200(":TR#"),false); break;
      case 11: DisplayMessageLX200(SetLX200(":T+#"),false); break;
      case 12: DisplayMessageLX200(SetLX200(":T-#"),false); break;
      }
    }
  }
}

#include "MenusConfiguration.h"
#include "MenusMount.h"

void SmartHandController::menuWifi()
{
  const char *string_list = (wifiOn) ? "Wifi off\nShow Password\nReset to factory" : "wifi on\nShow Password\nReset to factory";
  current_selection_L2 = 1;
  while (current_selection_L2 != 0)
  {
    current_selection_L2 = display->UserInterfaceSelectionList(&buttonPad, "Wifi", 1, string_list);
    switch (current_selection_L2)
    {
    case 1:
      wifiOn = !wifiOn;
      DisplayMessage("Please", "Reboot!", 3000);
      current_selection_L2 = 0;
      current_selection_L1 = 0;
      current_selection_L0 = 0;
      powerCylceRequired = true;
      break;
    case 2:
      DisplayMessage("masterPassword is", "password", 1000);
    case 3:
      DisplayMessage("Please", "Reboot!", 3000);
      current_selection_L2 = 0;
      current_selection_L1 = 0;
      current_selection_L0 = 0;
      powerCylceRequired = true;
    default:
      break;
    }
  }
}

void SmartHandController::menuSite()
{
  current_selection_L2 = 1;
  while (current_selection_L2 != 0)
  {
    const char *string_list_SiteL2 = "Select Site\n""Latitude\n""Longitude\n""UTC Offset";
    current_selection_L2 = display->UserInterfaceSelectionList(&buttonPad, "Menu Site", current_selection_L2, string_list_SiteL2);
    switch (current_selection_L2)
    {
    case 1: menuSites(); break;
    case 2: menuLatitude(); break;
    case 3: menuLongitude(); break;
    case 4: menuZone(); break;
    default: break;
    }
  }
}

void SmartHandController::menuSites()
{
  char out[20];
  int val;

  if (DisplayMessageLX200(GetSiteLX200(val)))
  {
    current_selection_L3 = val;
    const char *string_list_SiteL3 = "Site 1\n""Site 2\n""Site 3\n""Site 4";
    current_selection_L3 = display->UserInterfaceSelectionList(&buttonPad, "Menu Sites", current_selection_L3, string_list_SiteL3);
    if (current_selection_L3 != 0)
    {
      val = current_selection_L3 - 1;
      SetSiteLX200(val);
    }
  }
}

void SmartHandController::menuLatitude()
{
  char out[20];
  if (DisplayMessageLX200(GetLX200(":Gt#", out)))
  {
    char* pEnd;
    int degree = (int)strtol(&out[0], &pEnd, 10);
    int minute = (int)strtol(&out[4], &pEnd, 10);
    long angle = degree * 60;
    degree > 0 ? angle += minute : angle -= minute;
    angle *= 60;
    if (display->UserInterfaceInputValueLatitude(&buttonPad, &angle))
    {
      angle /= 60;
      minute = abs(angle % 60);
      degree = angle / 60;
      sprintf(out, ":St%+03d*%02d#", degree, minute);
      DisplayMessageLX200(SetLX200(out),false);
    }
  }
}

void SmartHandController::menuLongitude()
{
  char out[20];
  if (DisplayMessageLX200(GetLX200(":Gg#", out)))
  {
    char* pEnd;
    int degree = (int)strtol(&out[0], &pEnd, 10);
    int minute = (int)strtol(&out[5], &pEnd, 10);
    long angle = degree * 60;
    degree > 0 ? angle += minute : angle -= minute;
    angle *= 60;
    if (display->UserInterfaceInputValueLongitude(&buttonPad, &angle))
    {
      angle /= 60;
      minute = abs(angle) % 60;
      degree = angle / 60;
      sprintf(out, ":Sg%+04d*%02d#", degree, minute);
      DisplayMessageLX200(SetLX200(out),false);
    }
  }
}

void SmartHandController::menuZone()
{
  char out[20];
  if (DisplayMessageLX200(GetLX200(":GG#", out)))
  {
    char* pEnd;
    int hr = (int)strtol(&out[0], &pEnd, 10);

    boolean negative=false;
    if (hr<0) negative=true;
    uint8_t b=abs(hr);
  
    if (display->UserInterfaceInputValueInteger(&buttonPad, "UTC Ofs (-Zone)", "", &b, 0, 14, 2, " hrs"))
    {
      if (display->UserInterfaceInputValueBoolean(&buttonPad, "UTC Ofs - ?", &negative)) {
        hr=b;
        if (negative) hr=-hr;
        sprintf(out, ":SG%+02d#", hr);
        DisplayMessageLX200(SetLX200(out),false);
      }
    }
  }
}

void SmartHandController::menuContrast()
{
  const char *string_list_Display = "Min\nLow\nHigh\nMax";
  current_selection_L3 = 1;

  current_selection_L3 = display->UserInterfaceSelectionList(&buttonPad, "Set Contrast", current_selection_L3, string_list_Display);
  if (current_selection_L3 > 0)
  {
    maxContrast = (uint)63 * (current_selection_L3 - 1);
    //EEPROM.write(14, maxContrast);
    //EEPROM.commit();
    display->setContrast(maxContrast);
  }
}

void SmartHandController::menuDimTimeout()
{
  const char *string_list_Display = "Disable\n30 sec\n60 sec";
  current_selection_L3 = 2;

  if (current_selection_L3 > 0)
  {
    current_selection_L3 = display->UserInterfaceSelectionList(&buttonPad, "Dim Timeout", current_selection_L3, string_list_Display);
    display_dim_time = (current_selection_L3 - 1) * 30000;
    //EEPROM.writeLong(16, display_dim_time);
    //EEPROM.commit();
  }
}

void SmartHandController::menuBlankTimeout()
{
  const char *string_list_Display = "Disable\n1 min\n2 min\n3 min\n4 min\n5 min";
  current_selection_L3 = 3;

  if (current_selection_L3 > 0)
  {
    current_selection_L3 = display->UserInterfaceSelectionList(&buttonPad, "Blank Timeout", current_selection_L3, string_list_Display);
    display_blank_time = (current_selection_L3 - 1) * 60 * 1000;
    //EEPROM.writeLong(20, display_blank_time);
    //EEPROM.commit();
  }
}

void SmartHandController::DisplayMessage(const char* txt1, const char* txt2, int duration)
{
  uint8_t x;
  uint8_t y = 40;
  display->firstPage();
  do {
    if (txt2 != NULL)
    {
      y = 50;
      x = (display->getDisplayWidth() - display->getStrWidth(txt2)) / 2;
      display->drawStr(x, y, txt2);
      y = 25;
    }
    x = (display->getDisplayWidth() - display->getStrWidth(txt1)) / 2;
    display->drawStr(x, y, txt1);
  } while (display->nextPage());
  if (duration >= 0) delay(duration); else { buttonPad.waitForPress(); buttonPad.clearAllPressed(); }
}

void SmartHandController::DisplayLongMessage(const char* txt1, const char* txt2, const char* txt3, const char* txt4, int duration)
{
  display->setFont(u8g2_font_helvR10_tr);
  uint8_t h = 15;
  uint8_t x = 0;
  uint8_t y = h;
  display->firstPage();
  do {

    y = h;
    x = (display->getDisplayWidth() - display->getStrWidth(txt1)) / 2;
    display->drawStr(x, y, txt1);
    y += h;
    if (txt2 != NULL)
    {
      x = 0;
      display->drawStr(x, y, txt2);
    }
    else
    {
      y -= 7;
    }
    y += 15;
    if (txt3 != NULL)
    {
      x = 0;
      display->drawStr(x, y, txt3);
    }

    y += 15;
    if (txt4 != NULL)
    {
      x = 0;
      display->drawStr(x, y, txt4);
    }
  } while (display->nextPage());
  if (duration >= 0) delay(duration); else { buttonPad.waitForPress(); buttonPad.clearAllPressed(); }

  display->setFont(u8g2_font_helvR12_te);
}

bool SmartHandController::DisplayMessageLX200(LX200RETURN val, bool silentOk)
{
  char text1[20] = "";
  char text2[20] = "";
  int time = -1;
  if (val < LX200OK)
  {
    if (val == LX200NOTOK)
    {
      sprintf(text1, "LX200 Command");
      sprintf(text2, "has failed!");
    }
    else if (val == LX200SETVALUEFAILED)
    {
      sprintf(text1, "Set Value");
      sprintf(text2, "has failed!");
    }
    else if (val == LX200GETVALUEFAILED)
    {
      sprintf(text1, "Get Value");
      sprintf(text2, "has failed!");
    }
    else if (val == LX200SYNCFAILED)
    {
      sprintf(text1, "Sync");
      sprintf(text2, "has failed!");
    }
    else if (val == LX200SETTARGETFAILED)
    {
      sprintf(text1, "Set Target");
      sprintf(text2, "has failed!");
    }
    else if (val == LX200BELOWHORIZON)
    {
      sprintf(text1, "Target is");
      sprintf(text2, "Below Horizon!");
    }
    else if (val == LX200NOOBJECTSELECTED)
    {
      sprintf(text1, "No Object");
      sprintf(text2, "Selected!");
    }
    else if (val == LX200PARKED)
    {
      sprintf(text1, "Telescope");
      sprintf(text2, "is Parked!");
    }
    else if (val == LX200BUSY)
    {
      sprintf(text1, "Telescope");
      sprintf(text2, "is busy!");
    }
    else if (val == LX200LIMITS)
    {
      sprintf(text1, "Target");
      sprintf(text2, "outside limits");
    }
    else if (val == LX200UNKOWN)
    {
      sprintf(text1, "Unkown");
      sprintf(text2, "Error");
    }
    else
    {
      sprintf(text1, "Error");
      sprintf(text2, "-1");
    }
    DisplayMessage(text1, text2, -1);
  }
  else if (!silentOk)
  {
    time = 1000;
    if (val == LX200OK)
    {
      sprintf(text1, "LX200 Command");
      sprintf(text2, "Done!");
    }
    else if (val == LX200VALUESET)
    {
      sprintf(text1, "Value");
      sprintf(text2, "Set!");
    }
    else if (val == LX200GETVALUEFAILED)
    {
      sprintf(text1, "Value");
      sprintf(text2, "Get!");
    }
    else if (val == LX200SYNCED)
    {
      sprintf(text1, "Telescope");
      sprintf(text2, "Synced!");
    }
    else if (LX200GOINGTO)
    {
      sprintf(text1, "Slew to");
      sprintf(text2, "Target");
    }
    DisplayMessage(text1, text2, time);
  }
  return isOk(val);
}
