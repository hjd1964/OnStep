// -------------------------------------------------------------------------------------------------
// Pin map for OnStep using RAMPS 1.4 Pin defs (Arduino Mega2560 & Arduino DUE)

#if defined(__AVR_ATmega2560__) || defined(__AVR_ATmega1280__) || defined(__SAM3X8E__)

// The multi-purpose pins (Aux3..Aux8 can be analog (pwm/dac) if supported)
#define Aux0                 11     // Status LED
#define Aux1                 29     // ESP8266 GPIO0, SPI MISO/Fault
#define Aux2                 37     // ESP8266 RST, SPI MISO/Fault
#if PINMAP == MksGenL1
  #define Aux3               14     // Home SW (GenL1 Y- endstop)
  #define Aux4               15     // OneWire (requires h/w modification,) Home SW (GenL1 Y+ endstop)
#else
  #define Aux3               32     // Home SW; note modified pinmap 10/2/19 Aux3 and Aux4 were changed, 9/30/19 Aux5 was removed (Aux3 not present Gen-L1)
  #define Aux4               39     // OneWire, Home SW (Aux4 not present Gen-L1)
#endif
#define Aux5                  7     // Dew Heater0 (E1 , MKS Gen-L)
#define Aux6                  8     // Dew Heater1 (BED, MKS Gen-L, RAMPS1.4)
#define Aux7                  9     // Dew Heater2 (FAN, MKS Gen-L, RAMPS1.4)
#define Aux8                 10     // Dew Heater3 (E0 , MKS Gen-L, RAMPS1.4)

#ifndef DS3234_CS_PIN
  #define DS3234_CS_PIN      53     // Default CS Pin for DS3234 on SPI
#endif
#ifndef BME280_CS_PIN
  #define BME280_CS_PIN      49     // Default CS Pin for BME280 on SPI
#endif
#ifndef OneWirePin
  #define OneWirePin       Aux4     // Default Pin for OneWire bus
#endif
#define ESP8266Gpio0Pin    Aux1     // ESP8266 GPIO0 or SPI MISO/Fault
#define ESP8266RstPin      Aux2     // ESP8266 RST or SPI MISO/Fault

// For software SPI
#define SSPI_SCK 52
#define SSPI_MISO 50
#define SSPI_MOSI 51

// The PEC index sense is a 5V logic input, resets the PEC index on rising edge then waits for 60 seconds before allowing another reset
#if PINMAP == MksGenL1
  #define PecPin             41     // GenL1 EXP2
  #define AnalogPecPin       -1     // N/A
#else
  #define PecPin             57     // RAMPS AUX1, A-OUT (1=+5V, 2=GND, 3=PEC)
  #define AnalogPecPin       A3     // Note A3 is (57)
#endif

// The limit switch sense is a 5V logic input which uses the internal (or external 2k) pull up, shorted to ground it stops gotos/tracking
#define LimitPin              3     // RAMPS X- (1=LMT, 2=GND, 3=+5)

// The status LED is a two wire jumper with a 10k resistor in series to limit the current to the LED
#define LEDnegPin          Aux0     // RAMPS SERVO1 (1=GND, 2=+5, 3=LED-) (active LOW)
#define LEDneg2Pin            6     // RAMPS SERVO2 (1=GND, 2=+5, 3=LED-) (active LOW)
#define ReticlePin            5     // RAMPS SERVO3 (1=GND, 2=+5, 3=LED-) (active LOW)

// Pin for a piezo buzzer output on RAMPS Y-MIN
#define TonePin               4     // RAMPS SERVO4 (1=GND, 2=+5, 3=TONE+) (active HIGH)

// The PPS pin is a 5V logic input, OnStep measures time between rising edges and adjusts the internal sidereal clock frequency
#define PpsPin                2     // RAMPS X+, Interrupt 0 on Pin 2

// Pins to Axis1 RA/Azm on RAMPS X
#define Axis1_EN             38     // Enable
#if PINMAP == MksGenL2
  #define Axis1_M0           51     // SPI MOSI
  #define Axis1_M0PORT    PORTB
  #define Axis1_M0BIT         2
  #define Axis1_M1           52     // SPI SCK
  #define Axis1_M1PORT    PORTB
  #define Axis1_M1BIT         1
  #define Axis1_M2           A9     // SPI CS
  #define Axis1_M2PORT    PORTK
  #define Axis1_M2BIT         1
  #define Axis1_M3           50     // SPI MISO
