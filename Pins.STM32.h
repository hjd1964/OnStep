// -------------------------------------------------------------------------------------------------
// Pin map for OnStep STM32F103 V or Z

#if defined(ARM_STM32)

// The pins here are not tested yet, and need to change 
// The PEC index sense is a logic level input, resets the PEC index on rising edge then waits for 60 seconds before allowing another reset
#define PecPin        PA10
#define AnalogPecPin  PA11    // Pin 23 (PEC Sense, analog or digital)

// The status LED is a two wire jumper with a 10k resistor in series to limit the current to the LED
#define LEDnegPin     PA12    // Pin 19 (Drain)
#define LEDneg2Pin    PA13    // Pin 22 (Drain)
#define ReticulePin   PA13    // Pin 22 (Drain)

// For a piezo buzzer
#define TonePin       PA14    // Pin 29 (Tone)

// The PPS pin is a 3.3V logic input, OnStep measures time between rising edges and adjusts the internal sidereal clock frequency
#define PpsPin        PA15    // Pin 28 (PPS time source, GPS for example)

#define Axis1DirPin   PA0    // Pin 21 (Dir)
#define Axis1StepPin  PA1    // Pin 20 (Step)
#define RstPin        PA2    // Pin 19 (Reset)
#define Axis1_FAULT   PA3    // Pin 18 (Fault)
#define Axis1_Aux     PA3    // Pin 18 (Aux - ESP8266 GPIO0 or SPI MISO)
#define Axis1_M2      PA4    // Pin 17 (Microstep Mode 2 or SPI CS)
#define Axis1_M1      PA5    // Pin 16 (Microstep Mode 1 or SPI SCK)
#define Axis1_M0      PA6    // Pin 15 (Microstep Mode 0 or SPI MOSI)
#define Axis1_EN      PA7    // Pin 14 (Enable)

#define Axis2DirPin    PB0    // Pin  2 (Dir)
#define Axis2StepPin   PB1    // Pin  3 (Step)
#define LimitPin       PB2    // Pin  4 (The limit switch sense is a logic level input which uses the internal pull up, shorted to ground it stops gotos/tracking)
#define Axis2_FAULT    PB3    // Pin  5 (Fault)
#define Axis2_Aux      PB3    // Pin  5 (Aux - ESP8266 RST or SPI MISO)
#define Axis2_M2       PB4    // Pin  6 (Microstep Mode 2 or SPI CS)
#define Axis2_M1       PB5    // Pin  7 (Microstep Mode 1 or SPI SCK)
#define Axis2_M0       PB6    // Pin  8 (Microstep Mode 0 or SPI MOSI)
#define Axis2_EN       PB7    // Pin  9 (Enable)

// Pins to focuser2/rotator stepper driver
#define Axis3DirPin   PC1    // Pin 33 (Dir)
#define Axis3StepPin  PC2    // Pin 30 (Step)

// Pins to focuser1 stepper driver
#define Axis4DirPin   PC3    // Pin 32 (Dir)
#define Axis4StepPin  PC4    // Pin 31 (Step)

// ST4 interface
#define ST4RAw        PD10    // Pin 10 ST4 RA- West
#define ST4DEs        PD11    // Pin 11 ST4 DE- South
#define ST4DEn        PD12    // Pin 12 ST4 DE+ North
#define ST4RAe        PD13    // Pin 13 ST4 RA+ East

#else
#error "Wrong processor for this configuration!"

#endif

