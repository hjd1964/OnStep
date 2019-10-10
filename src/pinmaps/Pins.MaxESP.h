// -------------------------------------------------------------------------------------------------
// Pin map for OnStep MaxESP Version 1.x (ESP32S)

#if defined(ESP32)

// The multi-purpose pins (Aux3..Aux8 can be analog pwm/dac if supported)
#define Aux0                 25     // Status LED
#define Aux3                 5      // Home sw for Axis1 
#define Aux4                 14     // Home sw for Axis2
#define Aux8                 23     // LED2 or Reticle

// Misc. pins
#ifndef OneWirePin
  #define OneWirePin       Aux8     // Default Pin for one wire bus
#endif

// The PEC index sense is a logic level input, resets the PEC index on rising edge then waits for 60 seconds before allowing another reset
#define PecPin               36
#define AnalogPecPin         A0     // A0 or GPIO36 (PEC Sense, analog or digital)

// The status LED is a two wire jumper with a 10k resistor in series to limit the current to the LED
#define LEDnegPin          Aux0     // Drain
#define LEDneg2Pin         Aux8     // Drain
#define ReticlePin         Aux8     // Drain

// For a piezo buzzer
#define TonePin              13     // Tone

// The PPS pin is a 3.3V logic input, OnStep measures time between rising edges and adjusts the internal sidereal clock frequency
#define PpsPin             Aux0     // PPS time source, GPS for example

#define LimitPin             39     // The limit switch sense is a logic level input normally pull high (2k resistor,) shorted to ground it stops gotos/tracking

// Axis1 RA/Azm step/dir driver
#define Axis1_EN              4     // Enable
#define Axis1StepPin         18     // Step
#define Axis1DirPin          19     // Dir

// Axis2 Dec/Alt step/dir driver
#define Axis2_EN             12     // Enable
#define Axis2StepPin         27     // Step
#define Axis2DirPin          26     // Dir

// For rotator stepper driver
#define Axis3_EN              0     // Enable
#define Axis3StepPin          2     // Step
#define Axis3DirPin          15     // Dir

// For focuser1 stepper driver
#define Axis4_EN              0     // Enable
#define Axis4StepPin          2     // Step
#define Axis4DirPin          15     // Dir

// For focuser2 stepper driver
#define Axis4_EN             -1     // Enable
#define Axis4StepPin         -1     // Step
#define Axis4DirPin          -1     // Dir

// ST4 interface
#define ST4RAw               34     // ST4 RA- West
#define ST4DEs               32     // ST4 DE- South
#define ST4DEn               33     // ST4 DE+ North
#define ST4RAe               35     // ST4 RA+ East

#else
#error "Wrong processor for this configuration!"

#endif
