OnStep Telescope Controller
===========================
# Important Note
THIS IS THE BETA BRANCH, IT'S FAIRLY WELL TESTED BUT STILL FREQUENTLY GETS UPDATED WITH NEW FEATURES.

THE BETA BRANCH IS RECOMMENDED FOR MOST USERS.

# What is OnStep?
OnStep is a computerized telescope goto controller, based on Teensy or
Arduino control of stepper motors.

It supports Equatorial Mounts (GEM, Fork, etc.) as well as Alt-Az mounts
(including Dobsonians, and the like.)

OnStep was designed, from the beginning, as a more or less general purpose
system and provisions were made in the firmware to allow for use on a variety
of mounts.

# Features
OnStep supports a wide variety of connection options.  Either two or three serial
"command channels" can be utilized. One of the these is normally devoted to a USB
connection and for the other(s) choose from the following:
* Bluetooth
* ESP8266 WiFi
* Arduino M0/Ethernet Shield
* Even another USB port or RS232 serial isn't very difficult to add.

Other software in the OnStep ecosystem include:

* an ASCOM driver (with IP and Serial support),
* an Android App useable over WiFi or Bluetooth equipped Phones/Tablets
  (version 2.3.3 or later),
* a "built-in" website (on the Ethernet and/or WiFi device),
* a full planetarium program that controls all features (Sky Planetarium).

OnStep is compatible with the LX200 protocol. This means it can be controlled
from planetarium software, like: Sky Safari, CdC (even without ASCOM),
Stellarium, etc.

There are also INDI drivers so it can be used from Linux, with CdC or KStars.


# More Information
More information, including the full set of features, detailed designs for
PCBs, instructions on how to build a controller, and on the software as well,
can be found at my [StellarJourney web-site](http://www.stellarjourney.com/index.php?r=site/equipment_onstep) and in the [Wiki for the OnStep Group](https://groups.io/g/onstep/wiki/home).

# License
OnStep is open source free software, licensed under the GPL.

See [license.txt](./license.txt) file.

# Author
[Howard Dutton](http://www.stellarjourney.com)
