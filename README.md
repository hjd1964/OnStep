OnStep
======
www.stellarjourney.com

Computerized telescope goto control based on Arduino driven stepper motors.  All common mount types are supported: German Equatorial, Fork (and variations), and AltAzm (Dobsonian, etc.)

Since I aquired an Losmandy G11 mount without any goto provision I decided to create my own solution. I was a bit reluctant to use a Gemini, mainly because of the cost, but also due to the reliablity since the servo motors are somewhat prone to failure. I also like to have complete control over the software/firmware so that no "show stopper" issues will ever be a problem for me.

OnStep was designed, from the beginning, as a more or less general purpose system and provisions were made in the firmware to allow for use on a variety of mounts.

OnStep supports a wide variety of connection options.  In addition to two serial ports, the Arduino Ethernet Shield is also supported.  One of the serial ports is normally devoted to a USB connection and the other to Bluetooth, though they can be used as serial connections to anything you like.  Other software in the OnStep ecosystem includes an ASCOM driver (with Ethernet and Serial support,) an Android App for Bluetooth equipped Phones/Tablets (version 2.3.3 or later,) a built-in website (Ethernet,) and a full planetarium program that controls all features (Sky Planetarium.)  OnStep speaks LX200 protocol, so Sky Safari, CdC (even without ASCOM), Stellarium, etc. all work too.
