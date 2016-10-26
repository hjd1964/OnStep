// -----------------------------------------------------------------------------------
// TMC2130 control over bit bit-banged SPI
#define WRITE          0x80 //write flag
#define REG_GCONF      0x00
#define REG_GSTAT      0x01
#define REG_IHOLD_IRUN 0x10
#define REG_TPOWERDOWN 0x11
#define REG_CHOPCONF   0x6C
#define REG_COOLCONF   0x6D
#define REG_DCCTRL     0x6E
#define REG_DRVSTATUS  0x6F
#define REG_PWMCONF    0x70

uint8_t TMC2130_write(byte Address, uint32_t data_out)
{
  Address=Address|0x80;
  uint8_t status_byte=spiTransfer(Address);
  spiTransfer32(data_out);
  return status_byte;
}

uint8_t TMC2130_read(byte Address, uint32_t* data_out)
{
  Address=Address&!0x80;
  uint8_t status_byte=spiTransfer(Address);
  *data_out=spiTransfer32(*data_out);
  return status_byte;
}

// TMC2130 setup:
// 256x interpolation:  intpol
// stealth chop on/off: stealth_chop
// microstepping mode:  micro_step_mode (0=256x, 1=128x, 2=64x, 3=32x, 4=16x, 5=8x, 6=4x, 7=2x, 8=1x)
// power level       :  low_power (true for low power: 50%, 100% otherwise)
void TMC2130_setup(bool intpol, bool stealth_chop, byte micro_step_mode, bool low_power) {
  uint32_t data_out=0;
  uint32_t data_out_low=0;

  // voltage on AIN is current reference
  data_out=0x00000001UL;
  // set stealthChop bit
  if (stealth_chop) data_out|=0x00000004UL; 
  TMC2130_write(REG_GCONF,data_out);
  spiPause();

  // *** My notes are limited, see TMC2130 spec. sheet for more info. ***

  // IHOLDDELAY=0x00, IRUN=0x1F, IHOLD=0x1F (  0,   31,   31   ) or 50% (0,16,16)
  //                                         0b0000 11111 11111
  // IHOLDDELAY, default=0,  range 0 to 15 (Delay per current reduction step in x 2^18 clocks)
  // IRUN,       default=31, range 0 to 31 (Run current 0=1/32... 31=32/32)
  // IHOLD,      default=16, range 0 to 31 (Standstill current 0=1/32... 31=32/32)
  //            IHOLD + IRUN  + IHOLDDELAY
  data_out_low=(16UL<<0)+(12UL<<8)+(0UL<<16);
  data_out    =(31UL<<0)+(16UL<<8)+(0UL<<16);
  if (!low_power) TMC2130_write(REG_IHOLD_IRUN,data_out); else TMC2130_write(REG_IHOLD_IRUN,data_out_low);
  spiPause();

  // TPOWERDOWN, default=127, range 0 to 255 (Delay after standstill for motor current power down, about 0 to 4 seconds)
  //            TPOWERDOWN
  data_out    =(127UL<<0);
  TMC2130_write(REG_TPOWERDOWN,data_out);
  spiPause();

  // native 256 microsteps, MRES=0, TBL=1=24, TOFF=8
  // data_out=0x00008008UL;
  // TOFF,  default=4, range 2 to 15
  // HSTRT, default=0, range 0 to 7  (Hysteresis start 1, 2, ..., 8)
  // HEND,  default=0, range 0 to 15 (Hysteresis -3, -2, -1, 0, 1 ..., 12)
  // TBL,   default=1, range 0 to 3  (for 6, 24, 36 or 54 clocks)
  //        TOFF + HSTRT + HEND + TBL
  data_out=(4UL<<0)+(0UL<<4) +(0UL<<7)+(1UL<<15);
  // set the interpolation bit
  if (intpol) data_out|=1UL<<28;
  // set the micro-step mode bits
  data_out|=((uint32_t)micro_step_mode)<<24;
  TMC2130_write(REG_CHOPCONF,data_out);
}

bool TMC2130_error() {
  uint32_t data_out=0;

  // get global status register, look for driver error bit
  uint8_t result=TMC2130_read(REG_GSTAT,&data_out);
  if ((result&2)!=0) return true; else return false;
}

