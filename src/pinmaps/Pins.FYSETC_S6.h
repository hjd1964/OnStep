// -------------------------------------------------------------------------------------------------
// Pin map for FYSETC S6

#if defined(STM32F446xx)

// TX2/RX2 (PA2/PA3) is on the Y+ and Z+ end stops and is reserved for GPS (etc, no command channel is associated with this port)
// Use "#define SerialGPS SoftwareSerial2" in Config.h for the GPS only serial port

// Temperature sensors (From Marlin) perhaps these would make better Limit sw and 2x home switches, and PEC.
// They have built-in pullups and a 10uf cap for ESD but they are not labeled as such so leaving it be.
#define Temp0Pin            PC0     // Temp0   (on TE0, THERMO0)
#define Temp1Pin            PC1     // Temp1   (on TE1, THERMO1)
#define Temp2Pin            PC2     // Temp2   (on TE2, THERMO2)
#define Temp3Pin            PC3     // Temp3   (on TB , THERMO3)

// Fans (From Marlin) we use for Auxiliary Features (switches etc.)  Probably with a little crafty wiring these can be 3V3 or 5V.
#define Fan0Pin             PB0     // Fan0    (on FAN0)
#define Fan1Pin             PB1     // Fan1    (on FAN1)
#define Fan2Pin             PB2     // Fan2    (on FAN2)

// Heaters (From Marlin) we use for Auxiliary Features (switches, dew heaters, etc.)
#define Heater0Pin          PB3     // Heater0 (on E0-OUT, E0-Heater)
#define Heater1Pin          PB4     // Heater1 (on E1-OUT, E1-Heater)
#define Heater2Pin         PB15     // Heater2 (on E2-OUT, E2-Heater)
#define Heater3Pin          PC8     // Heater3 (on BED-OUT, Heated-Bed)

// The multi-purpose pins (Aux3..Aux8 can be analog pwm/dac if supported)
// I defined 7 Aux pins so they match up with the first 7 Auxiliary Feature slots avaliable in OnStep
// Aux1-3 can be used for pretty much anything
// Aux4-7 are more for dew-heaters
#define Aux1            Fan0Pin
#define Aux2            Fan1Pin
#define Aux3            Fan2Pin
#define Aux4         Heater0Pin
#define Aux5         Heater1Pin
#define Aux6         Heater2Pin
#define Aux7         Heater3Pin

// Misc. pins
#ifndef DS3234_CS_PIN
  #define DS3234_CS_PIN     PA4     // Default CS Pin for DS3234 on SPI (on EXP2 shared with the ESP8266 RST pin)
#endif
#ifndef BME280_CS_PIN
  #define BME280_CS_PIN     PC7     // Default CS Pin for BME280 on SPI (on EXP2 shared with LED2/Reticle)
#endif
#ifndef OneWirePin
  #define OneWirePin        PE0     // Default Pin for OneWire bus (on E2-MOT PD-EN, right hand pin)
#endif
#define ESP8266Gpio0Pin     PD0     // ESP8266 GPIO0 (on EXP1)
#define ESP8266RstPin       PA4     // ESP8266 RST (on EXP2 shared with the DS3234 CS pin)

// The PEC index sense is a logic level input, resets the PEC index on rising edge then waits for 60 seconds before allowing another reset
#define PecPin              PA1
#define AnalogPecPin        PA1     // PEC Sense, analog or digital (on X+ so it can have 3v3 or 5v on adjacent pin)

// The status LED is a two wire jumper with a 10k resistor in series to limit the current to the LED
#define LEDnegPin           PC6     // Drain (on EXP2) One could perhaps move these to the RGB leds, there's a header but no +5V present on it.
#define LEDneg2Pin          PC7     // Drain (on EXP2 shared with Reticle/BME280_CS)
#define ReticlePin          PC7     // Drain (on EXP2 shared with LED2/BME280_CS)

// For a piezo buzzer
#define TonePin             PC9     // Tone (on BEEP/EXP1)

// The PPS pin is a 3.3V logic input, OnStep measures time between rising edges and adjusts the internal sidereal clock frequency
#define PpsPin             PB10     // PPS time source, GPS for example (on EXP2)

#define LimitPin            PA0     // Limit switch sense (on Z-)

// Axis1 RA/Azm step/dir driver
#define Axis1_EN           PE12     // Enable
#define Axis1_M0            PA7     // Microstep Mode 0 or SPI MOSI
#define Axis1_M1            PA5     // Microstep Mode 1 or SPI SCK
#define Axis1_M2            PE7     // Microstep Mode 2 or SPI CS or Decay Mode
#define Axis1_M3            PA6     // SPI MISO/Fault
#define Axis1_STEP         PE11     // Step
#define Axis1_DIR          PE10     // Dir
#define Axis1_DECAY    Axis1_M2     // Decay mode
#define Axis1_FAULT    Axis1_M3     // SPI MISO/Fault
#define Axis1_HOME         PB14     // Sense home position Axis1 (on X-)

// Axis2 Dec/Alt step/dir driver
#define Axis2_EN            PD9     // Enable
#define Axis2_M0            PA7     // Microstep Mode 0 or SPI MOSI
#define Axis2_M1            PA5     // Microstep Mode 1 or SPI SCK
#define Axis2_M2           PE15     // Microstep Mode 2 or SPI CS or Decay Mode
#define Axis2_M3            PA6     // SPI MISO
#define Axis2_STEP          PD8     // Step
#define Axis2_DIR          PB12     // Dir
#define Axis2_DECAY    Axis2_M2     // Decay mode
#define Axis2_FAULT    Axis2_M3     // SPI MISO/Fault
#define Axis2_HOME         PB13     // Sense home position Axis2 (on Y-)

// For rotator stepper driver
#define Axis3_EN           PD15     // Enable
#define Axis3_M0            PA7     // SPI MOSI
#define Axis3_M1            PA5     // SPI SCK
#define Axis3_M2           PD10     // SPI CS or Decay Mode
#define Axis3_M3            PA6     // SPI MISO
#define Axis3_STEP         PD14     // Step
#define Axis3_DIR          PD13     // Dir

// For focuser1 stepper driver
#define Axis4_EN            PD4     // Enable
#define Axis4_M0            PA7     // SPI MOSI
#define Axis4_M1            PA5     // SPI SCK
#define Axis4_M2            PD7     // SPI CS or Decay Mode
#define Axis4_M3            PA6     // SPI MISO
#define Axis4_STEP          PD5     // Step
#define Axis4_DIR           PD6     // Dir

// For focuser2 stepper driver
#define Axis5_EN            PE5     // Enable
#define Axis5_M0            PA7     // SPI MOSI
#define Axis5_M1            PA5     // SPI SCK
#define Axis5_M2           PC14     // SPI CS or Decay Mode
#define Axis5_M3            PA6     // SPI MISO
#define Axis5_STEP          PE6     // Step
#define Axis5_DIR          PC13     // Dir

// ST4 interface
#define ST4RAw              PA8     // ST4 RA- West  (on EXP1)
#define ST4DEs              PD2     // ST4 DE- South (on EXP1)
#define ST4DEn             PC12     // ST4 DE+ North (on EXP1)
#define ST4RAe              PD1     // ST4 RA+ East  (on EXP1)

#else
#error "Wrong processor for this configuration!"

#endif
