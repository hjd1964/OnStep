// -------------------------------------------------------------------------------------------------
// Pin map for OnStep on STM32
// 
// This pin map is for an STM32F103C8T6 "Black Pill" stick.
// It runs at 72MHz has 128K flash and 20K RAM, and is 3.3V only
//
// Cost on eBay and AliExpress is less than US $2.50
// More info, schematic at:
//   http://wiki.stm32duino.com/index.php?title=Black_Pill
//
// Other boards based on the following chips also have enough flash
// and RAM for OnStep, and should work, with pin modifications.
//
// STM32103VC: 72MHz, 256K flash, 48K RAM
// STM32103VE: 72MHz, 512K flash, 64K RAM
// STM32103ZE: 72MHz, 512K flash, 64K RAM
// STM32103RC: 72MHz, 256K flash, 48K RAM
// STM32103RE: 72MHz, 512K flash, 64K RAM

#if defined(__STM32F1__)

// The pins here are not tested yet, and need to change 

#define Axis1_EN        PA2    // Enable
#define Axis1_M0        PA1    // Microstep Mode 0
#define Axis1_M1        PA0    // Microstep Mode 1
#define Axis1_M2        PC13   // Microstep Mode 2
#define Axis1StepPin    PB9    // Step
#define Axis1DirPin     PB8    // Motor Direction
//#define Axis1_FAULT   Undefined    // Fault
//#define Axis1_Aux     Axis1_Aux    // Aux - ESP8266 GPIO0 or SPI MISO

#define Axis2_EN        PB13   // Enable
#define Axis2_M0        PB14   // Microstep Mode 0
#define Axis2_M1        PB15   // Microstep Mode 1
#define Axis2_M2        PA8    // Microstep Mode 2
#define Axis2StepPin    PA9    // Step
#define Axis2DirPin     PA10   // Motor Direction
//#define Axis2_FAULT   Undefined    // Fault
//#define Axis2_Aux     Axis2_FAULT  // Aux - ESP8266 RST or SPI MISO

// ST4 interface
#define ST4DEn          PA14   // ST4 DE+ North
#define ST4DEs          PA13   // ST4 DE- South
#define ST4RAw          PB1    // ST4 RA- West
#define ST4RAe          PB0    // ST4 RA+ East

// This is the built in LED for the Black Pill board. There is a pin
// available from it too, in case you want to power another LED with a wire
#define LEDnegPin       PB12         // Drain

// For a piezo buzzer
#define TonePin         PB4    // Tone

// The PPS pin is a 3.3V logic input, OnStep measures time between rising edges and
// adjusts the internal sidereal clock frequency
#define PpsPin          PB5    // Pulse Per Second time source, e.g. GPS, RTC

// Pins to focuser1 stepper driver
#define Axis4DirPin     PA15   // Dir
#define Axis4StepPin    PB3    // Step

// For rotator stepper driver
//#define Axis3DirPin     PB4    // Dir
//#define Axis3StepPin    PB5    // Step

// For focuser2 stepper driver
//#define Axis5DirPin   Undefined    // Dir
//#define Axis5StepPin  Undefined    // Step

// The limit switch sense is a logic level input which uses the internal pull up,
// shorted to ground it stops gotos/tracking
//#define LimitPin      Undefined   

// The PEC index sense is a logic level input, resets the PEC index on rising
// edge then waits for 60 seconds before allowing another reset
//#define PecPin       Undefined
//#define AnalogPecPin Undefined    // PEC Sense, analog or digital

//#define LEDneg2Pin    Undefined    // Drain

//#define ReticulePin   Undefined    // Drain

#else
#error "Wrong processor for this configuration!"

#endif

