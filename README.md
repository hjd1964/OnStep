OnStep Telescope Controller
===========================
# Important Note
THIS IS THE ALPHA BRANCH, THE MOST ADVANCED ONSTEP VERSION BUT ALSO THE
LEAST WELL TESTED!

THE BETA BRANCH IS RECOMMENDED FOR MOST USERS.

# What is OnStep?
OnStep is a computerized telescope goto controller, based on Teensy or
Arduino driven stepper motors.

It supports German Equatorial Mounts (GEMs), as well as Alt-Az mounts
(including Dobsonians, fork mounts, and the like).

OnStep was designed, from the beginning, as a more or less general purpose
system and provisions were made in the firmware to allow for use on a variety
of mounts.

# Features
OnStep supports a wide variety of connection options.  In addition to two
serial ports, the Arduino Ethernet Shield is also supported.  One of the
serial ports is normally devoted to a USB connection and the other to
Bluetooth, though they can be used as serial connections to anything you
like.

Other software in the OnStep ecosystem include:

* an ASCOM driver (with Ethernet and Serial support),
* an Android App useable over WiFi or Bluetooth equipped Phones/Tablets
  (version 2.3.3 or later),
* a built-in website (Ethernet, or WiFi),
* a full planetarium program that controls all features (Sky Planetarium).

OnStep is compatible with the LX200 protocol. This means it can be controlled
from planetarium software, like: Sky Safari, CdC (even without ASCOM),
Stellarium, etc.

There are also INDI drivers so it can be used from Linux, with CdC or KStars.

# Reasons for OnStep
Since I aquired an Losmandy G11 mount without any goto provision I decided to
create my own solution. I was a bit reluctant to use a Gemini, mainly because
of the cost, but also due to the reliablity since the servo motors are somewhat
prone to failure. I also like to have complete control over the software/firmware
so that no "show stopper" issues will ever be a problem for me.

# More Information
More information, including the full set of features, detailed designs for
PCBs, instructions on how to build a controller, and on the software as well,
can be found in the [Wiki for the OnStep Group](https://groups.io/g/onstep/wiki/home).

# License
OnStep is open source free software, licensed under the GPL.

See [license.txt](./license.txt) file.

# Author
[Howard Dutton](http://www.stellarjourney.com)
