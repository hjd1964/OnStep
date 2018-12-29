// ---------------------------------------------------------------------------------------
// Configuration

// at startup this firmware will attempt to switch OnStep's baud rate to a faster speed and AFTER success, configure WiFi, etc.
// if establishing a link to OnStep was ***unsuccessful*** the ESP8266 will retain prior settings including it's SSID (from factory defaults, for example)
// valid baud rates are 115200, 57600, 38400, 28800, 19200, 9600

#define SERIAL_BAUD_DEFAULT 9600  // Default=9600, OnStep must be attached and running it's Serial1 interface this baud rate

#define SERIAL_BAUD 57600         // Default=57600, use 19200 for talking to OnStep on a Mega2560

#define LED_PIN_OFF               // Default=_OFF, Flashes LED when trying to connect to OnStep, use LED_PIN D4 for WeMos D1 Mini

#define SERIAL_SWAP_OFF            // Default=_OFF, _ON to swap ESP8266 Serial default location GPIO 1(TX)/3(RX) to 15(TX)/13(RX) 
                                   // useful for WeMos D1 Mini where these are labeled D8(TXD2) and D7(RXD2), for example

#define SPECIAL_CHARS_ON          // Default=_ON, _OFF for standard ASCII special symbols (compatibility)
#define ADVANCED_CHARS_ON         // Default=_ON, _OFF for standard "RA/Dec" instead of symbols

#define AMBIENT_CONDITIONS_OFF    // Default=_OFF, _ON for ambient conditions on status page
#define INTERNAL_TEMPERATURE_OFF  // Default=_OFF, _ON for internal MCU temperature display

// --------------------------------------------------------------------------------------
// Encoder support (either _AB for Quadrature A/B OR _CWCCW for CW/CCW)
#define AXIS1_ENC_OFF              // RA/Azm Axis, default = _OFF
#define AXIS1_ENC_A_PIN 14         // specify pin#, for A or CW, default=_OFF
#define AXIS1_ENC_B_PIN 12         // specify pin#, for B or CCW, default=_OFF
#define AXIS1_ENC_REVERSE_OFF      // _ON to reverse the count direction, default=_OFF
#define AXIS1_ENC_TICKS_DEG (200000.0/360.0) // 0.0018 degrees
#define AXIS1_ENC_DIFF_LIMIT  450  // in arcseconds (0.125 degrees)

#define AXIS1_ENC_RATE_CONTROL_OFF // if a high resolution encoder is on Axis1 this enables RA rate control (EQ mounts)
#define AXIS1_ENC_RATE_AUTO_OFF    // adjusts the encoder rate to account for the average rate over the last worm period, this should smooth out the encoder display but not while OnStep rate control is active, default=_OFF
#define AXIS1_SEC_PER_WORM_ROT 600 // if encoder rate control is used this lets the system know how long the worm period is
#define AXIS1_ENC_INTPOL_COS_OFF   // enable cosine compensation feature, default=_OFF
#define AXIS1_ENC_BIN_AVG_ON       // enable binned rolling average compensation feature, default=_OFF
#define AXIS1_ENC_INTPOL_PERIOD 20 // period (in counts) for encoder interpolation

#define AXIS2_ENC_OFF              // Dec/Alt Axis, default = _OFF
#define AXIS2_ENC_A_PIN 5          // specify pin#, for A or CW, default=_OFF
#define AXIS2_ENC_B_PIN 4          // specify pin#, for B or CCW, default=_OFF
#define AXIS2_ENC_REVERSE_ON       // _ON to reverse the count direction, default=_OFF
#define AXIS2_ENC_TICKS_DEG (4800.0/360.0) // 0.075 degrees
#define AXIS2_ENC_DIFF_LIMIT  900  // in arcseconds (0.25 degrees)

#define ENCODERS_AUTO_SYNC_ON     // enable support for auto sync of OnStep to encoder values

// --------------------------------------------------------------------------------------
// Auxillary switch/feature control; for example: SW0 "Cover" to open/close an OTA cover
// only OnStep Aux pins that are unused for other purposes should be assigned!
#define SW0_OFF                   // For Aux0
#define SW1_OFF                   // For Aux1
#define SW2_OFF                   // For Aux2
#define SW3_OFF                   // For Aux3
#define SW4_OFF                   // For Aux4
#define SW5_OFF                   // For Aux5
#define SW6_OFF                   // For Aux6
#define SW7_OFF                   // For Aux7
#define SW8_OFF                   // For Aux8
#define SW9_OFF                   // For Aux9
#define SW10_OFF                  // For Aux10
#define SW11_OFF                  // For Aux11
#define SW12_OFF                  // For Aux12
#define SW13_OFF                  // For Aux13
#define SW14_OFF                  // For Aux14
#define SW15_OFF                  // For Aux15

// --------------------------------------------------------------------------------------
// Auxillary analog/feature control; for example: AN7 "Primary Heater" to allow pwm control of a dew heater at 0 to 100% power
// only Aux3..Aux8 have the ability to support pwm/dac operation in OnStep (see Pins.xxx.h to see which pins are actually available, if any.)
#define AN3_OFF                   // For Aux3
#define AN4_OFF                   // For Aux4
#define AN5_OFF                   // For Aux5
#define AN6_OFF                   // For Aux6
#define AN7_OFF                   // For Aux7
#define AN8_OFF                   // For Aux8

// --------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------
// misc. options that are usually not changed
#define DEBUG_OFF                 // Turn _ON to allow WiFi startup without OnStep attached (Serial port for debug at 115200 baud)
#define Ser Serial                // Default=Serial, This is the hardware serial port where OnStep is attached

// On first successful startup an AP will appear with an SSID of "ONSTEP", after connecting:
// The web-site is at "192.168.0.1" and the cmd channel is at "192.168.0.1:9999".
//
// If locked out of the ESP8266, a Sketch (like MultiSerial) uploaded to the OnStep MCU (Teensy3.2, Mega2560, etc.) can recover access.
// Simply sending an 'R' in reply to any '#' received will cause a reset to AP only enabled and the default SSID/Password.  Be sure to
// upload OnStep again if necessary and then restart both devices to resume operation.

// -------------------------------------------------------------------------------

