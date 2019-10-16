// -----------------------------------------------------------------------------------
// Control Trinamic SPI mode stepper drivers

#pragma once

class tmcSpiDriver {
  public:
    long sgResult;
    
    tmcSpiDriver(int cs, int sck, int miso, int mosi) {
      _cs=cs; _sck=sck; _miso=miso; _mosi=mosi;
    }

    // ----------------------------------------------------------------------------------------------------------------------
    // TMC setup most common settings
    // 256x interpolation:   intpol
    // decay mode:           decay_mode (STEALTHCHOP or SPREADCYCLE)
    // microstepping mode:   micro_step_mode (0=256x, 1=128x, 2=64x, 3=32x, 4=16x, 5=8x, 6=4x, 7=2x, 8=1x)
    // irun, ihold, rsense:  current in mA and sense resistor value
    void setup(bool intpol, int decay_mode, byte micro_step_mode, int irun, int ihold, float rsense) {
      BBSpi.begin(_cs,_sck,_miso,_mosi);
      uint32_t data_out=0;

      // voltage on AIN is current reference
      data_out=0x00000001UL;
      // set stealthChop bit
      if (decay_mode == STEALTHCHOP) data_out |= 0x00000004UL;
      if (last_GCONF != data_out) {
        last_GCONF=data_out;
        write(REG_GCONF,data_out);
        BBSpi.pause();
      }

      // *** My notes are limited, see the TMC2130 datasheet for more info. ***
    
      // IHOLDDELAY=0x00, IRUN=0x1F, IHOLD=0x1F (  0,   31,   31   ) or 50% (0,16,16)
      //                                         0b0000 11111 11111
      // IHOLD,      default=16, range 0 to 31 (Standstill current 0=1/32... 31=32/32)
      // IRUN,       default=31, range 0 to 31 (Run current 0=1/32... 31=32/32)
      // IHOLDDELAY, default=4,  range 0 to 15 (Delay per current reduction step in x 2^18 clocks)
      
      float Ifs = 0.325/rsense;
      unsigned long IHOLD=round(( ((float)ihold/1000.0)/Ifs)*32.0)-1;
      unsigned long IRUN =round(( ((float)irun/1000.0)/Ifs)*32.0)-1;
      if (IHOLD < 0) IHOLD=0; if (IHOLD > 31) IHOLD=31;
      if (IRUN < 0)  IRUN=0;  if (IRUN > 31)  IRUN=31;

      //        IHOLD    + IRUN    + IHOLDDELAY
      data_out=(IHOLD<<0)+(IRUN<<8)+(4UL<<16);
      if (last_IHOLD_IRUN != data_out) {
        last_IHOLD_IRUN=data_out;
        write(REG_IHOLD_IRUN,data_out);
        BBSpi.pause();
      }

      // TPOWERDOWN, default=127, range 0 to 255 (Delay after standstill for motor current power down, about 0 to 4 seconds)
      data_out=(_tpd_value<<0);
      if (last_TPOWERDOWN != data_out) {
        last_TPOWERDOWN = data_out;
        write(REG_TPOWERDOWN,data_out);
        BBSpi.pause();
      }

      // TPWMTHRS, default=0, range 0 to 2^20 (switchover upper velocity for stealthChop voltage PWM mode)
      data_out=(_tpt_value<<0);
      if (last_TPWMTHRS != data_out) {
        last_TPWMTHRS = data_out;
        write(REG_TPWMTHRS,data_out);
        BBSpi.pause();
      }

      // THIGH, default=0, range 0 to 2^20 (switchover rate for vhighfs/vhighchm)
      data_out=(_thigh_value<<0);
      if (last_THIGH != data_out) {
        last_THIGH = data_out;
        write(REG_THIGH,data_out);
        BBSpi.pause();
      }

      // PWMCONF
      // default=0x00050480UL;
      data_out    =(_pc_PWM_AMPL<<0)+(_pc_PWM_GRAD<<8)+(_pc_pwm_freq<<16)+(_pc_pwm_auto<<18)+(_pc_pwm_sym<<19)+(_pc_pwm_freewheel<<20);
      if (last_PWMCONF != data_out) {
        last_PWMCONF = data_out;
        write(REG_PWMCONF,data_out);
        BBSpi.pause();
      }

      // CHOPCONF
      // native 256 microsteps, mres=0, tbl=1=24, toff=8 ( data_out=0x00008008UL; )
      data_out=(_cc_toff<<0)+(_cc_hstart<<4)+(_cc_hend<<7)+(_cc_rndtf<<13)+(_cc_tbl<<15)+(_cc_vsense<<17)+(_cc_vhighfs<<18)+(_cc_vhighchm<<19);
      // set the interpolation bit
      if (intpol) data_out |= 1UL<<28;
      // set the micro-step mode bits
      data_out |= ((uint32_t)micro_step_mode)<<24;
      write(REG_CHOPCONF,data_out);

      BBSpi.end();
    }

