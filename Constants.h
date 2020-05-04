// -----------------------------------------------------------------------------------
// Constants

#pragma once
// Configuration options -----------------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------------------------------------
// Pinmaps -------------------------------------------------------------------------------------------------------------------------

#define PINMAP_FIRST                 1
#define Classic                      1 // Original pin-map
#define ClassicShield                2 // Original pin-map, for Steve's shield where ST4 port is on the Mega2560's "alternate pins"
#define AltMega2560                  3 // Obsolete

// RAMPS - 3D printer shields/boards that work with OnStep, all are 5-axis designs
#define Ramps14                     10 // Ramps v1.4 shield for Mega2560
#define Ramps15                     10 // Ramps v1.5
#define MksGenL                     10 // Like Ramps above but better, Mega2560 built-in, crystal oscillator, 24V support
#define MksGenL1                    10
#define MksGenL2                    11 // Adds SPI bus to all stepper drivers (TMC only)

// Mini - Small 2-axis design suitable for embedded or mounting behind a panel can even be built with connectors up
#define MiniPCB                     20 // All for Teensy3.2
#define MiniPCB13                   21 // MiniPCB v1.3 adds better support for ESP-01 flashing and optional I2C

// Mini2 - Small 2-axis design for aluminum mini-case
#define MiniPCB2                    30 // Teensy3.2

// Max - Larger 4-axis design suitable for embedded or mounting behind a panel
#define MaxPCB                      40 // First custom "full feature" board. Teensy3.5/Teensy3.6

// Max2 & Max3 - Larger 4-axis design for aluminum case
#define MaxPCB2                     41 // Improved "full feature" board, Teensy3.5/Teensy3.6
#define MaxPCB3                     42 // Adds SPI bus to all stepper drivers (TMC only,) flashes WeMos D1 Mini through OnStep

#define MaxESP2                     50 // Similar to MaxPCB2 except 3 axes and uses cheaper/more available ESP32 MCU
#define MaxESP3                     51 // Adds 4th axis and option to flash the WeMos D1 Mini WiFi through OnStep

// Arduino CNC Sheild on WeMos D2 R32 (ESP32)
#define CNC3                        52 // Similar features to MaxESP3

// Khalid and Dave's PCB for STM32 Blue pill
#define STM32Blue                   60
#define STM32Black                  61
#define PINMAP_LAST                 61
// ---------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------
                                   
// On/Off, etc.
#define OFF                        -1
#define ON                         -2
#define ON_PULLUP                  -3
#define ON_PULLDOWN                -4
#define XDIRECT                    -5
#define AUX                        -6

// mount types                     
#define MOUNT_TYPE_FIRST            1
#define GEM                         1 // German Equatorial Mount, meridian flips enabled
#define FORK                        2 // Fork Mount, meridian flips disabled
#define ALTAZM                      3 // Altitude Azimuth Mounts, Dobsonians, etc.
#define MOUNT_TYPE_LAST             3

// various Time and Location sources supported
#define TLS_FIRST                   1
#define DS3231                      1 // DS3231 RTC on I2C
#define DS3234                      2 // Alias for DS3234M
#define DS3234M                     2 // DS3234 RTC on SPI (DS3234_CS_PIN) Makuna library
#define DS3234S                     3 // DS3234 RTC on SPI (DS3234_CS_PIN) Sparkfun Library
#define TEENSY                      4 // TEENSY3.2 RTC (Built-in)
#define GPS                         5 // GPS device
#define TLS_LAST                    5

// various Weather sensors supported
#define WEATHER_FIRST               1
#define BME280                      1 // BME280 on I2C (at default address 0x77)
#define BME280_0x77                 1 // BME280 on I2C (at address 0x77)
#define BME280_0x76                 2 // BME280 on I2C (at address 0x76)
#define BME280_SPI                  3 // BME280 on SPI (default CS)
#define BMP280                      4 // BMP280 on I2C (at default address 0x77)
#define BMP280_0x77                 4 // BMP280 on I2C (at address 0x77)
#define BMP280_0x76                 5 // BMP280 on I2C (at address 0x76)
#define BMP280_SPI                  6 // BMP280 on SPI (default CS)
#define WEATHER_LAST                6

// various Dallas/Maxim devices supported
#define DS_MASK    0x3F00000000000000
#define DS1820     0x2800000000000000 // DS18B20 temperature sensors etc. for focusing and dew heaters
#define DS2413     0x3A00000000000000 // DS2413 GPIO pins for dew heaters
#define CHAIN      0x3A00000000000001

// various auxillary features
#define AUXILLARY_FIRST             1
#define SWITCH                      1
#define ANALOG                      2
#define DEW_HEATER                  3
#define INTERVALOMETER              4
#define AUXILLARY_LAST              4

// coordinate mode for getting and setting RA/Dec
#define OBSERVED_PLACE              1
#define TOPOCENTRIC                 2
#define ASTROMETRIC_J2000           3
#define TOPOCENTRIC_STRICT          OFF

