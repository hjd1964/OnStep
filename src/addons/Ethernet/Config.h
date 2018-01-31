// -------------------------------------------------------------------------------
// Configuration

#define DEBUG_OFF                    // Turn ON to allow Ethernet startup without OnStep attached (Serial port for debug at 9600 baud)
#define WEBSERVER_DEBUG_OFF          // Turn ON to allow webserver debug messages

#define PASSWORD "password"          // Not currently used

#define Ser Serial1                  // This is the hardware serial port where OnStep is attached
#define SERIAL_BAUD_DEFAULT 9600     // Default=9600, OnStep must be attached and running it's Serial1 interface this baud rate

// At startup this firmware will attempt to switch OnStep to the baud rate below and AFTER success start Ethernet, etc.
#define SERIAL_BAUD ":SB1#"          // 0=115.2K, 1=57.6K, 2=38.4K Baud, 3=28.8K, 4=19.2K Baud ... Default ":SB1#", if talking to a Mega2560 ":SB4#"

#define TIMEOUT_WEB 15               // Default=15, if talking to a Mega2560 use 60
#define TIMEOUT_CMD 30               // Default=30, if talking to a Mega2560 use 60

// enter a MAC address for your controller below:
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

// the IP addresses below will be dependent on your local network:
IPAddress ip(192, 168, 1, 55);
IPAddress myDns(192,168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

// -------------------------------------------------------------------------------

