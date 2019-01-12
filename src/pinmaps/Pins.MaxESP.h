// -------------------------------------------------------------------------------------------------
// Pin map for OnStep MaxESP (ESP32) Version 1.x

#if defined(ESP32)

// The multi-purpose pins (Aux3..Aux8 can be analog pwm/dac if supported)
#define Aux0          25    // Status LED
#define Aux3          5     // Home sw for Axis1 
#define Aux4          14    // Home sw for Axis2
#define Aux5          0     // EN for Axis3

// The PEC index sense is a logic level input, resets the PEC index on rising edge then waits for 60 seconds before allowing another reset
#define PecPin        36
#define AnalogPecPin  A0    // A0 or GPIO36 (PEC Sense, analog or digital)

// The status LED is a two wire jumper with a 10k resistor in series to limit the current to the LED
#define LEDnegPin     Aux0  // Pin 25 (Drain)
#define LEDneg2Pin    23    // Pin 23 (Drain)
#define ReticulePin   23    // Pin 23 (Drain)

// For a piezo buzzer
#define TonePin       13    // Pin 13 (Tone)

// The PPS pin is a 3.3V logic input, OnStep measures time between rising edges and adjusts the internal sidereal clock frequency
#define PpsPin        Aux0  // Pin 25 (PPS time source, GPS for example)

#define LimitPin      39    // Pin 39 (The limit switch sense is a logic level input normally pull high (2k resistor,) shorted to ground it stops gotos/tracking)

#define Axis1DirPin   19    // Pin 19 (Dir)
#define Axis1StepPin  18    // Pin 18 (Step)
#define Axis1_HOME    Aux3  // Pin 5  (Home sw)
#define Axis1_EN      4     // Pin 4  (Enable)

#define Axis2DirPin   26    // Pin 27 (Dir)
#define Axis2StepPin  27    // Pin 26 (Step)
#define Axis2_HOME    Aux4  // Pin 14 (Home sw)
#define Axis2_EN      12    // Pin 12 (Enable)

// For rotator stepper driver
#define Axis3DirPin   15    // Pin 15 (Dir)
#define Axis3StepPin  2     // Pin 2  (Step)
#define Axis3_EN      Aux5  // Pin 0  (Enable)

// For focuser1 stepper driver
#define Axis4DirPin   15    // Pin 15 (Dir)
#define Axis4StepPin  2     // Pin 2  (Step)
#define Axis4_EN      Aux5  // Pin 0  (Enable)

// ST4 interface
#define ST4RAw        34    // Pin 34 ST4 RA- West
#define ST4DEs        32    // Pin 32 ST4 DE- South
#define ST4DEn        33    // Pin 33 ST4 DE+ North
#define ST4RAe        35    // Pin 35 ST4 RA+ East

#else
#error "Wrong processor for this configuration!"

#endif
