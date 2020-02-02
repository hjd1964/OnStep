// -------------------------------------------------------------------------------------------------
// Pin map for OnStep MaxPCB (Teensy3.5/3.6)

#if defined(__MK64FX512__) || defined(__MK66FX1M0__)

// The multi-purpose pins (Aux3..Aux8 can be analog pwm/dac if supported)
#define Aux0                 19
#define Aux1                 18
#define Aux2                  5
#define Aux3                 36     // Home SW, this should also be ok as pwm analog output (w/#define Aux3_Analog)
#define Aux4                 39     // OneWire, Home SW
#define Aux5                A21     // true analog output
#define Aux5_Analog
#define Aux6                A22     // true analog output
#define Aux6_Analog
#define Aux7                  4     // Limit SW, should be ok as pwm analog output (w/#define Aux7_Analog)
#define Aux8                 22     // should be ok as pwm analog output (w/#define Aux8_Analog)

// Misc. pins
#ifndef DS3234_CS_PIN
  #define DS3234_CS_PIN      10     // Default CS Pin for DS3234 on SPI
#endif
#ifndef OneWirePin
  #define OneWirePin       Aux4     // Default Pin for one wire bus
#endif
#if PINMAP == MaxPCB3
  #define ESP8266Gpio0Pin     2     // ESP8266 GPIO0 (Dir2)
  #define ESP8266RstPin    Aux2     // ESP8266 RST
#elif PINMAP == MaxPCB
  #define ESP8266Gpio0Pin  Aux1     // ESP8266 GPIO0 or SPI MISO/Fault
  #define ESP8266RstPin    Aux2     // ESP8266 RST or SPI MISO/Fault
#endif

// The PEC index sense is a logic level input, resets the PEC index on rising edge then waits for 60 seconds before allowing another reset
#define PecPin               23
#define AnalogPecPin         23     // PEC Sense, analog or digital

// The status LED is a two wire jumper with a 10k resistor in series to limit the current to the LED
#define LEDnegPin          Aux0     // Drain
#define LEDneg2Pin         Aux8     // Drain
#define ReticlePin         Aux8     // Drain

// For a piezo buzzer
#define TonePin              29     // Tone

// The PPS pin is a 3.3V logic input, OnStep measures time between rising edges and adjusts the internal sidereal clock frequency
#define PpsPin               28     // PPS time source, GPS for example

#define LimitPin           Aux7     // The limit switch sense is a logic level input normally pull high (2k resistor,) shorted to ground it stops gotos/tracking

// Axis1 RA/Azm step/dir driver
#define Axis1DirPin          21     // Dir
#define Axis1StepPin         20     // Step

#define Axis1_EN             14     // Enable
#define Axis1_M0             15     // Microstep Mode 0 or SPI MOSI
#define Axis1_M1             16     // Microstep Mode 1 or SPI SCK
#define Axis1_M2             17     // Microstep Mode 2 or SPI CS or Decay Mode
#define Axis1_M3           Aux1     // SPI MISO/Fault (or ESP8266 GPIO0 on MaxPCB)
#define Axis1ModePin   Axis1_M2     // Decay mode
#define Axis1FaultPin      Aux1     // SPI MISO/Fault (or ESP8266 GPIO0 on MaxPCB)
#define Axis1HomePin       Aux3     // Sense home position Axis1

// Axis2 Dec/Alt step/dir driver
#define Axis2DirPin           2     // Dir (ESP8266 GPIO0 on MaxPCB3)
#define Axis2StepPin          3     // Step

#define Axis2_EN              9     // Enable
#if PINMAP == MaxPCB3
  #define Axis2_M0           15     // Microstep Mode 0 or SPI MOSI
  #define Axis2_M1           16     // Microstep Mode 1 or SPI SCK
  #define Axis2_M2            6     // Microstep Mode 2 or SPI CS or Decay Mode
  #define Axis2_M3         Aux1     // SPI MISO
#else
  #define Axis2_M0            8     // Microstep Mode 0 or SPI MOSI
  #define Axis2_M1            7     // Microstep Mode 1 or SPI SCK
  #define Axis2_M2            6     // Microstep Mode 2 or SPI CS or Decay Mode
  #define Axis2_M3         Aux2     // ESP8266 RST or SPI MISO/Fault
#endif
#define Axis2ModePin   Axis2_M2     // Decay mode
#define Axis2FaultPin      Aux2
#define Axis2HomePin       Aux4     // Sense home position

// For rotator stepper driver
#define Axis3_EN           Aux5     // Enable
#if PINMAP == MaxPCB3
  #define Axis3_M0           15     // SPI MOSI
  #define Axis3_M1           16     // SPI SCK
  #define Axis3_M2            7     // SPI CS or Decay Mode
  #define Axis3_M3         Aux1     // SPI MISO
#endif
#define Axis3StepPin         30     // Step
#define Axis3DirPin          33     // Dir

// For focuser1 stepper driver
#define Axis4_EN           Aux6     // Enable
#if PINMAP == MaxPCB3
  #define Axis4_M0           15     // SPI MOSI
  #define Axis4_M1           16     // SPI SCK
  #define Axis4_M2            8     // SPI CS or Decay Mode
  #define Axis4_M3         Aux1     // SPI MISO
#endif
#define Axis4StepPin         34     // Step
#define Axis4DirPin          35     // Dir

// For focuser2 stepper driver
#define Axis5_EN           Aux5     // Enable
#if PINMAP == MaxPCB3
  #define Axis5_M0           15     // SPI MOSI
  #define Axis5_M1           16     // SPI SCK
  #define Axis5_M2            7     // SPI CS or Decay Mode
  #define Axis5_M3         Aux1     // SPI MISO
#endif
#define Axis5StepPin         30     // Step
#define Axis5DirPin          33     // Dir

// ST4 interface
#define ST4RAw               24     // ST4 RA- West
#define ST4DEs               25     // ST4 DE- South
#define ST4DEn               26     // ST4 DE+ North
#define ST4RAe               27     // ST4 RA+ East

#else
#error "Wrong processor for this configuration!"

#endif
