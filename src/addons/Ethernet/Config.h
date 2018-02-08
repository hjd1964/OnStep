// -------------------------------------------------------------------------------
// Configuration

// at startup this firmware will attempt to switch OnStep's baud rate to a faster speed and AFTER success, start WiFi, etc.
// valid baud rates are 115200, 57600, 38400, 28800, 19200, 9600

#define SERIAL_BAUD_DEFAULT 9600     // Default=9600, OnStep must be attached and running it's Serial1 interface this baud rate

#define SERIAL_BAUD 57600            // Default=57600, use 19200 for talking to OnStep on a Mega2560

#define PASSWORD "password"          // Default="password", Not currently used

// enter a MAC address for your controller below:
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

// the IP addresses below will be dependent on your local network:
IPAddress ip(192, 168, 1, 55);
IPAddress myDns(192,168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

// misc. options that are usually not changed
#define DEBUG_OFF                    // Turn _ON to allow Ethernet startup without OnStep attached (Serial port for debug at 9600 baud)
#define Ser Serial1                  // Default=Serial1, This is the hardware serial port where OnStep is attached

// -------------------------------------------------------------------------------