    bool error() {
      BBSpi.begin(_cs,_sck,_miso,_mosi);

      // get global status register, look for driver error bit
      uint32_t data_out=0;
      uint8_t result=read(REG_GSTAT,&data_out);

      BBSpi.end();
      if ((result&2) != 0) return true; else return false;
    }

// -------------------------------
// CHOPCONF settings

    bool set_CHOPCONF_toff(int v)     { if ((v >= 2) && (v <= 15)) { _cc_toff    = v; return true; } return false; }
    bool set_CHOPCONF_hstart(int v)   { if ((v >= 0) && (v <= 7))  { _cc_hstart  = v; return true; } return false; }
    bool set_CHOPCONF_hend(int v)     { if ((v >= 0) && (v <= 15)) { _cc_hend    = v; return true; } return false; }
    bool set_CHOPCONF_rndtf(int v)    { if ((v >= 0) && (v <= 1))  { _cc_rndtf   = v; return true; } return false; }
    bool set_CHOPCONF_tbl(int v)      { if ((v >= 0) && (v <= 3))  { _cc_tbl     = v; return true; } return false; }
    bool set_CHOPCONF_vsense(int v)   { if ((v >= 0) && (v <= 1))  { _cc_vsense  = v; return true; } return false; }
    bool set_CHOPCONF_vhighfs(int v)  { if ((v >= 0) && (v <= 1))  { _cc_vhighfs = v; return true; } return false; }
    bool set_CHOPCONF_vhignchm(int v) { if ((v >= 0) && (v <= 1))  { _cc_vhighchm= v; return true; } return false; }

// -------------------------------
// TPOWERDOWN setting
    bool set_TPOWERDOWN_value(int v)  { if ((v >= 0) && (v <= 255)) { _tpd_value = v; return true; } return false; }

// -------------------------------
// TPWMTHRS setting
    bool set_TPWMTHRS_value(int v)    { if ((v >= 0) && (v <= 1048575)) { _tpt_value = v; return true; } return false; }

// -------------------------------
// THIGH setting
    bool set_THIGH_value(int v)       { if ((v >= 0) && (v <= 1048575)) { _thigh_value = v; return true; } return false; }

// -------------------------------
// PWMCONF settings

    bool set_PWMCONF_PWM_AMPL(int v) { if ((v >= 0) && (v <= 255))    { _pc_PWM_AMPL     = v; return true; } return false; }
    bool set_PWMCONF_PWM_GRAD(int v) { if ((v >= 0) && (v <= 255))    { _pc_PWM_GRAD     = v; return true; } return false; }
    bool set_PWMCONF_pwm_freq(int v) { if ((v >= 0) && (v <= 3))      { _pc_pwm_freq     = v; return true; } return false; }
    bool set_PWMCONF_pwm_auto(int v) { if ((v >= 0) && (v <= 1))      { _pc_pwm_auto     = v; return true; } return false; }
    bool set_PWMCONF_pwm_sym(int v)  { if ((v >= 0) && (v <= 1))      { _pc_pwm_sym      = v; return true; } return false; }
    bool set_PWMCONF_pwm_freewheel(int v) { if ((v >= 0) && (v <= 1)) { _pc_pwm_freewheel= v; return true; } return false; }
    
// ----------------------------------------------------------------------------------------------------------------------
// DRVSTATUS

