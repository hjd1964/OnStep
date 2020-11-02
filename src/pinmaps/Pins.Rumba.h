// -------------------------------------------------------------------------------------------------
// Pin map for OnStep using RUMBA Pin defs (Arduino Mega2560)

#warning "This is a new, experimental, pin-map use at your own risk!"

#if defined(__AVR_ATmega2560__)

// The multi-purpose pins (Aux3..Aux8 can be analog (pwm/dac) if supported)
#define Aux0                PB7     // Status LED
// Note: PWM is not allowed on any of the following (auxiliary feature ANALOG is not supported)
#define Aux1                PC7     // EXP3
#define Aux2                PC6     // EXP3
#define Aux3                PE4     // DEW_HEATER or SWITCH (HE0)  (D2)
#define Aux4                PE5     // DEW_HEATER or SWITCH (HE1)  (D3)
#define Aux5                PH3     // DEW_HEATER or SWITCH (HE2)  (D6)
#define Aux6                PH6     // DEW_HEATER or SWITCH (BED)  (D9)
#define Aux7                PH5     // DEW_HEATER or SWITCH (FAN0) (D8)
#define Aux8                PH4     // DEW_HEATER or SWITCH (FAN1) (D7)

#ifndef DS3234_CS_PIN
  #define DS3234_CS_PIN      A9     // Default CS Pin for DS3234 on SPI
#endif
#ifndef BME280_CS_PIN
  #define BME280_CS_PIN     A10     // Default CS Pin for BME280 on SPI
#endif
#ifndef OneWirePin
  #define OneWirePin        PC4     // Default Pin for 1-Wire bus Z-MIN
#endif
#define AddonBootModePin    PG0     // ESP8266 GPIO0 or SPI MISO/Fault
#define AddonResetPin       PD7     // ESP8266 RST or SPI MISO/Fault

// For software SPI
#define SSPI_SHARED
#define SSPI_SCK  52
#define SSPI_MISO 50
#define SSPI_MOSI 51

// The PEC index sense is a 5V logic input, resets the PEC index on rising edge then waits for 60 seconds before allowing another reset
#define PecPin              PC1     // X-MAX
#define AnalogPecPin        PC1     // A9

// The limit switch sense is a 5V logic input which uses the internal (or external 2k) pull up, shorted to ground it stops gotos/tracking
#define LimitPin            PC0     // X-MIN

// The status LED is a two wire jumper with a 10k resistor in series to limit the current to the LED
#define LEDnegPin          Aux0     // Built-in
#define LEDneg2Pin          PL7     // EXP2
#define ReticlePin          PE3     // EXP3

// Pin for a piezo buzzer output
#define TonePin             PG5     // EXP3

// The PPS pin is a 5V logic input, OnStep measures time between rising edges and adjusts the internal sidereal clock frequency
#define PpsPin              PD3     // TX1 pin on EXP3 don't use PPS if Serial1 is active

// Pins to Axis1 RA/Azm stepper driver on RUMBA X
#define Axis1_EN            PL1     // Enable
#define Axis1_M0             51     // SPI MOSI
#define Axis1_M0PORT      PORTB
#define Axis1_M0BIT           2
#define Axis1_M1             52     // SPI SCK
#define Axis1_M1PORT      PORTB
#define Axis1_M1BIT           1
#define Axis1_M2            PL3     // SPI CS
#define Axis1_M2PORT      PORTL
#define Axis1_M2BIT           3
#define Axis1_M3             50     // SPI MISO
#define Axis1_STEP          PH0     // Step
#define Axis1_StepPORT    PORTH
#define Axis1_StepBIT         0
#define Axis1_DIR           PH1     // Dir
#define Axis1_DirPORT     PORTH
#define Axis1_DirBIT          1
#define Axis1_DECAY    Axis1_M2     // Decay mode
#define Axis1_HOME          PC3     // Sense home position

// Pins to Axis2 Dec/Alt stepper driver on RUMBA Y
#define Axis2_EN            PF1     // Enable
#define Axis2_M0             51     // SPI MOSI
#define Axis2_M0PORT      PORTB
#define Axis2_M0BIT           2
#define Axis2_M1             52     // SPI SCK
#define Axis2_M1PORT      PORTB
#define Axis2_M1BIT           1
#define Axis2_M2            PB0     // SPI CS
#define Axis2_M2PORT      PORTB
#define Axis2_M2BIT           0
#define Axis2_M3             50     // SPI MISO
#define Axis2_STEP          PF0     // Step
#define Axis2_StepPORT    PORTF
#define Axis2_StepBIT         0
#define Axis2_DIR           PL2     // Dir
#define Axis2_DirPORT     PORTL
#define Axis2_DirBIT          2
#define Axis2_DECAY    Axis2_M2     // Decay mode
#define Axis2_HOME          PC2     // Sense home position

// For rotator stepper driver on RUMBA Z
#define Axis3_EN            PK0     // Enable
#define Axis3_M0             51     // SPI MOSI
#define Axis3_M1             52     // SPI SCK
#define Axis3_M2            PL0     // SPI CS
#define Axis3_M3             50     // SPI MISO
#define Axis3_STEP          PF3     // Step
#define Axis3_DIR           PF2     // Dir

// For focuser1 stepper driver on RUMBA E0
#define Axis4_EN            PA2     // Enable
#define Axis4_M0             51     // SPI MOSI
#define Axis4_M1             52     // SPI SCK
#define Axis4_M2            PB6     // SPI CS
#define Axis4_M3             50     // SPI MISO
#define Axis4_STEP          PA1     // Step
#define Axis4_DIR           PA0     // Dir

// For focuser2 stepper driver on RUMBA E1
#define Axis5_EN            PA5     // Enable
#define Axis5_M0             51     // SPI MOSI
#define Axis5_M1             52     // SPI SCK
#define Axis5_M2            PB7     // SPI CS
#define Axis5_M3             50     // SPI MISO
#define Axis5_STEP          PA4     // Step
#define Axis5_DIR           PA3     // Dir

// ST4 interface on JTAG
#define ST4RAw              PF4     // ST4 RA- West
#define ST4DEs              PF6     // ST4 DE- South
#define ST4DEn              PF5     // ST4 DE+ North
#define ST4RAe              PF7     // ST4 RA+ East

#else
#error "Wrong processor for this configuration!"

#endif
