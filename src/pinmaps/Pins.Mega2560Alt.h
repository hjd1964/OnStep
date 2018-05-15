// -------------------------------------------------------------------------------------------------
// Pin map for legacy OnStep "Alternate pin-map" (Mega2560)

#if defined(__AVR_ATmega2560__)

// The PEC index sense is a 5V logic input, resets the PEC index on rising edge then waits for 60 seconds before allowing another reset
#define PecPin         2
#define AnalogPecPin   1

// The limit switch sense is a 5V logic input which uses the internal pull up, shorted to ground it stops gotos/tracking
#define LimitPin       3

// The status LED is a two wire jumper with a 10k resistor in series to limit the current to the LED
#define LEDposPin      8    // Pin 8 (LED)
#define LEDnegPin      9    // Pin 9 (GND)
#define LEDneg2Pin     7    // Pin 7 (GND)
#define ReticulePin   44    // Pin 44 (GND)  

// For a piezo buzzer
#define TonePin       42    // Pin 42 (Tone)

// The PPS pin is a 5V logic input, OnStep measures time between rising edges and adjusts the internal sidereal clock frequency
#define PpsPin        21    // Interrupt 2 on Pin 21 (alternate Int3 on Pin20)
#define DS3234_CS_PIN 10
                         
// Pins to enable/disable the stepper drivers and set microstep mode, optional and normally just hard-wired (DRV8825)/ignored (BED-A4988)
#define Axis1DirPin   37    // Pin 37 (Dir)  PC0
#define Axis1DirBit    0    //
#define Axis1DirPORT  PORTC //
#define Axis1StepPin  35    // Pin 35 (Step) PC2
#define Axis1StepBit   2    //
#define Axis1StepPORT PORTC //

#define Axis1_M0      29    // Pin 29 (Microstep or Decay Mode 0)
#define Axis1_M1      27    // Pin 27 (Microstep or Decay Mode 1)
#define Axis1_M2      25    // Pin 25 (Microstep or Decay Mode 2)
#define Axis1_EN      23    // Pin 23 (Enable)
#define Axis1_FAULT   39    // Pin 39 (Fault)
#define Axis1_Aux     39    // Pin 39 (Aux - SPI MISO)
#define Axis1_Mode    41    // Pin 41 (Aux Decay Mode for Axis1)

// The HA(RA) and Dec jumpers (going to the big easy drivers) are simply four wire jumper cables, each has identical wiring - simple modular construction
#define Axis2DirPin   22    // Pin 22 (Dir)   PA0
#define Axis2DirBit    0    //
#define Axis2DirPORT  PORTA //
#define Axis2StepPin  24    // Pin 24 (Step)  PA2
#define Axis2StepBit   2    //
#define Axis2StepPORT PORTA //

#define Axis2_M0      30    // Pin 30 (Microstep or Decay Mode 0)
#define Axis2_M1      32    // Pin 32 (Microstep or Decay Mode 1)
#define Axis2_M2      34    // Pin 34 (Microstep or Decay Mode 2)
#define Axis2_EN      36    // Pin 36 (Enabled)
#define Axis2_FAULT   38    // Pin 38 (Fault)
#define Axis2_Aux     38    // Pin 38 (Aux - SPI MISO)
#define Axis2_Mode    40    // Pin 40 (Aux Decay Mode for Axis2)

// ST4 interface
#define ST4RAw        47    // Pin 47 ST4 RA- West
#define ST4DEs        43    // Pin 43 ST4 DE- South
#define ST4DEn        45    // Pin 45 ST4 DE+ North
#define ST4RAe        49    // Pin 49 ST4 RA+ East

#else
#error "Wrong processor for this configuration!"

#endif

