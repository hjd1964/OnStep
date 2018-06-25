// -------------------------------------------------------------------------------------------------
// Pin map for OnStep Launchpad TM4C varients

#if (defined(__TM4C123GH6PM__) || defined(__LM4F120H5QR__))
// Note that TM4C123 has resistors R9 and R10 between pins 14 (B6) and 23 (D0) and between pins 15 (B7) and 24 (D1)
// Make sure you look at the list of the pins and options to avoid clashes or desolder the two bridges
// These pins are used for Axis2_M2 (14) and DE_M3 (15) and for Axis2DirPin (23) and Axis25vPin (24)
// If you have defined AXIS2_MODE_OFF in Config.h you should be safe to leave things as they are.

// Also note that we are using UART1 and UART 5 which use pins 3-6

// The PEC index sense is a 5V logic input, resets the PEC index on rising edge then waits for 60 seconds before allowing another reset
#define PecPin        11               // Pin A2

// The limit switch sense is a 5V logic input which uses the internal (or external 2k) pull up, shorted to ground it stops gotos/tracking
#define LimitPin      12               // Pin A3

// The status LED is a two wire jumper with a 10k resistor in series to limit the current to the LED
#define LEDposPin      2               // Pin B5 (LED)
#define LEDnegPin     33               // Pin D6 (GND)
#define LEDneg2Pin    26               // Pin D3 (GND)
#define ReticulePin   33

// For a piezo buzzer
#define TonePin       19               // Pin B5

// The PPS pin is a 5V logic input, OnStep measures time between rising edges and adjusts the internal sidereal clock frequency
#define PpsPin        19               // Pin B2 (PPS time source, GPS for example)

#define Axis1DirPin   27               // Pin E1 (Dir)
#define Axis1DirBit   GPIO_PIN_1       // Pin 27
#define Axis1DirPORT  GPIO_PORTE_BASE  //
#define Axis15vPin    28               // Pin E2 (5V?)
#define Axis1StepPin  29               // Pin E3 (Step)
#define Axis1StepBit  GPIO_PIN_3       // Pin 29
#define Axis1StepPORT GPIO_PORTE_BASE  //
// Pin GND (GND)

// Pins to enable/disable the stepper drivers and set microstep mode, optional and normally just hard-wired (DRV8825)/ignored (BED-A4988)
#define Axis1_M0      34               // Pin C7 (Microstep Mode 0)
#define Axis1_M1      35               // Pin C6 (Microstep Mode 1)
#define Axis1_M2      36               // Pin C5 (Microstep Mode 2)
#define Axis1_EN      37               // Pin C4 (Enable)
#define Axis1_FAULT   38               // Pin B3 (Fault)

// The HA(RA) and Dec jumpers (going to the big easy drivers) are simply four wire jumper cables, each has identical wiring - simple modular construction
#define Axis2DirPin   23               // Pin D0 (Dir)
#define Axis2DirBit   GPIO_PIN_0       // Pin 23=D0=B6 = pin 14
#define Axis2DirPORT  GPIO_PORTD_BASE  //
#define Axis25vPin    24               // Pin D1 (5V?)
#define Axis2StepPin  25               // Pin D2 (Step)
#define Axis2StepBit  GPIO_PIN_2       // Pin 25
#define Axis2StepPORT GPIO_PORTD_BASE  //
#define Axis2GndPin   26               // Pin D3 (GND)

#define Axis2_M0      13               // Pin A4 (Microstep Mode 0)
#define Axis2_M1      14               // Pin B6 (Microstep Mode 1) IF USED MAKE SURE YOU DESOLDER A BRIDGE or change pins around, otherwise pin 14 is connected to pin 23
#define Axis2_M2      15               // Pin B7 (Microstep Mode 2) IF USED MAKE SURE YOU DESOLDER A BRIDGE or change pins around, otherwise pin 15 is connected to pin 24
#define Axis2_EN      18               // Pin E0 (Enable)
#define Axis2_FAULT   17               // Pin F0 (Fault) NOTE, this is connected to pushbutton switch 2

// Pins to rotator stepper driver
#define Axis3DirPin   38               // Pin 38 (Dir)
#define Axis3StepPin  17               // Pin 17 (Step)

// For focuser1 stepper driver
#define Axis4DirPin   38               // Pin 38 (Dir)
#define Axis4StepPin  17               // Pin 17 (Step)