    int refresh_DRVSTATUS() {
      BBSpi.begin(_cs,_sck,_miso,_mosi);
      // get global status register, look for driver error bit
      uint32_t data_out=0;
      read(REG_DRVSTATUS,&data_out);
      
      BBSpi.pause();
      
      // first write returns nothing, second the status data
      data_out=0;
      read(REG_DRVSTATUS,&data_out);

      // get the extended status info.
      _stst=(bool)bitRead(data_out,31);      // DRV_STATUS 31 Standstill
      _olb =(bool)bitRead(data_out,30);      // DRV_STATUS 30 Open Load B
      _ola =(bool)bitRead(data_out,29);      // DRV_STATUS 29 Open Load A
      _s2ga=(bool)bitRead(data_out,28);      // DRV_STATUS 28 Short to Ground B
      _s2gb=(bool)bitRead(data_out,27);      // DRV_STATUS 27 Short to Ground A
      _otpw=(bool)bitRead(data_out,26);      // DRV_STATUS 26 Overtemperature Pre-warning 120C
      _ot  =(bool)bitRead(data_out,25);      // DRV_STATUS 25 Overtemperature Shutdown 150C
      _stallGuard=(bool)bitRead(data_out,24);// DRV_STATUS 24 Overtemperature Shutdown 150C
      _CS_ACTUAL=(data_out>>16)&0b011111;    // DRV_STATUS 16 stallGuard2 status
      _fsactive =(bool)bitRead(data_out,15); // DRV_STATUS 15 Full step active indicator
      _SG_RESULT=data_out&0b1111111111;      // DRV_STATUS  0 stallGuard2 result

      BBSpi.end();
      return sgResult;
    }

    bool get_DRVSTATUS_STST() { return _stst; }
    bool get_DRVSTATUS_OLa()  { return _ola; }
    bool get_DRVSTATUS_OLb()  { return _olb; }
    bool get_DRVSTATUS_S2Ga() { return _s2ga; }
    bool get_DRVSTATUS_S2Gb() { return _s2gb; }
    bool get_DRVSTATUS_OTPW() { return _otpw; }
    bool get_DRVSTATUS_OT()   { return _ot; }
    bool get_DRVSTATUS_stallGuard() { return _stallGuard; }
    int get_DRVSTATUS_CS_ACTUAL() { return _CS_ACTUAL; }
    bool get_DRVSTATUS_fsactive() { return _fsactive; }
    int get_DRVSTATUS_SG_RESULT() { return _SG_RESULT; }

// ----------------------------------------------------------------------------------------------------------------------
// COOLCONF

    bool refresh_COOLCONF() {
      BBSpi.begin(_cs,_sck,_miso,_mosi);
      uint32_t data_out=(_ccf_semin<<0)+(_ccf_seup<5)+(_ccf_semax<<8)+(_ccf_sedn<<13)+(_ccf_seimin<<15)+(_ccf_sgt<<16)+(_ccf_sfilt<<24);
      write(REG_COOLCONF,data_out);
      BBSpi.end();
      return true;
    }

