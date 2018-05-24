# OnStep WiFi Server

This is the ESP8266 webserver for OnStep.
It enables OnStep to be controlled over WiFi (IP). This access can be over a command channel
and/or website.
This server is known to work from personal computers (using a browser), phones and tablets,
(using the Android OnStep App), Sky Safari, and my ASCOM driver (PC.)

# Flashing The WiFi Server
For an ESP8266 ESP-01 module to be flashed with this firmware, it needs its pins in a certain
configuration:

Pin CH_PD/EN must be HIGH (connected to 3.3V)
Pin GPIO0 must be LOW (connected to GND).

For the ESP-01 module, you can by adapter boards that help with flashing from eBay, Amazon
and other sources.

For a WeMos D1 mini or NodeMCU simply plug in via the USB interface.
To send the firmware I use the Arduino IDE with the add-on for the ESP8266, as follows:

Under Preferences add this line in the additional "Boards Manager" area:
http://arduino.esp8266.com/stable/package_esp8266com_index.json

Then from the "Tools, Boards, Boards Manager" menu select and install the ESP8266 add-on.

Then pick "Tools, Boards, Generic 8266 Module" (for an ESP-01) or select your device from the list.

If you are using an ESP-01, then use the following parameters under Tools:

- Flash Mode: “DIO”
- Flash Frequency: “40MHz”
- CPU Frequency: “80 MHz”
- Flash Size: “512K (64K SPIFFS)”
- Reset Method: “ck”
- Upload Speed: “115200”
- Debug Port: “Disabled”
- Debug Level: “None”

This source code is inside your OnStep folder under the "addons/WiFi-Bluetooth" subdirectory.

Open the WiFi-Bluetooth.ino file inside this folder. Check the "Config.h" file for information
about serial port wiring and to configure before uploading.  Once this is done upload to the ESP8266.

# Wiring
If you are using a pre-wired board (e.g. MiniPCB, MaxPCB), you don't need to worry about wiring, since
it is all done for you.

If you are not using a pre-wired board, you to connect your ESP8266 to certain pins for your
microcontroller.  Basically, the TX on the ESP8266 should be connected on Serial1's RX for the
microcontroller, and RX on the ESP8266 should be connected to TX on Serial1 for the microcontroller.

Note that Serial1 may be remapped in the HAL for your microcontroller, for example on STM32, PA3 and 
PA2 are used, and they are designated Serial2. If you are using a HAL that is tested and verified
working, you don't need to worry about this part. 

You also need to make the CH_PD/EN pin high by connecting it to 3.3V, otherwise, your ESP-01 would
not turn on. 

# Power
These modules require more power than the 3.3v regulator on a Mega2560 can supply so the WeMos or
NodeMCU with on-board regulator can be handy.  The Teensy3.2's 3.3v regulator will run an ESP-01.
This should work with OnStep on any Platform (Mega2560, Teensy3.2, and Launchpad TM4C.)  OnStep running
on the Teensy3.2 is well tested with it.

# Starting Up
The WiFi server will only start WiFi if it was able to connect to OnStep via Serial1 and find it working.
Otherwise, you will not see the "ONSTEP" SSID.

You can replace DEBUG_OFF in Config.h with DEBUG_ON to change this behavior, and allow for resetting
the configuration on the ESP.

