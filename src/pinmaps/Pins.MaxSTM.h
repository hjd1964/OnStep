// -------------------------------------------------------------------------------------------------
// Pin map for OnStep MaxSTM STM32F411CE/STM32F401CE PCB (or the Blackpill https://github.com/WeActTC/MiniF4-STM32F4x1)

// For an 8KB EEPROM on the MaxSTM3.6I
#if PINMAP == MaxSTM3I
  #define NV_M24C32
#endif

#if defined(STM32F411xE) || defined(STM32F401xC)

// The multi-purpose pins (Aux3..Aux8 can be analog pwm/dac if supported)
#define Aux0                PB12     // Status LED
#define Aux2                PA13     // PPS
#define Aux3                PB13     // Home SW
#define Aux4                PB14     // OneWire, Home SW
#define Aux5                 PA9     // TX1 
#define Aux6                PA10     // RX1
#define Aux7                PB15     // Limit SW
#define Aux8                 PA8     // Status2 LED, Reticle LED

// Misc. pins
#ifndef OneWirePin
  #define OneWirePin        Aux4     // Default Pin for OneWire bus (note: this pin has a 0.1uF capacitor that must be removed for OneWire to function)
#endif
#define AddonBootModePin     PB0     // ESP8266 GPIO0 (Dir2)
#define AddonResetPin       Aux2     // ESP8266 RST

// The PEC index sense is a logic level input, resets the PEC index on rising edge then waits for 60 seconds before allowing another reset
#define PecPin               PB1
#define AnalogPecPin         PB1     // PEC Sense, analog or digital

// The status LED is a two wire jumper with a 10k resistor in series to limit the current to the LED
#define LEDnegPin           Aux0     // Drain
#define LEDneg2Pin          Aux8     // Drain
#define ReticlePin          Aux8     // Drain

// For a piezo buzzer
#define TonePin             PA14     // Tone

// The PPS pin is a 3.3V logic input, OnStep measures time between rising edges and adjusts the internal sidereal clock frequency
#ifdef MAXSTM_AUX0_PPS
  #define PpsPin            Aux0     // PPS time source, GPS for example (MaxSTM version 3.6)
#else
  #define PpsPin            Aux2     // PPS time source, GPS for example (MaxSTM version 3.61 and later)
#endif

#define LimitPin            Aux7     // The limit switch sense is a logic level input normally pull high (2k resistor,) shorted to ground it stops gotos/tracking

// Axis1 RA/Azm step/dir driver
#define Axis1_EN             OFF     // No ENable pin control
#define Axis1_M0             PA7     // SPI MOSI
#define Axis1_M0PORT       GPIOA
#define Axis1_M0BIT   GPIO_PIN_7
#define Axis1_M1             PA5     // SPI SCK
#define Axis1_M1PORT       GPIOA
#define Axis1_M1BIT   GPIO_PIN_5
#define Axis1_M2             PA1     // SPI CS
#define Axis1_M2PORT       GPIOA
#define Axis1_M2BIT   GPIO_PIN_1
#define Axis1_M3             PA6     // SPI MISO
#define Axis1_M3PORT       GPIOA
#define Axis1_M3BIT   GPIO_PIN_6
#define Axis1_STEP          PB10     // Step
#define Axis1_StpPORT      GPIOB
#define Axis1_StpBIT GPIO_PIN_10
#define Axis1_DIR            PB2     // Dir
#define Axis1_DirPORT      GPIOB
#define Axis1_DirBIT  GPIO_PIN_2
#define Axis1_DECAY     Axis1_M2     // Decay mode
#define Axis1_FAULT         Aux1     // SPI MISO/Fault (or ESP8266 GPIO0 on MaxPCB)
#define Axis1_HOME          Aux3     // Sense home position Axis1

// Axis2 Dec/Alt step/dir driver
#define Axis2_EN             OFF     // No ENable pin control
#define Axis2_M0             PA7     // SPI MOSI
#define Axis2_M0PORT       GPIOA
#define Axis2_M0BIT   GPIO_PIN_7
#define Axis2_M1             PA5      // SPI SCK
#define Axis2_M1PORT       GPIOA
#define Axis2_M1BIT   GPIO_PIN_5
#define Axis2_M2             PA0     // SPI CS
#define Axis2_M2PORT       GPIOA
#define Axis2_M2BIT   GPIO_PIN_0
#define Axis2_M3             PA6     // SPI MISO
#define Axis2_M3PORT       GPIOA
#define Axis2_M3BIT   GPIO_PIN_6
#define Axis2_STEP           PA4     // Step
#define Axis2_StpPORT      GPIOA
#define Axis2_StpBIT  GPIO_PIN_4
#define Axis2_DIR            PB0     // Dir (ESP8266 GPIO0 on MaxPCB3)
#define Axis2_DirPORT      GPIOB
#define Axis2_DirBIT  GPIO_PIN_0
#define Axis2_DECAY     Axis2_M2     // Decay mode
#define Axis2_FAULT         Aux2
#define Axis2_HOME          Aux4     // Sense home position

// For rotator stepper driver
#define Axis3_EN             OFF     // No ENable pin control
#define Axis3_M0             PA7     // SPI MOSI
#define Axis3_M1             PA5     // SPI SCK
#define Axis3_M2            PC15     // SPI CS
#define Axis3_M3             PA6     // SPI MISO
#define Axis3_STEP           PB8     // Step
#define Axis3_DIR           PC13     // Dir

// For focuser1 stepper driver
#define Axis4_EN             OFF     // No ENable pin control
#define Axis4_M0             PA7     // SPI MOSI
#define Axis4_M1             PA5     // SPI SCK
#define Axis4_M2            PC14     // SPI CS
#define Axis4_M3             PA6     // SPI MISO
#define Axis4_STEP           PB9     // Step
#define Axis4_DIR           PC13     // Dir

// For focuser2 stepper driver
#define Axis5_EN             OFF     // No ENable pin control
#define Axis5_M0             PA7     // SPI MOSI
#define Axis5_M1             PA5     // SPI SCK
#define Axis5_M2            PC15     // SPI CS
#define Axis5_M3             PA6     // SPI MISO
#define Axis5_STEP           PB8     // Step
#define Axis5_DIR           PC13     // Dir

// ST4 interface
#define ST4RAw              PA15     // ST4 RA- West
#define ST4DEs               PB3     // ST4 DE- South
#define ST4DEn               PB4     // ST4 DE+ North
#define ST4RAe               PB5     // ST4 RA+ East

#else
#error "Wrong processor for this configuration!"

#endif
