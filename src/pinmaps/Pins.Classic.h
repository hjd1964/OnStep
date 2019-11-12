// -------------------------------------------------------------------------------------------------
// Pin map for legacy OnStep Classic (Mega2560 or Teensy3.x)

#if defined(__AVR_ATmega2560__)

// Misc. pins
#if defined(ST4_ALTERNATE_PINS_ON) || (PINMAP == ClassicShield)
  #ifndef DS3234_CS_PIN
    #define DS3234_CS_PIN      53     // Default CS Pin for DS3234 on SPI
  #endif
  #ifndef BME280_CS_PIN
    #define BME280_CS_PIN      48     // Default CS Pin for BME280 on SPI
  #endif
#endif

// The PEC index sense is a 5V logic input, resets the PEC index on rising edge then waits for 60 seconds before allowing another reset
#define PecPin                2
#define AnalogPecPin          1

// The limit switch sense is a 5V logic input which uses the internal (or external 2k) pull up, shorted to ground it stops gotos/tracking
#define LimitPin              3

// The status LED is a two wire jumper with a 10k resistor in series to limit the current to the LED
//                                           Atmel 2560 port/bit
#define LEDposPin             8     // LED   PH5
#define LEDnegPin             9     // GND   PH6
#define LEDneg2Pin           10     // PGND  PB4
#define ReticlePin           44     // PGND

// For a piezo buzzer
#define TonePin              42     // Tone

// The PPS pin is a 5V logic input, OnStep measures time between rising edges and adjusts the internal sidereal clock frequency
#define PpsPin               21     // Interrupt 2 on Pin 21 (alternate Int3 on Pin20)

// Obsolete pins that would power Big Easy Drivers (A4988) back in the old days
#define POWER_SUPPLY_PINS_OFF
#define Axis15vPin           12     // 5V    PB6
#define Axis25vPin            5     // 5V    PE3
#define Axis2GndPin           7     // GND   PH4

// Axis1 RA/Azm step/dir driver
#define Axis1_EN             25     // Enable
#define Axis1_M0             22     // Microstep Mode 0
#define Axis1_M1             23     // Microstep Mode 1
#define Axis1_M2             24     // Microstep Mode 2
#define Axis1_M3             26     // SPI MISO/Fault
#define Axis1StepPin         13     // Step PB7
#define Axis1StepBit          7     //
#define Axis1StepPORT     PORTB     //
#define Axis1DirPin          11     // Dir  PB5
#define Axis1DirBit           5     //
#define Axis1DirPORT      PORTB     //
#define Axis1FaultPin        26     // SPI MISO/Fault
#define Axis1ModePin         32     // Mode switch for Axis1

// Axis2 Dec/Alt step/dir driver
#define Axis2_EN             30     // Enable
#define Axis2_M0             27     // Microstep Mode 0
#define Axis2_M1             28     // Microstep Mode 1
#define Axis2_M2             29     // Microstep Mode 2
#define Axis2_M3             31     // SPI MISO/Fault
#define Axis2StepPin          6     // Step  PH3
#define Axis2StepBit          3     //
#define Axis2StepPORT     PORTH     //
#define Axis2DirPin           4     // Dir   PG5
#define Axis2DirBit           5     //
#define Axis2DirPORT      PORTG     //
#define Axis2FaultPin        31     // SPI MISO/Fault
#define Axis2ModePin         33     // Mode switch for Axis2

// Pins to rotator stepper driver
#define Axis3_EN             -1     // Enable
#define Axis3StepPin         A9     // Step
#define Axis3DirPin          A8     // Dir

// Pins to focuser1 stepper driver
#define Axis3_EN             -1     // Enable
#define Axis4StepPin        A11     // Step
#define Axis4DirPin         A10     // Dir

// Pins to focuser2 stepper driver
#define Axis3_EN             -1     // Enable
#define Axis5StepPin        A13     // Step
#define Axis5DirPin         A12     // Dir

// ST4 interface
#if defined(ST4_ALTERNATE_PINS_ON) || (PINMAP == ClassicShield)
  #define ST4RAw               47     // ST4 RA- West
  #define ST4DEs               43     // ST4 DE- South
  #define ST4DEn               45     // ST4 DE+ North
  #define ST4RAe               49     // ST4 RA+ East
#else
  #define ST4RAw               47     // ST4 RA- West
  #define ST4DEs               49     // ST4 DE- South
  #define ST4DEn               51     // ST4 DE+ North
  #define ST4RAe               53     // ST4 RA+ East
#endif

#elif defined(__MK20DX256__) || defined(__MK64FX512__) || defined(__MK66FX1M0__)

// The PEC index sense is a logic level input, resets the PEC index on rising edge then waits for 60 seconds before allowing another reset
#define PecPin                2
#define AnalogPecPin         14

// The limit switch sense is a 3.3V logic input which uses the internal pull up, shorted to ground it stops gotos/tracking
#define LimitPin              3

// The status LED is a two wire jumper with a 10k resistor in series to limit the current to the LED
#define LEDposPin             8     // LED
#define LEDnegPin             9     // GND
#define LEDneg2Pin            7     // GND
#define ReticlePin            9     // GND

// For a piezo buzzer
#define TonePin              29     // Tone

// The PPS pin is a logic level input, OnStep measures time between rising edges and adjusts the internal sidereal clock frequency
#define PpsPin               23     // PPS time source, GPS for example

// Obsolete pins that would power stepper drivers in the old days
#define POWER_SUPPLY_PINS_OFF
#define Axis15vPin    11    // Pin 11 (3.3V)
#define Axis25vPin     5    // Pin 5 (3.3V)
#define Axis2GndPin    7    // Pin 7 (GND)

// Axis1 RA/Azm step/dir driver
#define Axis1_EN             16     // Enable
#define Axis1_M0             13     // Microstep Mode 0
#define Axis1_M1             14     // Microstep Mode 1
#define Axis1_M2             15     // Microstep Mode 2
#define Axis1_M3             17     // ESP8266 GPIO0 or SPI MISO
#define Axis1StepPin         12     // Step
#define Axis1DirPin          10     // Dir
#define Axis1FaultPin        17     // Fault

// Axis2 Dec/Alt step/dir driver
#define Axis2_EN             21     // Enable
#define Axis2_M0             18     // Microstep Mode 0
#define Axis2_M1             19     // Microstep Mode 1
#define Axis2_M2             20     // Microstep Mode 2
#define Axis2_M3             22     // ESP8266 Rst or SPI MISO
#define Axis2FaultPin        22     // Fault
#define Axis2StepPin          6     // Step
#define Axis2DirPin           4     // Dir

// For rotator stepper driver
#define Axis3_EN             -1     // Enable
#define Axis3StepPin         30     // Step
#define Axis3DirPin          33     // Dir

// For focuser1 stepper driver  
#if defined(__MK64FX512__) || defined(__MK66FX1M0__)
  // teensy3.5/3.6
  #define Axis4StepPin       34     // Step
  #define Axis4DirPin        35     // Dir
#else
  // teensy3.2
  #define Axis4StepPin       31     // Step
  #define Axis4DirPin        32     // Dir
#endif
#define Axis4_EN             -1     // Enable

// For focuser2 stepper driver
#define Axis5StepPin         30     // Step
#define Axis5DirPin          33     // Dir
#define Axis5_EN             -1     // Enable

// ST4 interface
#define ST4RAw               24     // ST4 RA- West
#define ST4DEs               25     // ST4 DE- South
#define ST4DEn               26     // ST4 DE+ North
#define ST4RAe               27     // ST4 RA+ East

#else
#error "Wrong processor for this configuration!"

#endif
