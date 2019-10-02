// -------------------------------------------------------------------------------------------------
// Pin map for legacy OnStep "Alternate pin-map" (Mega2560)

#if defined(__AVR_ATmega2560__)

// Misc. pins
#ifndef DS3234_CS_PIN
  #define DS3234_CS_PIN      10     // Default CS Pin for DS3234 on SPI
#endif
#ifndef BME280_CS_PIN
  #define BME280_CS_PIN      11     // Default CS Pin for BME280 on SPI
#endif
#ifndef OneWirePin
  #define OneWirePin         24     // Default Pin for one wire bus
#endif

// The PEC index sense is a 5V logic input, resets the PEC index on rising edge then waits for 60 seconds before allowing another reset
#define PecPin                2
#define AnalogPecPin          1

// The limit switch sense is a 5V logic input which uses the internal pull up, shorted to ground it stops gotos/tracking
#define LimitPin              3

// The status LED is a two wire jumper with a 10k resistor in series to limit the current to the LED
#define LEDposPin             8     // LED
#define LEDnegPin             9     // GND
#define LEDneg2Pin            7     // GND
#define ReticlePin           44     // GND

// For a piezo buzzer
#define TonePin              42     // Tone

// The PPS pin is a 5V logic input, OnStep measures time between rising edges and adjusts the internal sidereal clock frequency
#define PpsPin               21     // Interrupt 2 on Pin 21 (alternate Int3 on Pin20)

// Axis1 RA/Azm step/dir driver
#define Axis1_EN             23     // Enable
#define Axis1_M0             29     // Microstep or Decay Mode 0
#define Axis1_M1             27     // Microstep or Decay Mode 1
#define Axis1_M2             25     // Microstep or Decay Mode 2
#define Axis1_M3             39     // SPI MISO/Fault
#define Axis1StepPin         35     // Step PC2
#define Axis1StepBit          2     //
#define Axis1StepPORT     PORTC     //
#define Axis1DirPin          37     // Dir  PC0
#define Axis1DirBit           0     //
#define Axis1DirPORT      PORTC     //
#define Axis1FaultPin        39     // SPI MISO/Fault
#define Axis1ModePin         41     // Decay mode

// Axis2 Dec/Alt step/dir driver
#define Axis2_EN             36     // Enabled
#define Axis2_M0             30     // Microstep or Decay Mode 0
#define Axis2_M1             32     // Microstep or Decay Mode 1
#define Axis2_M2             34     // Microstep or Decay Mode 2
#define Axis2_M3             38     // SPI MISO/Fault
#define Axis2StepPin         24     // Step  PA2
#define Axis2StepBit          2     //
#define Axis2StepPORT     PORTA     //
#define Axis2DirPin          22     // Dir   PA0
#define Axis2DirBit           0     //
#define Axis2DirPORT      PORTA     //
#define Axis2FaultPin        38     // SPI MISO/Fault
#define Axis2ModePin         40     // Decay mode

// For rotator stepper driver
#define Axis3_EN             -1     // Enable
#define Axis3StepPin         -1     // Step
#define Axis3DirPin          -1     // Dir

// For focuser1 stepper driver
#define Axis4_EN             -1     // Enable
#define Axis4StepPin         -1     // Step
#define Axis4DirPin          -1     // Dir

// For focuser2 stepper driver
#define Axis5_EN             -1     // Enable
#define Axis5StepPin         -1     // Step
#define Axis5DirPin          -1     // Dir

// ST4 interface
#define ST4RAw               47     // ST4 RA- West
#define ST4DEs               43     // ST4 DE- South
#define ST4DEn               45     // ST4 DE+ North
#define ST4RAe               49     // ST4 RA+ East

#else
#error "Wrong processor for this configuration!"

#endif
