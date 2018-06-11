// -------------------------------------------------------------------------------------------------
// Pin map for legacy OnStep Classic (Mega2560 or Teensy3.x)

#if defined(__AVR_ATmega2560__)

// The PEC index sense is a 5V logic input, resets the PEC index on rising edge then waits for 60 seconds before allowing another reset
#define PecPin         2
#define AnalogPecPin   1

// The limit switch sense is a 5V logic input which uses the internal (or external 2k) pull up, shorted to ground it stops gotos/tracking
#define LimitPin       3

// The status LED is a two wire jumper with a 10k resistor in series to limit the current to the LED
//                                  Atmel   2560
#define LEDposPin      8    // Pin 8 (LED)   PH5
#define LEDnegPin      9    // Pin 9 (GND)   PH6
#define LEDneg2Pin    10    // Pin 10 (GND)  PB4
#define ReticulePin   44    // Pin 44 (GND)  

// For a piezo buzzer
#define TonePin       42    // Pin 42 (Tone)

// The PPS pin is a 5V logic input, OnStep measures time between rising edges and adjusts the internal sidereal clock frequency
#define PpsPin        21    // Interrupt 2 on Pin 21 (alternate Int3 on Pin20)
#define DS3234_CS_PIN 10

#define Axis1DirPin   11    // Pin 11 (Dir)  PB5
#define Axis1DirBit    5    //
#define Axis1DirPORT  PORTB //
#define Axis15vPin    12    // Pin 12 (5V?)  PB6
#define Axis1StepPin  13    // Pin 13 (Step) PB7
#define Axis1StepBit   7    //
#define Axis1StepPORT PORTB //
                            // Pin GND (GND)

// Pins to enable/disable the stepper drivers and set microstep mode, optional and normally just hard-wired (DRV8825)/ignored (BED-A4988)
#define Axis1_M0      22    // Pin 22 (Microstep Mode 0)
#define Axis1_M1      23    // Pin 23 (Microstep Mode 1)
#define Axis1_M2      24    // Pin 24 (Microstep Mode 2)
#define Axis1_EN      25    // Pin 25 (Enable)
#define Axis1_FAULT   26    // Pin 26 (Fault)
#define Axis1_Aux     26    // Pin 26 (Aux - SPI MISO)
#define Axis1_Mode    32    // Pin 32 (Mode switch for Axis1)

// The HA(RA) and Dec jumpers (going to the big easy drivers) are simply four wire jumper cables, each has identical wiring - simple modular construction
#define Axis2DirPin    4    // Pin 4 (Dir)   PG5
#define Axis2DirBit    5    //
#define Axis2DirPORT  PORTG //
#define Axis25vPin     5    // Pin 5 (5V?)   PE3
#define Axis2StepPin   6    // Pin 6 (Step)  PH3
#define Axis2StepBit   3    //
#define Axis2StepPORT PORTH //
#define Axis2GndPin    7    // Pin 7 (GND)   PH4

// Pins to enable/disable the stepper drivers and set microstep mode, optional and normally just hard-wired (DRV8825)/ignored (BED-A4988)
#define Axis2_M0      27    // Pin 27 (Microstep Mode 0)
#define Axis2_M1      28    // Pin 28 (Microstep Mode 1)
#define Axis2_M2      29    // Pin 29 (Microstep Mode 2)
#define Axis2_EN      30    // Pin 30 (Enable)
#define Axis2_FAULT   31    // Pin 31 (Fault)
#define Axis2_Aux     31    // Pin 31 (Aux - SPI MISO)
#define Axis2_Mode    33    // Pin 33 (Mode switch for Axis2)

// Pins to rotator stepper driver
#define Axis3DirPin   A8    // Pin A8 (Dir)
#define Axis3StepPin  A9    // Pin A9 (Step)

// Pins to focuser1 stepper driver
#define Axis4DirPin   A10   // Pin A10 (Dir)
#define Axis4StepPin  A11   // Pin A11 (Step)

// Pins to focuser2 stepper driver
#define Axis5DirPin   A12   // Pin A12 (Dir)
#define Axis5StepPin  A13   // Pin A13 (Step)