// ST4 interface
#define ST4RAw         7               // Pin B4 ST4 RA- West
#define ST4DEs         8               // Pin A5 ST4 DE- South
#define ST4DEn         9               // Pin A6 ST4 DE+ North
#define ST4RAe        10               // Pin A7 ST4 RA+ East

#elif defined(__TM4C1294NCPDT__) || defined(__TM4C1294XNCZAD__)
// No need to desolder anything on this launchpad as pins we are using are not bridged

// Note that we are using UART7 and UART 5 which use pins 3-5 and pin 8 (C4,C5,C6,C7)

// The PEC index sense is a 5V logic input, resets the PEC index on rising edge then waits for 60 seconds before allowing another reset
#define PecPin        11               // Pin P2

// The limit switch sense is a 5V logic input which uses the internal pull up, shorted to ground it stops gotos/tracking
#define LimitPin      12               // Pin N3

// The status LED is a two wire jumper with a 10k resistor in series to limit the current to the LED
#define LEDposPin      2               // Pin E4 (LED)
#define LEDnegPin     33               // Pin L1 (GND)
#define LEDneg2Pin    26               // Pin E3 (GND)
#define ReticulePin   33

// For a piezo buzzer
#define TonePin       19               // Pin M3

// The PPS pin is a 5V logic input, OnStep measures time between rising edges and adjusts the internal sidereal clock frequency
#define PpsPin        19               // Pin M3 (PPS time source, GPS for example)

#define Axis1DirPin   27               // Pin D7 (Dir)
#define Axis1DirBit   GPIO_PIN_7       // Pin 27
#define Axis1DirPORT  GPIO_PORTD_BASE  //
#define Axis15vPin    28               // Pin A6 (5V?)
#define Axis1StepPin  29               // Pin M4 (Step)
#define Axis1StepBit  GPIO_PIN_4       // Pin 29
#define Axis1StepPORT GPIO_PORTM_BASE  //
// Pin GND (GND)

// Pins to enable/disable the stepper drivers and set microstep mode, optional and normally just hard-wired (DRV8825)/ignored (BED-A4988)
#define Axis1_M0      34               // Pin L0 (Microstep Mode 0)
#define Axis1_M1      35               // Pin L5 (Microstep Mode 1)
#define Axis1_M2      36               // Pin L4 (Microstep Mode 2)
#define Axis1_EN      37               // Pin G0 (Enabled when LOW)
#define Axis1_FAULT   38               // Pin F3 (Fault if LOW)

// The HA(RA) and Dec jumpers (going to the big easy drivers) are simply four wire jumper cables, each has identical wiring - simple modular construction
#define Axis2DirPin   23               // Pin E0 (Dir)
#define Axis2DirBit   GPIO_PIN_0       // Pin 23
#define Axis2DirPORT  GPIO_PORTE_BASE  //
#define Axis25vPin    24               // Pin E1 (5V?)
#define Axis2StepPin  25               // Pin E2 (Step)
#define Axis2StepBit  GPIO_PIN_2       // Pin 25
#define Axis2StepPORT GPIO_PORTE_BASE  //
#define Axis2GndPin   26               // Pin E3 (GND)

#define Axis2_M0      13               // Pin N2 (Microstep Mode 0)
#define Axis2_M1      14               // Pin D0 (Microstep Mode 1)
#define Axis2_M2      15               // Pin D1 (Microstep Mode 2)
#define Axis2_EN      18               // Pin H2 (Enable)
#define Axis2_FAULT   17               // Pin H3 (Fault) NOTE, this is connected to pushbutton switch 2

// For rotator stepper driver
#define Axis3DirPin   38               // Pin 38 (Dir)
#define Axis3StepPin  17               // Pin 17 (Step)

// For focuser1 stepper driver
#define Axis4DirPin   38               // Pin 38 (Dir)
#define Axis4StepPin  17               // Pin 17 (Step)

// ST4 interface
#define ST4RAw         7               // Pin D3 ST4 RA- West
#define ST4DEs         6               // Pin C7 ST4 DE- South
#define ST4DEn         9               // Pin B2 ST4 DE+ North
#define ST4RAe        10               // Pin B3 ST4 RA+ East

#else
#error "Wrong processor for this configuration!"

#endif

