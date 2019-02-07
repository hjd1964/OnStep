// -------------------------------------------------------------------------------------------------
// Pin map for OnStep MaxESP (ESP32) Version 2.x

#if defined(ESP32)

// The multi-purpose pins (Aux3..Aux8 can be analog pwm/dac if supported)
#define Aux3          21    // Home SW for Axis1 (or I2C SDA)
#define Aux4          22    // Home SW for Axis2 (or I2C SCL)
//#define Aux5          "V0"  // Virtual Aux Pin0 (usually maps to AXIS1_ENC_A_PIN)
//#define Aux6          "V1"  // Virtual Aux Pin1 (usually maps to AXIS1_ENC_B_PIN)
#define Aux7          39    // Limit SW, Status LED2, Reticule, etc.
#define Aux8          25    // Status LED, PPS, Tone, etc.
//#define Aux9          "V2"  // Virtual Aux Pin2 (usually maps to AXIS2_ENC_A_PIN)
//#define Aux10         "V3"  // Virtual Aux Pin3 (usually maps to AXIS2_ENC_B_PIN)

// these enable virtual pin support
/*
int vpin_array[8] = {-1,-1,-1,-1,-1,-1,-1,-1};
void pinMode(const char* pin, byte mode) {}
void digitalWrite(const char* pin, int state) { int p=pin[1]-'0'; if ((pin[0]=='V') && (pin[2]==0) && (p>=0) && (p<=7)) { vpin_array[p]=state; } }
void analogWrite(const char* pin, int state) { int p=pin[1]-'0'; if ((pin[0]=='V') && (pin[2]==0) && (p>=0) && (p<=7)) { vpin_array[p]=state; } }
char* getVPins1to4() { static char s[20]; sprintf(s,"%d,%d,%d,%d",vpin_array[0],vpin_array[1],vpin_array[2],vpin_array[3]); return s; }
char* getVPins5to8() { static char s[20]; sprintf(s,"%d,%d,%d,%d",vpin_array[4],vpin_array[5],vpin_array[6],vpin_array[7]); return s; }
*/

// The PEC index sense is a logic level input, resets the PEC index on rising edge then waits for 60 seconds before allowing another reset
#define PecPin        36
#define AnalogPecPin  A0    // A0 or GPIO36 (PEC Sense, analog or digital)

// The status LED is a two wire jumper with a 10k resistor in series to limit the current to the LED
#define LEDnegPin     Aux8  // Pin 25 (Drain)
#define LEDneg2Pin    Aux8  // Pin 39 (Drain)
#define ReticulePin   Aux8  // Pin 39 (Drain)

// For a piezo buzzer
#define TonePin       Aux8  // Pin 25 (Tone)

// The PPS pin is a 3.3V logic input, OnStep measures time between rising edges and adjusts the internal sidereal clock frequency
#define PpsPin        Aux8  // Pin 25 (PPS time source, GPS for example)

#define LimitPin      Aux7  // Pin 39 (The limit switch sense is a logic level input normally pull high (2k resistor,) shorted to ground it stops gotos/tracking)

#define Axis1_HOME    Aux3  // Pin 36 (Home sw)
#define Axis1DirPin   19    // Pin 19 (Dir)
#define Axis1StepPin  18    // Pin 18 (Step)
#define Axis1_Aux     12    // Pin 12 (ESP8266 GPIO0 or SPI MISO)
#define Axis1_M2      23    // Pin 23 (Microstep Mode 2 or SPI CS)
#define Axis1_M1      14    // Pin 14 (Microstep Mode 1 or SPI SCK)
#define Axis1_M0      13    // Pin 13 (Microstep Mode 0 or SPI MOSI)
#define Axis1_EN       4    // Pin  4 (Enable)

#define Axis2_HOME    Aux4  // Pin 39 (Home sw)
#define Axis2DirPin   26    // Pin 26 (Dir)
#define Axis2StepPin  27    // Pin 27 (Step)
#define Axis2_Aux     12    // Pin 12 (ESP8266 RST or SPI MISO)
#define Axis2_M2      5     // Pin 5  (Microstep Mode 2 or SPI CS)
#define Axis2_M1      14    // Pin 14 (Microstep Mode 1 or SPI SCK)
#define Axis2_M0      13    // Pin 13 (Microstep Mode 0 or SPI MOSI)
#define Axis2_EN      4     // Pin 4  (Enable)

// For rotator stepper driver
#define Axis3DirPin   15    // Pin 15 (Dir)
#define Axis3StepPin  2     // Pin 2  (Step)
#define Axis3_EN      0     // Pin 0  (Enable)

// For focuser1 stepper driver
#define Axis4DirPin   15    // Pin 15 (Dir)
#define Axis4StepPin  2     // Pin 2  (Step)
#define Axis4_EN      0     // Pin 0  (Enable)

// For focuser2 stepper driver
#define Axis5DirPin   15    // Pin 15 (Dir)
#define Axis5StepPin  2     // Pin 2  (Step)
#define Axis5_EN      0     // Pin 0  (Enable)

// ST4 interface
#define ST4RAw        34    // Pin 34 ST4 RA- West
#define ST4DEs        32    // Pin 32 ST4 DE- South
#define ST4DEn        33    // Pin 33 ST4 DE+ North
#define ST4RAe        35    // Pin 35 ST4 RA+ East

#else
#error "Wrong processor for this configuration!"

#endif