// EEPROM Info ---------------------------------------------------------------------------------------------------------------------
// General purpose storage A (100 bytes), 0..99

#define EE_posAxis1                 0   // 4
#define EE_posAxis2                 4   // 4
#define EE_pierSide                 8   // 1
#define EE_parkStatus               9   // 1
#define EE_parkSaved                10  // 1
                                    
#define EE_currentSite              11  // 1
                                    
#define EE_pauseHome                12  // 1 + 1
                                    
#define EE_LMT                      14  // 4
#define EE_JD                       18  // 4
                                    
#define EE_pulseGuideRate           22  // 1
#define EE_maxRate                  23  // 2
                                    
#define EE_autoMeridianFlip         25  // 1
                                    
#define EE_dfCor                    26  // 4
#define EE_ax1Cor                   30  // 4
#define EE_ax2Cor                   34  // 4
                                    
#define EE_dpmE                     38  // 1
#define EE_dpmW                     39  // 1
#define EE_minAlt                   40  // 1
#define EE_maxAlt                   41  // 1
                                    
#define EE_doCor                    42  // 4
#define EE_pdCor                    46  // 4
#define EE_altCor                   50  // 4
#define EE_azmCor                   54  // 4
#define EE_indexAxis1               58  // 4
#define EE_indexAxis2               62  // 4
#define EE_tfCor                    66  // 4
                            
#define EE_pecStatus                70  // 1
#define EE_pecRecorded              71  // 1
                                    
#define EE_posAxis4                 72  // 4
                                    
#define EE_wormSensePos             76  // 4
                                    
#define EE_backlashAxis1            80  // 4
#define EE_backlashAxis2            84  // 4
#define EE_siderealInterval         88  // 4
#define EE_posAxis5                 92  // 4
                                    
#define EE_autoInitKey              96

// site index: 100-199
// 100..103 latitude  1  ((index 1-1)*25+100)
// 104..107 longitude 1
// 108      timeZone  1
// 109..124 site name 1

// 125..128 latitude  2  ((index 2-1)*25+100)
// 129..132 longitude 2
// 133      timeZone  2
// 134..149 site name 2

// 150..153 latitude  3  ((index 3-1)*25+100)
// 154..157 longitude 3
// 158      timeZone  3
// 159..174 site name 3

// 175..178 latitude  4  ((index 4-1)*25+100)
// 179..182 longitude 4
// 183      timeZone  4
// 184..199 site name 4

#define EE_sites                   100

// PEC table: 200...pecBufferSize+199
// pecBufferSize table of byte sized integers -128..+127, units are steps

#define EE_pecTable                200

// Library
// Catalog storage starts at 200+pecBufferSize and ends at E2END-100

// General purpose storage B (100 bytes), E2END-99..E2END
#define GSB                       (E2END-100)
#define EE_dcPwrAxis4              GSB+0   // 1
#define EE_dcPwrAxis5              GSB+1   // 1
#define EE_maxRateL                GSB+2   // 4
#define EE_tcfCoefAxis4            GSB+6   // 4
#define EE_tcfCoefAxis5            GSB+10  // 4
#define EE_tcfEnAxis4              GSB+14  // 1
#define EE_tcfEnAxis5              GSB+15  // 1

#define EE_feature1Value1          GSB+16  // 1
#define EE_feature1Value2          GSB+17  // 1
#define EE_feature1Value3          GSB+18  // 1
#define EE_feature2Value1          GSB+19  // 1
#define EE_feature2Value2          GSB+20  // 1
#define EE_feature2Value3          GSB+21  // 1
#define EE_feature3Value1          GSB+22  // 1
#define EE_feature3Value2          GSB+23  // 1
#define EE_feature3Value3          GSB+24  // 1
#define EE_feature4Value1          GSB+25  // 1
#define EE_feature4Value2          GSB+26  // 1
#define EE_feature4Value3          GSB+27  // 1
#define EE_feature5Value1          GSB+28  // 1
#define EE_feature5Value2          GSB+29  // 1
#define EE_feature5Value3          GSB+30  // 1
#define EE_feature6Value1          GSB+31  // 1
#define EE_feature6Value2          GSB+32  // 1
#define EE_feature6Value3          GSB+33  // 1
#define EE_feature7Value1          GSB+34  // 1
#define EE_feature7Value2          GSB+35  // 1
#define EE_feature7Value3          GSB+36  // 1
#define EE_feature8Value1          GSB+37  // 1
#define EE_feature8Value2          GSB+38  // 1
#define EE_feature8Value3          GSB+39  // 1

// ---------------------------------------------------------------------------------------------------------------------------------
// Unique identifier for the current initialization format for NV, do not change
#define NV_INIT_KEY 915307548

#define PierSideNone               0
#define PierSideEast               1
#define PierSideWest               2
#define PierSideBest               3
#define PierSideFlipWE1            10
#define PierSideFlipWE2            11
#define PierSideFlipWE3            12
#define PierSideFlipEW1            20
#define PierSideFlipEW2            21
#define PierSideFlipEW3            22
