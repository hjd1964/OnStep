// -------------------------------------------------------------------------------------------------
// Pin map for OnStep on STM32
// 
// This pin map is for an STM32F103C8T6 "Blue Pill" and "Black Pill" boards..
// They run at 72MHz, with 20K of RAM, and 128K of flash.
// The Black variant is 3.3V only, while the Blue has some pins that are 5V tolerant.
// The Black Pill has two less pins than the Blue Pill.
//
// More info, schematic at:
//   http://wiki.stm32duino.com/index.php?title=Black_Pill
//   http://wiki.stm32duino.com/index.php?title=Blue_Pill
//
// Cost on eBay and AliExpress is less than US $2.50

#if defined(__STM32F1__)

// === Pins for USB 
// In HAL, Serial_A is mapped to USART1, with the following pins:
// STM32 pin PA9  TX -> RX on CP2102 UART to TTL board
// STM32 pin PA10 RX -> TX on CP2102 UART to TTL board

// === Pins for WiFi
// In HAL, Serial_B is mapped to USART3, with the following pins:
//
// STM32 pin PB10 TX -> RX on ESP8266
// STM32 pin PB11 RX -> TX on ESP8266

// === Pins for DS3231 RTC/EEPROM
// The STM32 has no built in EEPROM. Therefore, we use a DS3231 RTC module 
// which has an EEPROM chip on the board. HAL takes care of its address and size:
//
// STM32 pin PB6 -> SCL on DS3231
// STM32 pin PB7 -> SDA on DS3231

#if defined(STM32Blue_ON)

  #define A1EN          PA12
  #define A1M0          PA11  
  #define A1M1          PA8
  #define A1M2          PB15
  #define A1MISO        PB14
  #define A1ST          PB13
  #define A1DR          PB12

  #define A2EN          PA5  
  #define A2M0          PA4 
  #define A2M1          PA3
  #define A2M2          PA2 
  #define A2MISO        PA1
  #define A2ST          PA0
  #define A2DR          PC15

  #define S4N           PA7
  #define S4S           PA6
  #define S4W           PB1
  #define S4E           PB0

  #define LED           PC13

  #define TONE          PB9 
  #define RETICLE       PB8
  #define PECIDX        PC14

  #define SQW           PB5 

  #define F1EN          PB4
  #define F1ST          PB3
  #define F1DR          PA15

  #define LIMIT         PA14
  #define SPARE_PA13    PA13

#elif defined(STM32Black_ON) 

  #define A1EN          PA3
  #define A1M0          PA2
  #define A1M1          PA1
  #define A1M2          PA0  
  #define A1MISO        PC13
  #define A1ST          PB9
  #define A1DR          PB8

  #define A2EN          PA15
  #define A2M0          PA12
  #define A2M1          PA11 
  #define A2M2          PA8  
  #define A2MISO        PB15 
  #define A2ST          PB14
  #define A2DR          PB13

  #define S4N           PA7 
  #define S4S           PA6 
  #define S4W           PA5 
  #define S4E           PA4 

  #define LED           PB12
  #define TONE          PB1 
  #define SQW           PB5 

  #define PECIDX        PB0

  #define SPARE1        PB3 
  #define SPARE2        PB4
  #define SPARE3        PA14
  #define SPARE4        PA13

#else
  #error "Unknown STM32 Board. This pinmap is only for Blue and Black Pill variants"
#endif

// The pins here are not tested yet, and need to change 

// The multi-purpose pins (Aux3..Aux8 can be analog pwm/dac if supported)
#define Aux0            LED
#define Aux1            A1MISO
#define Aux2            A2MISO

#define Axis1_EN        A1EN   // Enable
#define Axis1_M0        A1M0   // Microstep Mode 0
#define Axis1_M1        A1M1   // Microstep Mode 1
#define Axis1_M2        A1M2   // Microstep Mode 2
#define Axis1StepPin    A1ST   // Step
#define Axis1DirPin     A1DR   // Motor Direction
#define Axis1_Aux       Aux1   // Aux - ESP8266 GPIO0 or SPI MISO
#define Axis1_FAULT     Aux1   // Fault

#define Axis2_EN        A2EN   // Enable
#define Axis2_M0        A2M0   // Microstep Mode 0
#define Axis2_M1        A2M1   // Microstep Mode 1
#define Axis2_M2        A2M2   // Microstep Mode 2
#define Axis2StepPin    A2ST   // Step
#define Axis2DirPin     A2DR   // Motor Direction
#define Axis2_Aux       Aux2   // Aux - ESP8266 RST or SPI MISO
#define Axis2_FAULT     Aux2   // Fault

// ST4 interface
#define ST4DEn          S4N    // ST4 DE+ North
#define ST4DEs          S4S    // ST4 DE- South
#define ST4RAw          S4W    // ST4 RA- West
#define ST4RAe          S4E    // ST4 RA+ East

// This is the built in LED for the Black Pill board. There is a pin
// available from it too, in case you want to power another LED with a wire
#define LEDnegPin       Aux0   // Drain

// For a piezo buzzer
#define TonePin         TONE   // Tone

// The PPS pin is a 3.3V logic input, OnStep measures time between rising edges and
// adjusts the internal sidereal clock frequency
#define PpsPin          SQW    // Pulse Per Second time source, e.g. GPS, RTC

// For rotator stepper driver
#define Axis3DirPin     F1DR   // Dir
#define Axis3StepPin    F1ST   // Step
#define Axis3_EN        F1EN   // Enable

// Pins to focuser1 stepper driver
#define Axis4DirPin     F1DR   // Dir
#define Axis4StepPin    F1ST   // Step
#define Axis4_EN        F1EN   // Enable

// For focuser2 stepper driver
#define Axis5DirPin     F1DR   // Dir
#define Axis5StepPin    F1ST   // Step
#define Axis5_EN        F1EN   // Enable

// The limit switch sense is a logic level input which uses the internal pull up,
// shorted to ground it stops gotos/tracking
#define LimitPin        LIMIT

// The PEC index sense is a logic level input, resets the PEC index on rising
// edge then waits for 60 seconds before allowing another reset
#define PecPin          PECIDX
//#define AnalogPecPin  Undefined    // PEC Sense, analog or digital

//#define LEDneg2Pin    Undefined    // Drain

#define ReticulePin     RETICLE    // Drain

#else
#error "Wrong processor for this configuration!"

#endif
