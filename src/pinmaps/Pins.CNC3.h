// -------------------------------------------------------------------------------------------------
// Pin map for CNC Shield Version 3 (with WeMos D1 R32)

// Note: SPI for TMC drivers: GPIO32 (MOSI), GPIO33 (SCK), GPIO15 (CS Axis1) on WeMos D1 R32 PCB are more difficult to wire into but optional. GPIO18 (CS Axis2) is on top.

#if defined(ESP32)

// Serial2 (SerialB) needs to be re-mapped to GPIO13 (RX) and GPIO5 (TX)
#define SERIAL_B_RX          13
#define SERIAL_B_TX           5

// The multi-purpose pins (Aux3..Aux8 can be analog pwm/dac if supported)
#define Aux2                 18     // ESP8266 RST control, or Axis2 CS support
#define Aux3                 21     // Home SW for Axis1, or I2C SDA {assuming default SCL/SDA pins are present}
#define Aux4                 22     // Home SW for Axis2, or I2C SCL
#define Aux7                 39     // Limit SW, PPS, etc.
#define Aux8                 23     // {was 25} OneWire, Status LED, Status2 LED, Reticle LED, etc.

// Misc. pins
#ifndef OneWirePin
  #define OneWirePin       Aux8     // Default Pin for OneWire bus
#endif
#define ESP8266Gpio0Pin      27     // {was 26} ESP8266 GPIO0 (Dir2)
#define ESP8266RstPin      Aux2     // ESP8266 RST

// The PEC index sense is a logic level input, resets the PEC index on rising edge then waits for 60 seconds before allowing another reset
#define PecPin               36     // [input only 36]
#define AnalogPecPin         A0     // PEC Sense, analog or digital (GPIO36)

// The status LED is a two wire jumper with a 10k resistor in series to limit the current to the LED
#define LEDnegPin          Aux8     // Drain
#define LEDneg2Pin         Aux8     // Drain
#define ReticlePin         Aux8     // Drain

// For a piezo buzzer
#define TonePin               2     // Tone

// The PPS pin is a 3.3V logic input, OnStep measures time between rising edges and adjusts the internal sidereal clock frequency
#define PpsPin             Aux7     // PPS time source, GPS for example

#define LimitPin           Aux7     // The limit switch sense is a logic level input normally pull high (2k resistor,) shorted to ground it stops gotos/tracking

// Axis1 RA/Azm step/dir driver
#define Axis1_EN             12     // [must be low at boot 12] Enable
#define Axis1_M0             32     // {was 13} Microstep Mode 0 or SPI MOSI
#define Axis1_M1             33     // {was 14} Microstep Mode 1 or SPI SCK
#define Axis1_M2             15     // {was 23} Microstep Mode 2 or SPI CS
#define Axis1_M3           Aux2     // SPI MISO/Fault
#define Axis1_STEP           26     // {was 18} Step
#define Axis1_DIR            16     // {was 0} Dir
#define Axis1_DECAY    Axis1_M2     // Decay mode
#define Axis1_HOME         Aux3     // Sense home position

// Axis2 Dec/Alt step/dir driver
#define Axis2_EN             12     // [must be low at boot 12] Enable
#define Axis2_M0             32     // {was 13} Microstep Mode 0 or SPI MOSI
#define Axis2_M1             33     // {was 14} Microstep Mode 1 or SPI SCK
#define Axis2_M2              0     // {was 5} Microstep Mode 2 or SPI CS
#define Axis2_M3           Aux2     // SPI MISO/Fault
#define Axis2_STEP           25     // {was 27} Step
#define Axis2_DIR            27     // {was 26} Dir
#define Axis2_DECAY    Axis2_M2     // Decay mode
#define Axis2_HOME         Aux4     // Sense home position

// For rotator stepper driver
#define Axis3_EN             -1     // Enable {enable pin disabled so focuser/rotator doesn't impact main axes EN state}
#define Axis3_STEP           19     // {was 2} Step
#define Axis3_DIR            14     // {was 15} Dir

// For focuser1 stepper driver
#define Axis4_EN             -1     // Enable {enable pin disabled so focuser/rotator doesn't impact main axes EN state}
#define Axis4_STEP           17     // Step
#define Axis4_DIR            14     // {was 15} Dir

// For focuser2 stepper driver
#define Axis5_EN             -1     // Enable {enable pin disabled so focuser/rotator doesn't impact main axes EN state}
#define Axis5_STEP           19     // {was 2} Step
#define Axis5_DIR            14     // {was 15} Dir

// ST4 interface
#define ST4RAw               34     // [input only 34] ST4 RA- West
#define ST4DEs               18     // {was 32} ST4 DE- South
#define ST4DEn                4     // {was 33} ST4 DE+ North
#define ST4RAe               35     // [input only 35] ST4 RA+ East

#else
#error "Wrong processor for this configuration!"

#endif