#else
  #define Axis1_M0           23     // Microstep Mode 0 or SPI MOSI
  #define Axis1_M0PORT    PORTA
  #define Axis1_M0BIT         1
  #define Axis1_M1           25     // Microstep Mode 1 or SPI SCK
  #define Axis1_M1PORT    PORTA
  #define Axis1_M1BIT         3
  #define Axis1_M2           27     // Microstep Mode 2 or SPI CS
  #define Axis1_M2PORT    PORTA
  #define Axis1_M2BIT         5
  #define Axis1_M3         Aux1     // SPI MISO/Fault
#endif
#define Axis1_STEP           54     // Step
#define Axis1_StepPORT    PORTF     //
#define Axis1_StepBIT         0     //
#define Axis1_DIR            55     // Dir
#define Axis1_DirPORT     PORTF     //
#define Axis1_DirBIT          1     //
#define Axis1_DECAY    Axis1_M2     // Decay mode
#define Axis1_FAULT        Aux1
#define Axis1_HOME         Aux3     // Sense home position

// Axis2 Dec/Alt step/dir driver on RMAPS Y
#define Axis2_EN             56     // Enable (Pin A2)
#if PINMAP == MksGenL2
  #define Axis2_M0           51     // SPI MOSI
  #define Axis2_M0PORT    PORTB
  #define Axis2_M0BIT         2
  #define Axis2_M1           52     // SPI SCK
  #define Axis2_M1PORT    PORTB
  #define Axis2_M1BIT         1
  #define Axis2_M2          A10     // SPI CS
  #define Axis2_M2PORT    PORTK
  #define Axis2_M2BIT         2
  #define Axis2_M3           50     // SPI MISO
#else
  #define Axis2_M0           31     // Microstep Mode 0 or SPI MOSI
  #define Axis2_M0PORT    PORTC
  #define Axis2_M0BIT         6
  #define Axis2_M1           33     // Microstep Mode 1 or SPI SCK
  #define Axis2_M1PORT    PORTC
  #define Axis2_M1BIT         4
  #define Axis2_M2           35     // Microstep Mode 2 or SPI CS
  #define Axis2_M2PORT    PORTC
  #define Axis2_M2BIT         2
  #define Axis2_M3         Aux2     // SPI MISO/Fault
#endif
#define Axis2_STEP           60     // Step (Pin A6)
#define Axis2_StepPORT    PORTF     //
#define Axis2_StepBIT         6     //
#define Axis2_DIR            61     // Dir (Pin A7)
#define Axis2_DirPORT     PORTF     //
#define Axis2_DirBIT          7     //
#define Axis2_DECAY    Axis2_M2     // Decay mode
#define Axis2_FAULT        Aux2
#define Axis2_HOME         Aux4     // Sense home position

// For rotator stepper driver on RAMPS Z
#define Axis3_EN             62     // Enable (Pin A8)
#if PINMAP == MksGenL2
  #define Axis3_M0           51     // SPI MOSI
  #define Axis3_M1           52     // SPI SCK
  #define Axis3_M2          A11     // SPI CS
  #define Axis3_M3           50     // SPI MISO
#endif
#define Axis3_STEP           46     // Step
#define Axis3_DIR            48     // Dir

// For focuser1 stepper driver on RAMPS E0
#define Axis4_EN             24     // Enable
#if PINMAP == MksGenL2
  #define Axis4_M0           51     // SPI MOSI
  #define Axis4_M1           52     // SPI SCK
  #define Axis4_M2          A12     // SPI CS
  #define Axis4_M3           50     // SPI MISO
#endif
#define Axis4_STEP           26     // Step
#define Axis4_DIR            28     // Dir

// For focuser2 stepper driver on RAMPS E1
#define Axis5_EN             30     // Enable
#if PINMAP == MksGenL2 || PINMAP == MksGenL21
  #define Axis5_M0           51     // SPI MOSI
  #define Axis5_M1           52     // SPI SCK
  #if PINMAP == MksGenL21
    #define Axis5_M2         12     // SPI CS
  #else
    #define Axis5_M2         21     // SPI CS
  #endif
  #define Axis5_M3           50     // SPI MISO
#endif
#define Axis5_STEP           36     // Step
#define Axis5_DIR            34     // Dir

#if PINMAP == MksGenL2
  // ST4 interface on MksGenL2 EXP-1
  #define ST4RAw             27     // ST4 RA- West
  #define ST4DEs             23     // ST4 DE- South
  #define ST4DEn             25     // ST4 DE+ North
  #define ST4RAe             35     // ST4 RA+ East
#else
  // ST4 interface on RAMPS AUX-2
  #define ST4RAw             A9     // ST4 RA- West
  #define ST4DEs             40     // ST4 DE- South
  #define ST4DEn             42     // ST4 DE+ North
  #define ST4RAe            A11     // ST4 RA+ East
#endif

#else
#error "Wrong processor for this configuration!"

#endif
