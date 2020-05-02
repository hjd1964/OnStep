// -------------------------------------------------------------------------------------------------
// Pin map for OnStep MaxPCB (Teensy3.5/3.6)

#if defined(STM32F446xx)

// The multi-purpose pins (Aux3..Aux8 can be analog pwm/dac if supported)
/*
#define Aux0                 -1      // Status LED
#define Aux1                 -1     // ESP8266 GPIO0, SPI MISO/Fault
#define Aux2                 -1      // ESP8266 RST, SPI MISO/Fault
#define Aux3                 -1        // Home SW
#define Aux4                 -1      // OneWire, Home SW
#define Aux5                 -1     // Axis3_EN; true analog output
#define Aux6                 -1     // Axis4_EN; true analog output
#define Aux7                 -1       // Limit SW
#define Aux8                 -1     // Status2 LED, Reticle LED
*/

// Misc. pins
#ifndef DS3234_CS_PIN
  #define DS3234_CS_PIN      -1     // Default CS Pin for DS3234 on SPI
#endif
#ifndef OneWirePin
  #define OneWirePin         -1     // Default Pin for OneWire bus (note: this pin has a 0.1uF capacitor that must be removed for OneWire to function)
#endif
#define ESP8266Gpio0Pin      -1     // ESP8266 GPIO0 (Dir2)
#define ESP8266RstPin        -1     // ESP8266 RST

// The PEC index sense is a logic level input, resets the PEC index on rising edge then waits for 60 seconds before allowing another reset
#define PecPin               PC1
#define AnalogPecPin         -1     // PEC Sense, analog or digital

// The status LED is a two wire jumper with a 10k resistor in series to limit the current to the LED
#define LEDnegPin          PB14     // Drain
//#define LEDneg2Pin         Aux8     // Drain
#define ReticlePin         PA1      // Drain

// Temperature sensors (From Marlin)
#define Temp0Pin           PC0
#define Temp1Pin           PC1
#define Temp2Pin           PC2
#define Temp3Pin           PC3

// Heaters (From Marlin)
#define Heater0Pin         PB3
#define Heater1Pin         PB4
#define Heater2Pin         PB15
#define Heater3Pin         PC8

// Fans (From Marlin)
#define Fan0Pin            PB0
#define Fan1Pin            PB1
#define Fan2Pin            PB2

// For a piezo buzzer
#define TonePin              PC9     // Tone

// The PPS pin is a 3.3V logic input, OnStep measures time between rising edges and adjusts the internal sidereal clock frequency
#define PpsPin               PC2   // PPS time source, GPS for example

#define LimitPin           PA0     // The limit switch sense is a logic level input normally pull high (2k resistor,) shorted to ground it stops gotos/tracking

// Axis1 RA/Azm step/dir driver
#define Axis1_EN           PE12     // Enable
#define Axis1_M0             -1     // Microstep Mode 0 or SPI MOSI
#define Axis1_M1             -1     // Microstep Mode 1 or SPI SCK
#define Axis1_M2           PE7      // Microstep Mode 2 or SPI CS or Decay Mode
#define Axis1_M3           Aux1     // SPI MISO/Fault (or ESP8266 GPIO0 on MaxPCB)
#define Axis1_STEP         PE11     // Step
#define Axis1_DIR          PE10     // Dir
#define Axis1_DECAY    Axis1_M2     // Decay mode
#define Axis1_FAULT        Aux1     // SPI MISO/Fault (or ESP8266 GPIO0 on MaxPCB)
#define Axis1_HOME         PB10     // Sense home position Axis1

// Axis2 Dec/Alt step/dir driver
#define Axis2_EN           PD8     // Enable
#define Axis2_M0           -1     // Microstep Mode 0 or SPI MOSI
#define Axis2_M1           -1     // Microstep Mode 1 or SPI SCK
#define Axis2_M2           PE15     // Microstep Mode 2 or SPI CS or Decay Mode
#define Axis2_M3         Aux1     // SPI MISO
#define Axis2_STEP         PD8     // Step
#define Axis2_DIR          PB12     // Dir (ESP8266 GPIO0 on MaxPCB3)
#define Axis2_DECAY    Axis2_M2     // Decay mode
#define Axis2_FAULT        Aux2
#define Axis2_HOME         PC7

// For rotator stepper driver
#define Axis3_EN           PD15     // Enable
#define Axis3_M0           -1     // SPI MOSI
#define Axis3_M1           -1     // SPI SCK
#define Axis3_M2           PD10     // SPI CS or Decay Mode
#define Axis3_M3         Aux1     // SPI MISO
#define Axis3_STEP         PD14     // Step
#define Axis3_DIR          PD13     // Dir

// For focuser1 stepper driver
#define Axis4_EN           PD4     // Enable
#define Axis4_M0           -1     // SPI MOSI
#define Axis4_M1           -1     // SPI SCK
#define Axis4_M2           PD7     // SPI CS or Decay Mode
#define Axis4_M3         Aux1     // SPI MISO
#define Axis4_STEP         PD5     // Step
#define Axis4_DIR          PD6     // Dir

// For focuser2 stepper driver
#define Axis5_EN           PE5     // Enable
#define Axis5_M0           -1     // SPI MOSI
#define Axis5_M1           -1     // SPI SCK
#define Axis5_M2           PC14     // SPI CS or Decay Mode
#define Axis5_M3         Aux1     // SPI MISO
#define Axis5_STEP         PE6     // Step
#define Axis5_DIR          PC13     // Dir

// ST4 interface
#define ST4RAw               PA8      // ST4 RA- West
#define ST4DEs               PD2      // ST4 DE- South
#define ST4DEn               PC12     // ST4 DE+ North
#define ST4RAe               PD1       // ST4 RA+ East

#else
#error "Wrong processor for this configuration!"

#endif
