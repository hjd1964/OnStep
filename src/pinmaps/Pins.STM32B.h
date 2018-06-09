// -------------------------------------------------------------------------------------------------
// Pin map for OnStep on STM32
// 
// This pin map is for an STM32F103C8T6 "Black Pill" and "Blue Pill" sticks.
// They run at 72MHz, have 128K flash and 20K RAM
// The Black variant is 3.3V only, while the Blue has some pins that are 5V tolerant.
// The Black Pill has two less pins than the Blue Pill. The Blue Pill has two flaws
// with workarounds, wrong resistor interfers with flashing via USB, and a flimsy
// USB connector. 
//
// More info, schematic at:
//   http://wiki.stm32duino.com/index.php?title=Black_Pill
//   http://wiki.stm32duino.com/index.php?title=Blue_Pill
//
// Cost on eBay and AliExpress is less than US $2.50
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

// === Pins for WiFi
// In HAL, Serial_B is mapped to USART3, with the following
// pins. If you change HAL, this will change:
//
// STM32 pin PB10 TX -> RX on ESP8266
// STM32 pin PB11 RX -> TX on ESP8266

// === Pins for DS3231 RTC/EEPROM
// Without an EEPROM, the STM32 cannot work. The best EEPROM is
// a DS3231 module that has an EEPROM chip in it. HAL takes care
// of its address and size:
//
// STM32 pin PB6 -> SCL on DS3231
// STM32 pin PB7 -> SDA on DS3231

#if defined(STM32Black_ON)

  #define A1EN          PA3
  #define A1M0          PA2
  #define A1M1          PA1
  #define A1M2          PA0  
  #define A1ST          PC13
  #define A1DR          PB9
  #define A1MO          PB8

  #define A2EN          PA15
  #define A2M0          PA10
  #define A2M1          PA9 
  #define A2M2          PA8 
  #define A2ST          PB15
  #define A2DR          PB14
  #define A2MO          PB13

  #define S4N           PA7 
  #define S4S           PA6 
  #define S4W           PA5 
  #define S4E           PA4 

  #define LED           PB12
  #define TONE          PB4 
  #define SQW           PB5 

  #define F1DR          PB0 
  #define F1ST          PB1 
  #define F1EN          PB3

  #define SPARE1        PA13
  #define SPARE2        PA14

#elif defined(STM32Blue_ON) 

  #define A1EN          PA10
  #define A1M0          PA9 
  #define A1M1          PA8 
  #define A1M2          PB15
  #define A1ST          PB14
  #define A1DR          PB13
  #define A1MO          PB12

  #define A2EN          PA4  
  #define A2M0          PA3 
  #define A2M1          PA2
  #define A2M2          PA1 
  #define A2ST          PA0 
  #define A2DR          PC15
  #define A2MO          PC14

  #define S4N           PA15
  #define S4S           PB3 
  #define S4W           PB4
  #define S4E           PB5

  #define LED           PC13
  #define TONE          PB9 
  #define SQW           PB8 

  #define F1DR          PA5 
  #define F1ST          PA6 
  #define F1EN          PA7 

  #define SPARE1        PA13
  #define SPARE2        PA14
  #define SPARE3        PB0 
  #define SPARE4        PB1

#else
  #error "Unknown STM32 Board. This pinmap is only for Blue and Black Pill variants"
#endif

// The pins here are not tested yet, and need to change 

#define Axis1_EN        A1EN   // Enable
#define Axis1_M0        A1M0   // Microstep Mode 0
#define Axis1_M1        A1M1   // Microstep Mode 1
#define Axis1_M2        A1M2   // Microstep Mode 2
#define Axis1StepPin    A1ST   // Step
#define Axis1DirPin     A1DR   // Motor Direction
#define Axis1_Aux       A1MO   // Aux - ESP8266 GPIO0 or SPI MISO
//#define Axis1_FAULT   Undefined    // Fault

#define Axis2_EN        A2EN   // Enable
#define Axis2_M0        A2M0   // Microstep Mode 0
#define Axis2_M1        A2M1   // Microstep Mode 1
#define Axis2_M2        A2M2   // Microstep Mode 2
#define Axis2StepPin    A2ST   // Step
#define Axis2DirPin     A2DR   // Motor Direction
#define Axis2_Aux       A2MO   // Aux - ESP8266 RST or SPI MISO
//#define Axis2_FAULT   Undefined    // Fault

// ST4 interface
#define ST4DEn          S4N    // ST4 DE+ North
#define ST4DEs          S4S    // ST4 DE- South
#define ST4RAw          S4W    // ST4 RA- West
#define ST4RAe          S4E    // ST4 RA+ East

// This is the built in LED for the Black Pill board. There is a pin
// available from it too, in case you want to power another LED with a wire
#define LEDnegPin       LED    // Drain

// For a piezo buzzer
#define TonePin         TONE   // Tone

// The PPS pin is a 3.3V logic input, OnStep measures time between rising edges and
// adjusts the internal sidereal clock frequency
#define PpsPin          SQW    // Pulse Per Second time source, e.g. GPS, RTC

// Pins to focuser1 stepper driver
#define Axis4DirPin     F1DR   // Dir
#define Axis4StepPin    F1ST   // Step
#define Axis4_EN        F1EN   // Step

// For focuser2 stepper driver
//#define Axis5DirPin     F2DR   // Dir
//#define Axis5StepPin    F2ST   // Step

// For rotator stepper driver
//#define Axis5DirPin   PB4    // Dir
//#define Axis5StepPin  PB5    // Step

// The limit switch sense is a logic level input which uses the internal pull up,
// shorted to ground it stops gotos/tracking
//#define LimitPin      Undefined   

// The PEC index sense is a logic level input, resets the PEC index on rising
// edge then waits for 60 seconds before allowing another reset
//#define PecPin        Undefined
//#define AnalogPecPin  Undefined    // PEC Sense, analog or digital

//#define LEDneg2Pin    Undefined    // Drain

//#define ReticulePin   Undefined    // Drain

#else
#error "Wrong processor for this configuration!"

#endif

