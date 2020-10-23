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

#if defined(__STM32F1__) || defined(STM32F103xB) || defined(STM32F303xC)

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

#if PINMAP == STM32Blue

// The multi-purpose pins (Aux3..Aux8 can be analog pwm/dac if supported)
#define Aux0                PC13    // Status LED
#define Aux1                PB14    // ESP8266 GPIO0, SPI MISO/Fault
#define Aux2                 PA1    // ESP8266 RST, SPI MISO/Fault
#define Aux3                 PB8    // Reticle, Home SW
#define Aux4                PA13    // OneWire, Home SW

#ifndef OneWirePin
  #define OneWirePin        Aux4    // Default Pin for OneWire bus
#endif

// For ESP8266 control
#define AddonBootModePin    Aux1    // ESP8266 GPIO0 or SPI MISO/Fault
#define AddonResetPin       Aux2    // ESP8266 RST or SPI MISO/Fault

// Axis1 RA/Azm step/dir driver
#define Axis1_EN            PA12    // Enable
#define Axis1_M0            PA11    // Microstep Mode 0
#define Axis1_M0PORT       GPIOA
#define Axis1_M0BIT  GPIO_PIN_11
#define Axis1_M1             PA8    // Microstep Mode 1
#define Axis1_M1PORT       GPIOA
#define Axis1_M1BIT   GPIO_PIN_8
#define Axis1_M2            PB15    // Microstep Mode 2
#define Axis1_M2PORT       GPIOB
#define Axis1_M2BIT  GPIO_PIN_15
#define Axis1_M3            Aux1    // ESP8266 GPIO0 or SPI MISO/Fault
#define Axis1_STEP          PB13    // Step
#define Axis1_StpPORT      GPIOB    //
#define Axis1_StpBIT GPIO_PIN_13    //
#define Axis1_DIR           PB12    // Motor Direction
#define Axis1_DirPORT      GPIOB    //
#define Axis1_DirBIT GPIO_PIN_12    //
#define Axis1_DECAY     Axis1_M2    // Decay mode
#define Axis1_FAULT         Aux1    // ESP8266 GPIO0 or SPI MISO/Fault
#define Axis1_HOME          Aux3    // Sense home position Axis1

// Axis2 Dec/Alt step/dir driver
#define Axis2_EN             PA5    // Enable
#define Axis2_M0             PA4    // Microstep Mode 0
#define Axis2_M0PORT       GPIOA
#define Axis2_M0BIT   GPIO_PIN_4
#define Axis2_M1             PA3    // Microstep Mode 1
#define Axis2_M1PORT       GPIOA
#define Axis2_M1BIT   GPIO_PIN_3
#define Axis2_M2             PA2    // Microstep Mode 2
#define Axis2_M2PORT       GPIOA
#define Axis2_M2BIT   GPIO_PIN_2
#define Axis2_M3            Aux2    // ESP8266 RST or SPI MISO/Fault
#define Axis2_STEP           PA0    // Step
#define Axis2_StpPORT      GPIOA    //
#define Axis2_StpBIT  GPIO_PIN_0    //
#define Axis2_DIR           PC15    // Motor Direction
#define Axis2_DirPORT      GPIOC    //
#define Axis2_DirBIT GPIO_PIN_15    //
#define Axis2_DECAY     Axis2_M2    // Decay mode
#define Axis2_FAULT         Aux2    // ESP8266 RST or SPI MISO/Fault
#define Axis2_HOME          Aux4    // Sense home position Axis2

// ST4 interface
#define ST4DEn               PA7    // ST4 DE+ North
#define ST4DEs               PA6    // ST4 DE- South
#define ST4RAw               PB1    // ST4 RA- West
#define ST4RAe               PB0    // ST4 RA+ East

// This is the built in LED for the Black Pill board. There is a pin
// available from it too, in case you want to power another LED with a wire
#define LEDnegPin           Aux0    // Drain
#define LEDneg2Pin           OFF    // Drain

// For a piezo buzzer
#define TonePin              PB9    // Tone

// The PPS pin is a 3.3V logic input, OnStep measures time between rising edges and
// adjusts the internal sidereal clock frequency
#define PpsPin               PB5    // Pulse Per Second time source, e.g. GPS, RTC

// For rotator stepper driver
#define Axis3_EN             PB4    // Enable
#define Axis3_STEP           PB3    // Step
#define Axis3_DIR           PA15    // Dir

// Pins to focuser1 stepper driver
#define Axis4_EN             PB4    // Enable
#define Axis4_STEP           PB3    // Step
#define Axis4_DIR           PA15    // Dir

// For focuser2 stepper driver
#define Axis5_EN             OFF    // Enable
#define Axis5_STEP           OFF    // Step
#define Axis5_DIR            OFF    // Dir

// The limit switch sense is a logic level input which uses the internal pull up,
// shorted to ground it stops gotos/tracking
#define LimitPin            PA14

// The PEC index sense is a logic level input, resets the PEC index on rising
// edge then waits for 60 seconds before allowing another reset
#define PecPin              PC14
#define AnalogPecPin         OFF    // PEC Sense, analog or digital

#define ReticlePin          Aux3    // Drain

#else
  #error "Unknown STM32 Board. This pinmap is only for Blue and Black Pill variants"
#endif

#else
  #error "Wrong processor for this configuration!"
#endif
