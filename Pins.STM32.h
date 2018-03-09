// -------------------------------------------------------------------------------------------------
// Pin map for OnStep on STM32F103
// Boards based on the following chips have enough flash and RAM for OnStep, and should work with 
// the pins below:
//
// STM32103VC: 72MHz, 256K flash, 48K RAM
// STM32103VE: 72MHz, 512K flash, 64K RAM
// STM32103ZE: 72MHz, 512K flash, 64K RAM

// The following boards should also work, but you have to modify the pins below:
//
// STM32103RC: 72MHz, 256K flash, 48K RAM
// STM32103RE: 72MHz, 512K flash, 64K RAM

#if defined(__ARM_STM32__)

// The pins here are not tested yet, and need to change 

#define Axis1_EN      PD10   // Enable
#define Axis1DirPin   PD12   // Motor Direction
#define Axis1StepPin  PD14   // Step
#define Axis1_M2      PD11   // Microstep Mode 2 or SPI CS
#define Axis1_M1      PD13   // Microstep Mode 1 or SPI SCK
#define Axis1_M0      PD15   // Microstep Mode 0 or SPI MOSI
#define Axis1_FAULT   PD9    // Fault
#define Axis1_Aux     Axis1_Aux    // Aux - ESP8266 GPIO0 or SPI MISO

#define Axis2_EN      PE10   // Enable
#define Axis2DirPin   PE12   // Motor Direction
#define Axis2StepPin  PE14   // Step
#define Axis2_M2      PE11   // Microstep Mode 2 or SPI CS
#define Axis2_M1      PE13   // Microstep Mode 1 or SPI SCK
#define Axis2_M0      PE15   // Microstep Mode 0 or SPI MOSI
#define Axis2_FAULT   PE9    // Fault
#define Axis2_Aux     Axis2_FAULT    // Aux - ESP8266 RST or SPI MISO

// The PEC index sense is a logic level input, resets the PEC index on rising edge then waits for 60 seconds before allowing another reset
#define PecPin        PA10
#define AnalogPecPin  PA11    // PEC Sense, analog or digital

// The status LED is a two wire jumper with a 10k resistor in series to limit the current to the LED
#define LEDnegPin     PA12    // Drain
#define LEDneg2Pin    PA13    // Drain
#define ReticulePin   PA13    // Drain

// For a piezo buzzer
#define TonePin       PA14    // Tone

// The PPS pin is a 3.3V logic input, OnStep measures time between rising edges and adjusts the internal sidereal clock frequency
#define PpsPin        PA15    // Pin 28 (PPS time source, GPS for example)

#define LimitPin       PB2    // The limit switch sense is a logic level input which uses the internal pull up, shorted to ground it stops gotos/tracking

// For rotator stepper driver
#define Axis3DirPin   PC1    // Dir
#define Axis3StepPin  PC2    // Step

// Pins to focuser1 stepper driver
#define Axis4DirPin   PC3    // Dir
#define Axis4StepPin  PC4    // Step

// For focuser2 stepper driver
#define Axis5DirPin   PC1    // Dir
#define Axis5StepPin  PC2    // Step

// ST4 interface
#define ST4RAw        PD10    // ST4 RA- West
#define ST4DEs        PD11    // ST4 DE- South
#define ST4DEn        PD12    // ST4 DE+ North
#define ST4RAe        PD13    // ST4 RA+ East

#else
#error "Wrong processor for this configuration!"

#endif

