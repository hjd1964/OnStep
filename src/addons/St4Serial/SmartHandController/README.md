# OnStepESPServer

This is the ESP8266 webserver for OnStep.  It provides WiFi (IP) access to connect to OnStep via command channel and/or website.  This works with Sky Safari (tablets/phones,) my OnStep Android App, and my ASCOM driver (PC.)

For an ESP8266 ESP-01 module use the wiring as shown in numerous places on the internet to get the module in firmware upload mode.
For a WeMos D1 mini or NodeMCU simply plug in via the USB interface.
To send the firmware I use the Arduino IDE with the add-on for the ESP8266.  To get this setup...

Look in the Arduino Preferences menu and place this line in the additional "Boards Manager" area:
http://arduino.esp8266.com/staging/package_esp8266com_index.json
Then from the "Tools->Boards->Boards Manager" menu select the ESP8266 add-on.

Then pick "Tools->Boards->Generic 8266 Module" (for an ESP-01) or select your device from the list.

Download the OnStepESPServer.zip file from GitHub.

Unzip the folder inside and rename it to "OnStepESPServer". This can be on your desktop or in your ~/Documents/Arduino folder.

Open the OnStepESPServer.ino file inside this folder.  Check the "Config.h" file for information about serial port wiring and to configure before uploading.  Once this is done upload to the ESP8266.

Once the firmware is on the module simply connect the serial interface lines to OnStep's Serial1 port similar to how a Bluetooth module would be connected.

These modules require more power than the 3.3v regulator on a Mega2560 can supply so the WeMos or NodeMCU with on-board regulator can be handy.  The Teensy3.2's 3.3v regulator will run an ESP-01.  This should work with OnStep on any Platform (Mega2560, Teensy3.2, and Launchpad TM4C.)  OnStep running on the Teensy3.2 is well tested with it.

