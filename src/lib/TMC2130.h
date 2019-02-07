// -----------------------------------------------------------------------------------
// TMC2130 stepper drivers

#pragma once

class tmc2130 {
  public:
    long sgResult;
    
    tmc2130(int cs, int sck, int miso, int mosi) {
      _cs=cs; _sck=sck; _miso=miso; _mosi=mosi;
    }
    
#ifdef STALL_GUARD_ON
    // TMC2130 stallGuard ----------------------------------------------------------------
    // COOLCONF
    bool sgSetSgt(int8_t sgt) {
      BBSpi.begin(_cs,_sck,_miso,_mosi);
      // sgt,    default=0, range -64 to +63                         (This  signed  value  controls  stallGuard2  level  for  stall output  and  sets  the  optimum  measurement range for readout. 
      //                                    . | 2   .     1 .       . A lower value gives a higher sensitivity. Zero is the starting value working with most motors.)
      //                             10987654321098765432109876543210
      uint32_t data_out =(sgt<<16)&0b00000000011111110000000000000000UL; // mask sgt since it can be a negative value
               data_out|=          0b00000001000000000000000000000000UL; // turn on sfilt
      
      write(REG_COOLCONF,data_out);
      BBSpi.end();
      return true;
    }
     
    int sgUpdateResult() {
      BBSpi.begin(_cs,_sck,_miso,_mosi);
      // get global status register, look for driver error bit
      uint32_t data_out=0;
      read(REG_DRVSTATUS,&data_out);
      
      BBSpi.pause();
      
      // first write returns nothing, second the status data
      data_out=0;
      read(REG_DRVSTATUS,&data_out);
    
      // get the first 10 bits: SG_RESULT
      long r=data_out&0b1111111111;

      // apply a smoothing filter
      sgResult=round((((double)sgResult*4.0)+(double)r)/5.0);
      
      BBSpi.end();
      return sgResult;
    }
#endif 
   
    // TMC2130 setup ---------------------------------------------------------------------
    // 256x interpolation:  intpol
    // stealth chop on/off: stealth_chop
    // microstepping mode:  micro_step_mode (0=256x, 1=128x, 2=64x, 3=32x, 4=16x, 5=8x, 6=4x, 7=2x, 8=1x)
    // power level       :  low_power (true for low power: 50%, 100% otherwise)
    void setup(bool intpol, bool stealth_chop, byte micro_step_mode, bool low_power) {
      BBSpi.begin(_cs,_sck,_miso,_mosi);
      uint32_t data_out=0;
      uint32_t data_out_low=0;
    
      // voltage on AIN is current reference
      data_out=0x00000001UL;
      // set stealthChop bit
      if (stealth_chop) data_out|=0x00000004UL;
      write(REG_GCONF,data_out);
      BBSpi.pause();
    
      // *** My notes are limited, see the TMC2130 datasheet for more info. ***
    
      // IHOLDDELAY=0x00, IRUN=0x1F, IHOLD=0x1F (  0,   31,   31   ) or 50% (0,16,16)
      //                                         0b0000 11111 11111
      // IHOLD,      default=16, range 0 to 31 (Standstill current 0=1/32... 31=32/32)
      // IRUN,       default=31, range 0 to 31 (Run current 0=1/32... 31=32/32)
      // IHOLDDELAY, default=4,  range 0 to 15 (Delay per current reduction step in x 2^18 clocks)
      //            IHOLD   + IRUN    + IHOLDDELAY
      data_out_low=(12UL<<0)+(16UL<<8)+(4UL<<16);
      data_out    =(16UL<<0)+(31UL<<8)+(4UL<<16);
      if (!low_power) write(REG_IHOLD_IRUN,data_out); else write(REG_IHOLD_IRUN,data_out_low);
      BBSpi.pause();
    
      // TPOWERDOWN, default=127, range 0 to 255 (Delay after standstill for motor current power down, about 0 to 4 seconds)
      //            TPOWERDOWN
      data_out    =(127UL<<0);
      write(REG_TPOWERDOWN,data_out);
      BBSpi.pause();
    
    /*
      // Uncomment this for PWM configuration
      
      // TPWMTHRS, default=0, range 0 to 2^20 (switchover upper velocity for stealthChop voltage PWM mode)
      //            TPWMTHRS
      data_out    =(0UL<<0);
      write(REG_TPWMTHRS,data_out);
      spiPause();
    
      // THIGH, default=0, range 0 to 2^20 (switchover rate for vhighfs/vhighchm)
      //            THIGH
      data_out    =(0UL<<0);
      write(REG_THIGH,data_out);
      spiPause();
    
      // PWMCONF
      // default=0x00050480UL;
      // PWM_AMPL, default=128, range 0 to 255 (PWM amplitude or switch back amplitude if pwm_auto=1)
      // PWM_GRAD, default=4,   range 0 to 255 (PWM gradient)
      // pwm_freq, default=1,   range 0 to 3   (PWM frequency 0: fpwm=2/1024 fclk, 1: fpwm=2/683 fclk, 2: fpwm=2/512 fclk, 3: fpwm=2/410 fclk)
      // pwm_auto, default=1,   range 0 to 1   (PWM automatic current control)
      // pwm_sym,  default=0,   range 0 to 1   (PWM symmetric 0: value may change during cycle, 1: enforce)
      //            PWM_AMPL +PWM_GRAD+pwm_freq +pwm_auto +pwm_sym
      data_out    =(128UL<<0)+(4UL<<8)+(1UL<<16)+(1UL<<18)+(0UL<<19);
      write(REG_PWMCONF,data_out);
      spiPause();
    */

      #define _cc_toff 4UL     // default=4, range 2 to 15 (Off time setting, slow decay phase)
      #define _cc_hstart 0UL   // default=0, range 0 to 7  (Hysteresis start 1, 2, ..., 8)
      #define _cc_hend 0UL     // default=0, range 0 to 15 (Hysteresis -3, -2, -1, 0, 1 ..., 12)
      #define _cc_rndtf 0UL    // default=0, range 0 to 1  (Enables small random value to be added to TOFF)
      #define _cc_tbl 1UL      // default=1, range 0 to 3  (for 6, 24, 36 or 54 clocks)
      #define _cc_vsense 0UL   // default=0, range 0 to 1  (0 for high sensitivity, 1 for low sensitivity @ 50% current setting)
      #define _cc_vhighfs 0UL  // default=0, range 0 to 1  (Enables switch to full-step when VHIGH (THIGH?) is exceeded)
      #define _cc_vhighchm 0UL // default=0, range 0 to 1  (Enables switch to fast-decay mode VHIGH (THIGH?) is exceeded)

      // CHOPCONF
      // native 256 microsteps, mres=0, tbl=1=24, toff=8 ( data_out=0x00008008UL; )
      data_out=(_cc_toff<<0)+(_cc_hstart<<4)+(_cc_hend<<7)+(_cc_rndtf<<13)+(_cc_tbl<<15)+(_cc_vsense<<17)+(_cc_vhighfs<<18)+(_cc_vhighchm<<19);
      // set the interpolation bit
      if (intpol) data_out|=1UL<<28;
      // set the micro-step mode bits
      data_out|=((uint32_t)micro_step_mode)<<24;
      write(REG_CHOPCONF,data_out);

      BBSpi.end();
    }

