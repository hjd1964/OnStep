# OnStepESPServer

This is the ESP8266 webserver for OnStep.  It provides WiFi (IP) access to connect to OnStep via command channel and/or website.  This works with Sky Safari (tablets/phones,) my OnStep Android App, and my ASCOM driver (PC.)

For an ESP8266 ESP-01 module to be flashed with this firmware, it needs its pins in a certain configuration:
Pin CH_PD/EN must be HIGH (connected to 3.3V)
Pin GPIO0 must be LOW (connected to GND).

Here is an example of how it is wired:

Using a USB to TTL adapter
https://www.diyhobi.com/flash-program-esp-01-using-usb-serial-adapter/

Other methods
https://www.iot-experiments.com/flashing-esp8266-esp01/

For a WeMos D1 mini or NodeMCU simply plug in via the USB interface.
To send the firmware I use the Arduino IDE with the add-on for the ESP8266.  To get this setup...

Look in the Arduino Preferences menu and place this line in the additional "Boards Manager" area:
http://arduino.esp8266.com/stable/package_esp8266com_index.json

Then from the "Tools->Boards->Boards Manager" menu select and install the ESP8266 add-on.

Then pick "Tools->Boards->Generic 8266 Module" (for an ESP-01) or select your device from the list.

If you are using an ESP-01, then use the following parameters under Tools:

- Flash Mode: “DIO”
- Flash Frequency: “40MHz”
- CPU Frequency: “80 MHz”
- Flash Size: “1M (64K SPIFFS)”
- Reset Method: “ck”
- Upload Speed: “115200”
- Debug Port: “Disabled”
- Debug Level: “None”

This source code is inside your OnStep folder under the "src/WiFi-Bluetooth" subdirectory.

Open the WiFi-Bluetooth.ino file inside this folder.  Check the "Config.h" file for information about serial port wiring and to configure before uploading.  Once this is done upload to the ESP8266.

Once the firmware is on the module simply connect the serial interface lines to OnStep's Serial1 port similar to how a Bluetooth module would be connected.

These modules require more power than the 3.3v regulator on a Mega2560 can supply so the WeMos or NodeMCU with on-board regulator can be handy.  The Teensy3.2's 3.3v regulator will run an ESP-01.  This should work with OnStep on any Platform (Mega2560, Teensy3.2, and Launchpad TM4C.)  OnStep running on the Teensy3.2 is well tested with it.

Note that the WiFi server on the ESP does NOT start until it is connected to a running OnStep via Serial1.

You can replace DEBUG_OFF in Config.h with DEBUG_ON to change this behavior, and allow for resetting the configuration on the ESP.