// ST4 interface
#ifdef ST4_ALTERNATE_PINS_ON
#define ST4RAw        47    // Pin 47 ST4 RA- West
#define ST4DEs        43    // Pin 43 ST4 DE- South
#define ST4DEn        45    // Pin 45 ST4 DE+ North
#define ST4RAe        49    // Pin 49 ST4 RA+ East
#else
#define ST4RAw        47    // Pin 47 ST4 RA- West
#define ST4DEs        49    // Pin 49 ST4 DE- South
#define ST4DEn        51    // Pin 51 ST4 DE+ North
#define ST4RAe        53    // Pin 53 ST4 RA+ East
#endif

#elif defined(__MK20DX256__) || defined(__MK64FX512__) || defined(__MK66FX1M0__)

// The PEC index sense is a logic level input, resets the PEC index on rising edge then waits for 60 seconds before allowing another reset
#define PecPin         2
#define AnalogPecPin  14

// The limit switch sense is a 3.3V logic input which uses the internal pull up, shorted to ground it stops gotos/tracking
#define LimitPin       3

// The status LED is a two wire jumper with a 10k resistor in series to limit the current to the LED
#define LEDposPin      8    // Pin 8 (LED)
#define LEDnegPin      9    // Pin 9 (GND)
#define LEDneg2Pin     7    // Pin 7 (GND)
#define ReticulePin    9    // Pin 9 (GND)

// For a piezo buzzer
#define TonePin       29    // Pin 29 (Tone)

// The PPS pin is a logic level input, OnStep measures time between rising edges and adjusts the internal sidereal clock frequency
#define PpsPin        23    // Pin 23 (PPS time source, GPS for example)

#define Axis1DirPin   10    // Pin 10 (Dir)
#define Axis15vPin    11    // Pin 11 (5V?)
#define Axis1StepPin  12    // Pin 12 (Step)
                            // Pin GND (GND)

// Pins to enable/disable the stepper drivers and set microstep mode, optional and normally just hard-wired (DRV8825)/ignored (BED-A4988)
#define Axis1_M0      13    // Pin 13 (Microstep Mode 0)
#define Axis1_M1      14    // Pin 14 (Microstep Mode 1)
#define Axis1_M2      15    // Pin 15 (Microstep Mode 2)
#define Axis1_EN      16    // Pin 16 (Enable)
#define Axis1_FAULT   17    // Pin 17 (Fault)
#define Axis1_Aux     17    // Pin 17 (Aux - ESP8266 GPIO0 or SPI MISO)

// The HA(RA) and Dec jumpers (going to the big easy drivers) are simply four wire jumper cables, each has identical wiring - simple modular construction
#define Axis2DirPin    4    // Pin 4 (Dir)
#define Axis25vPin     5    // Pin 5 (5V?)
#define Axis2StepPin   6    // Pin 6 (Step)
#define Axis2GndPin    7    // Pin 7 (GND)

#define Axis2_M0      18    // Pin 18 (Microstep Mode 0)
#define Axis2_M1      19    // Pin 19 (Microstep Mode 1)
#define Axis2_M2      20    // Pin 20 (Microstep Mode 2)
#define Axis2_EN      21    // Pin 21 (Enable)
#define Axis2_FAULT   22    // Pin 22 (Fault)
#define Axis2_Aux     22    // Pin 22 (Aux - ESP8266 Rst or SPI MISO)

// For rotator stepper driver
#define Axis3DirPin   33    // Pin 33 (Dir)
#define Axis3StepPin  30    // Pin 30 (Step)

// For focuser1 stepper driver  
#if defined(__MK64FX512__) || defined(__MK66FX1M0__)
  // teensy3.5/3.6
  #define Axis4DirPin   35    // Pin 35 (Dir)
  #define Axis4StepPin  34    // Pin 34 (Step)
#else
  // teensy3.2
  #define Axis4DirPin   32    // Pin 32 (Dir)
  #define Axis4StepPin  31    // Pin 31 (Step)
#endif

// For focuser2 stepper driver
#define Axis5DirPin   33    // Pin 33 (Dir)
#define Axis5StepPin  30    // Pin 30 (Step)

// ST4 interface
#define ST4RAw        24    // Pin 24 ST4 RA- West
#define ST4DEs        25    // Pin 25 ST4 DE- South
#define ST4DEn        26    // Pin 26 ST4 DE+ North
#define ST4RAe        27    // Pin 27 ST4 RA+ East

#else
#error "Wrong processor for this configuration!"

#endif