    bool error() {
      BBSpi.begin(_cs,_sck,_miso,_mosi);

      // get global status register, look for driver error bit
      uint32_t data_out=0;
      uint8_t result=read(REG_GSTAT,&data_out);

      BBSpi.end();
      if ((result&2)!=0) return true; else return false;
    }
    
    int getStatus() {
      BBSpi.begin(_cs,_sck,_miso,_mosi);
      // get global status register, look for driver error bit
      uint32_t data_out=0;
      read(REG_DRVSTATUS,&data_out);
      
      BBSpi.pause();
      
      // first write returns nothing, second the status data
      data_out=0;
      read(REG_DRVSTATUS,&data_out);

      // get the extended status info.
      _stst=(bool)bitRead(data_out,31); // DRV_STATUS 31 Standstill
      _olb =(bool)bitRead(data_out,30); // DRV_STATUS 30 Open Load B
      _ola =(bool)bitRead(data_out,29); // DRV_STATUS 29 Open Load A
      _s2ga=(bool)bitRead(data_out,28); // DRV_STATUS 28 Short to Ground B
      _s2gb=(bool)bitRead(data_out,27); // DRV_STATUS 27 Short to Ground A
      _otpw=(bool)bitRead(data_out,26); // DRV_STATUS 26 Overtemperature Pre-warning 120C
      _ot  =(bool)bitRead(data_out,25); // DRV_STATUS 25 Overtemperature Shutdown 150C
      
      BBSpi.end();
      return sgResult;
    }

    // Standstill
    bool statusSTST() {
      return _stst;
    }

    // Open Load A
    bool statusOLa() {
      return _ola;
    }

    // Open Load B
    bool statusOLb() {
      return _olb;
    }

    // Short to Ground A
    bool statusS2Ga() {
      return _s2ga;
    }

    // Short to Ground B
    bool statusS2Gb() {
      return _s2gb;
    }

    // Overtemperature Shutdown 150C
    bool statusOT() {
      return _ot;
    }

    // Overtemperature Pre-warning 120C
    bool statusOTPW() {
      return _otpw;
    }

  private:
    uint8_t write(byte Address, uint32_t data_out)
    {
      Address=Address|0x80;
      uint8_t status_byte=BBSpi.transfer(Address);
      BBSpi.transfer32(data_out);
      return status_byte;
    }
    
    uint8_t read(byte Address, uint32_t* data_out)
    {
      Address=Address&~0x80;
      uint8_t status_byte=BBSpi.transfer(Address);
      *data_out=BBSpi.transfer32(*data_out);
      return status_byte;
    }

    bbspi BBSpi;

    int _cs;
    int _sck;
    int _miso;
    int _mosi;

    const static uint8_t WRITE          = 0x80; // write flag
    const static uint8_t REG_GCONF      = 0x00;
    const static uint8_t REG_GSTAT      = 0x01;
    const static uint8_t REG_IHOLD_IRUN = 0x10;
    const static uint8_t REG_TPOWERDOWN = 0x11;
    const static uint8_t REG_TPWMTHRS   = 0x13;
    const static uint8_t REG_THIGH      = 0x15;
    const static uint8_t REG_CHOPCONF   = 0x6C;
    const static uint8_t REG_COOLCONF   = 0x6D;
    const static uint8_t REG_DCCTRL     = 0x6E;
    const static uint8_t REG_DRVSTATUS  = 0x6F;
    const static uint8_t REG_PWMCONF    = 0x70;

    bool _stst = false;
    bool _olb = false;
    bool _ola = false;
    bool _s2ga = false;
    bool _s2gb = false;
    bool _ot = false;
    bool _otpw = false;
};
