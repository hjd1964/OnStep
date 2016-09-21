// -----------------------------------------------------------------------------------
// TMC2130 control over bit bit-banged SPI
#define WRITE          0x80 //write flag
#define REG_GCONF      0x00
#define REG_GSTAT      0x01
#define REG_IHOLD_IRUN 0x10
#define REG_CHOPCONF   0x6C
#define REG_COOLCONF   0x6D
#define REG_DCCTRL     0x6E
#define REG_DRVSTATUS  0x6F

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
// microstepping mode:  micro_step_mode (0 = 256x, 1= 128x, 2=64x, 3=32x, 4=16x, 5=8x, 6=4x, 7=2x, 8=1x)
void TMC2130_setup(bool intpol, bool stealth_chop, byte micro_step_mode) {
  uint32_t data_out=0;

  // voltage on AIN is current reference
  if (stealth_chop) data_out=0x00000003UL; else data_out=0x00000001UL;
  TMC2130_write(REG_GCONF,data_out);

  TMC2130_write(REG_IHOLD_IRUN,0x00001010UL); // IHOLD=0x10, IRUN=0x10 (16,16) 
  
  // native 256 microsteps, MRES=0, TBL=1=24, TOFF=8
  data_out=0x00008008UL;
  // set the interpolation bit
  if (intpol) bitWrite(data_out,28,1); else bitWrite(data_out,28,0);
  // set the micro-step mode bits
  data_out|=micro_step_mode<<24;
  TMC2130_write(REG_CHOPCONF,data_out);
}

bool TMC2130_error() {
  uint32_t data_out=0;

  // get global status register, look for driver error bit
  data_out=0;
  if (TMC2130_read(REG_GSTAT,&data_out)&2!=0) return true; else return false;
}