    bool set_COOLCONF_semin(int v)  { if ((v >= 0) && (v <= 15))   { _ccf_semin =v; return true; } return false; }
    bool set_COOLCONF_seup(int v)   { if ((v >= 0) && (v <= 3))    { _ccf_seup  =v; return true; } return false; }
    bool set_COOLCONF_semax(int v)  { if ((v >= 0) && (v <= 15))   { _ccf_semax =v; return true; } return false; }
    bool set_COOLCONF_sedn(int v)   { if ((v >= 0) && (v <= 3))    { _ccf_sedn  =v; return true; } return false; }
    bool set_COOLCONF_seimin(int v) { if ((v >= 0) && (v <= 1))    { _ccf_seimin=v; return true; } return false; }
    bool set_COOLCONF_sgt(int v)    { if ((v >= -64) && (v <= 63)) { _ccf_sgt=v+64; return true; } return false; }
    bool set_COOLCONF_sfilt(int v)  { if ((v >= 0) && (v <= 1))    { _ccf_sfilt =v; return true; } return false; }

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

// keep track of what registers need to be updated
    unsigned long last_GCONF      = 0;
    unsigned long last_IHOLD_IRUN = 0;
    unsigned long last_TPOWERDOWN = 0;
    unsigned long last_TPWMTHRS   = 0;
    unsigned long last_THIGH      = 0;
    unsigned long last_PWMCONF    = 0;

// CHOPCONF settings
    unsigned long _cc_toff      = 4UL; // default=4,  range 2 to 15 (Off time setting, slow decay phase)
    unsigned long _cc_hstart    = 0UL; // default=0,  range 0 to 7  (Hysteresis start 1, 2, ..., 8)
    unsigned long _cc_hend      = 0UL; // default=0,  range 0 to 15 (Hysteresis -3, -2, -1, 0, 1 ..., 12)
    unsigned long _cc_rndtf     = 0UL; // default=0,  range 0 to 1  (Enables small random value to be added to TOFF)
    unsigned long _cc_tbl       = 1UL; // default=1,  range 0 to 3  (for 6, 24, 36 or 54 clocks)
    unsigned long _cc_vsense    = 0UL; // default=0,  range 0 to 1  (0 for high sensitivity, 1 for low sensitivity @ 50% current setting)
    unsigned long _cc_vhighfs   = 0UL; // default=0,  range 0 to 1  (Enables switch to full-step when VHIGH (THIGH?) is exceeded)
    unsigned long _cc_vhighchm  = 0UL; // default=0,  range 0 to 1  (Enables switch to fast-decay mode VHIGH (THIGH?) is exceeded)

// TPOWERDOWN settings
    unsigned long _tpd_value    = 128; // default=127,range 0 to 255 (Delay after standstill for motor current power down, about 0 to 4 seconds)

// TPWMTHRS settings
    unsigned long _tpt_value      = 0; // default=0,  range 0 to 2^20 (Switchover upper velocity for stealthChop voltage PWM mode)

// THIGH settings
    unsigned long _thigh_value    = 0; // default=0,  range 0 to 2^20 (Switchover rate for vhighfs/vhighchm)

// PWMCONF settings
    unsigned long _pc_PWM_AMPL    = 0; // default=0,  range 0 to 255 (PWM amplitude or switch back amplitude if pwm_auto=1)
    unsigned long _pc_PWM_GRAD    = 0; // default=0,  range 0 to 255 (PWM gradient)
    unsigned long _pc_pwm_freq    = 0; // default=0,  range 0 to 3   (PWM frequency 0: fpwm=2/1024 fclk, 1: fpwm=2/683 fclk, 2: fpwm=2/512 fclk, 3: fpwm=2/410 fclk)
    unsigned long _pc_pwm_auto    = 0; // default=0,  range 0 to 1   (PWM automatic current control)
    unsigned long _pc_pwm_sym     = 0; // default=0,  range 0 to 1   (PWM symmetric 0: value may change during cycle, 1: enforce)
    unsigned long _pc_pwm_freewheel=0; // default=0,  range 0 to 1   (PWM freewheel 0: normal, 1: freewheel, 2:LS short, 3: HS short)

// DRVSTATUS settings
    int _sg_result = 0;
    bool _stst = false;
    bool _olb = false;
    bool _ola = false;
    bool _s2ga = false;
    bool _s2gb = false;
    bool _ot = false;
    bool _otpw = false;
    bool _stallGuard = false;
    int _CS_ACTUAL = 0;
    bool _fsactive = false;
    int _SG_RESULT = 0;

// COOLCONF settings
    unsigned long _ccf_semin    = 0UL; // default=0, range 0 to 15  (Minimum stallGuard2 value for smart current control and smart current enable)
    unsigned long _ccf_seup     = 0UL; // default=0, range 0 to 3   (Current up step width)
    unsigned long _ccf_semax    = 0UL; // default=0, range 0 to 15  (stallGuard2 hysteresis value for smart current control)
    unsigned long _ccf_sedn     = 0UL; // default=0, range 0 to 3   (Current down step speed)
    unsigned long _ccf_seimin   = 0UL; // default=0, range 0 to 1   (Minimum current for scc)
    unsigned long _ccf_sgt      = 0UL; // default=0, range 0 to 127 (stallGuard2 hysteresis value -64 to 63)
    unsigned long _ccf_sfilt    = 0UL; // default=0, range 0 to 1   (stallGuard2 filter enable)
};
