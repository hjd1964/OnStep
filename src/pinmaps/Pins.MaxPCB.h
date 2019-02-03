// -------------------------------------------------------------------------------------------------
// Pin map for OnStep MaxPCB (Teensy3.5/3.6)

#if defined(__MK64FX512__) || defined(__MK66FX1M0__)

// The multi-purpose pins (Aux3..Aux8 can be analog pwm/dac if supported)
#define Aux0          19
#define Aux1          18
#define Aux2          5
#define Aux3          36    // this should also be ok as pwm analog output (w/#define Aux3_Analog)
#define Aux4          39
#define Aux5         A21    // true analog output
#define Aux5_Analog
#define Aux6         A22    // true analog output
#define Aux6_Analog
#define Aux7           4    // should be ok as pwm analog output (w/#define Aux7_Analog)
#define Aux8          22    // should be ok as pwm analog output (w/#define Aux8_Analog)

// The PEC index sense is a logic level input, resets the PEC index on rising edge then waits for 60 seconds before allowing another reset
#define PecPin        23
#define AnalogPecPin  23    // Pin 23 (PEC Sense, analog or digital)

// The status LED is a two wire jumper with a 10k resistor in series to limit the current to the LED
#define LEDnegPin     Aux0  // Pin 19 (Drain)
#define LEDneg2Pin    Aux8  // Pin 22 (Drain)
#define ReticulePin   Aux8  // Pin 22 (Drain)

// For a piezo buzzer
#define TonePin       29    // Pin 29 (Tone)

// The PPS pin is a 3.3V logic input, OnStep measures time between rising edges and adjusts the internal sidereal clock frequency
#define PpsPin        28    // Pin 28 (PPS time source, GPS for example)
#define DS3234_CS_PIN 10

#define Axis1DirPin   21    // Pin 21 (Dir)
#define Axis1StepPin  20    // Pin 20 (Step)
#define Axis1_HOME    Aux3  // Pin 36 (Home sw)
#define Axis1_FAULT   Aux1  // Pin 18 (Fault)
#define Axis1_Aux     Aux1  // Pin 18 (ESP8266 GPIO0 or SPI MISO)
#define Axis1_M2      17    // Pin 17 (Microstep Mode 2 or SPI CS)
#define Axis1_M1      16    // Pin 16 (Microstep Mode 1 or SPI SCK)
#define Axis1_M0      15    // Pin 15 (Microstep Mode 0 or SPI MOSI)
#define Axis1_EN      14    // Pin 14 (Enable)

#define Axis2DirPin    2    // Pin  2 (Dir)
#define Axis2StepPin   3    // Pin  3 (Step)
#define LimitPin      Aux7  // Pin  4 (The limit switch sense is a logic level input normally pull high (2k resistor,) shorted to ground it stops gotos/tracking)
#define Axis2_HOME    Aux4  // Pin 39 (Home sw)
#define Axis2_FAULT   Aux2  // Pin  5 (Fault)
#define Axis2_Aux     Aux2  // Pin  5 (ESP8266 RST or SPI MISO)
#define Axis2_M2       6    // Pin  6 (Microstep Mode 2 or SPI CS)
#define Axis2_M1       7    // Pin  7 (Microstep Mode 1 or SPI SCK)
#define Axis2_M0       8    // Pin  8 (Microstep Mode 0 or SPI MOSI)
#define Axis2_EN       9    // Pin  9 (Enable)

// For rotator stepper driver
#define Axis3DirPin   33    // Pin 33 (Dir)
#define Axis3StepPin  30    // Pin 30 (Step)
#ifndef Axis3_EN
#define Axis3_EN      Aux5  // Pin A21 (Enable)
#endif

// For focuser1 stepper driver
#define Axis4DirPin   35    // Pin 35 (Dir)
#define Axis4StepPin  34    // Pin 34 (Step)
#ifndef Axis4_EN
#define Axis4_EN      Aux6  // Pin A22 (Enable)
#endif

// For focuser2 stepper driver
#define Axis5DirPin   33    // Pin 33 (Dir)
#define Axis5StepPin  30    // Pin 30 (Step)
#ifndef Axis5_EN
#define Axis5_EN      Aux5  // Pin A21 (Enable)
#endif

// ST4 interface
#define ST4RAw        24    // Pin 24 ST4 RA- West
#define ST4DEs        25    // Pin 25 ST4 DE- South
#define ST4DEn        26    // Pin 26 ST4 DE+ North
#define ST4RAe        27    // Pin 27 ST4 RA+ East

#else
#error "Wrong processor for this configuration!"

#endif
