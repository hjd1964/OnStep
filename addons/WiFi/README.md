# OnStep WiFi Server
This is the ESP8266 webserver for OnStep.
It enables OnStep to be controlled over WiFi (IP). This access can be over a command channel
and/or website.
This server is known to work from personal computers (using a browser), phones and tablets,
(using the Android OnStep App), Sky Safari, and my ASCOM driver (PC.)

# Installing the ESP8266 Platform
Before you can flash the firmware on your ESP8266 device, you need to install the ESP8266
platform for the Arduino IDE.

Under Preferences add this line in the additional "Boards Manager" area:
  http://arduino.esp8266.com/stable/package_esp8266com_index.json

Then from the "Tools, Boards, Boards Manager" menu select and install the ESP8266 add-on.

# Flashing The WiFi Server
The exact flashing procedure depends on which ESP8826 device you will be using.

In the Arduino IDE, pick your device from "Tools, Boards". 

For a WeMos D1 Mini or NodeMCU, select the device from the list.
For an ESP-01, select "Generic 8266 Module".

For devices with a USB port (WeMos D1 Mini, NodeMCU), you can connect directly using the USB connector.
For an ESP-01 module, its pins should be in a certain configuration:

Pin CHPD/EN must be HIGH (connected to 3.3V)
Pin GPIO0 must be LOW (connected to GND).

You can buy an inexpensive adapter or board that help with flashing the ESP01 from eBay, Amazon and other sources.

For the ESP-01, use the following parameters under Tools:

- Board: "Generic ESP8266 Module"
- Flash Mode: "DIO"
- Erase Flash: "Sketch + WiFi Settings"
- Flash Size: "1M (64K SPIFFS)" (Most ESP01 have 1M, older ones have 512K)
- Flash Frequency: "40MHz"
- CPU Frequency: "80 MHz"
- Reset Method: "ck"
- Upload Speed: "115200"
- Debug Port: "Disabled"
- Debug Level: "None"

The source code is inside the OnStep folder under the "addons/WiFi-Bluetooth" subdirectory.

Open the WiFi-Bluetooth.ino file inside this folder. Check the "Config.h" file for information
about serial port wiring and to configure before uploading.  Once this is done upload to the ESP8266.

# Wiring
If you are using a pre-wired board (e.g. MiniPCB, MaxPCB), you don't need to worry about wiring, since
it is all done for you.

If you are not using a pre-wired board, you have to connect your ESP8266 to certain pins for your
microcontroller.  Basically, the TX on the ESP8266 should be connected on Serial1's RX for the
microcontroller, and RX on the ESP8266 should be connected to TX on Serial1 for the microcontroller.

Note that Serial1 may be remapped in the HAL for your microcontroller, for example on STM32, PA10/TX
and PA11/RX are used, and they are designated Serial3. If you are using a HAL that is tested and
verified working, you don't need to worry about this part. 

You also need to make the CHPD/EN pin high by connecting it to 3.3V, otherwise, your ESP-01 would
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

