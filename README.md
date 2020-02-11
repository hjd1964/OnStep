OnStep Telescope Controller
===========================
# Important Note

THERE ARE SEVERAL GITHUB BRANCHES OF ONSTEP:
* THE **MASTER BRANCH** IS THE MOST UP TO DATE ONSTEP VERSION, WHERE WE ADD NEW FEATURES.  IT IS THE LEAST WELL TESTED BRANCH AND SHOULD ONLY BE USED BY EXPERIENCED USERS WILLING TO TEST FOR BUGS!
* THE **BETA BRANCH**, IF PRESENT, IS A "SNAP-SHOT" OF THE MASTER WHERE WE HAVE REACHED A POINT OF APPARENT STABILITY.  THIS PROVIDES ACCESS TO MOST NEW FEATURES FOR ADVENTUROUS USERS.
* THE **RELEASE BRANCHES** ARE WELL TESTED AND WHAT MOST SHOULD USE.  USUALLY THE NEWEST (HIGHEST REVISION) RELEASE IS RECOMMENDED.  NO NEW FEATURES AND ONLY BUG FIXES WHERE NECESSARY AND SAFE.

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

* an [ASCOM](http://ascom-standards.org/) driver (with IP and Serial support),
* an Android App useable over WiFi or Bluetooth equipped Phones/Tablets
  (version 2.3.3 or later),
* a "built-in" website (on the Ethernet and/or WiFi device),
* a full planetarium program that controls all features ([Sky Planetarium](http://stellarjourney.com/index.php?r=site/software_sky)).

OnStep is compatible with the LX200 protocol. This means it can be controlled
from other planetarium software, like: Sky Safari, CdC (even without ASCOM),
Stellarium, etc.

There are also [INDI](http://www.indilib.org/about.html) drivers so it can be used from Linux, with CdC or KStars.

# Documentation
Detailed documentation, including the full set of features, detailed designs for
PCBs, instructions on how to build a controller, how to configure the firmware
for your particular mount, can all be found the [OnStep Group Wiki](https://groups.io/g/onstep/wiki/home).

# Change Log
All the changes are tracking in git, and a detailed list can be accessed using the
following git command:
 
git log --date=short --pretty=format:"%h %ad %<(20)%an %<(150,trunc)%s"

# Support
Questions and discussion should be on the mailing list (also accessible via the
web) at the [OnStep Group](https://groups.io/g/onstep/).

# License
OnStep is open source free software, licensed under the GPL.

See [LICENSE.txt](./LICENSE.txt) file.

# Author
[Howard Dutton](http://www.stellarjourney.com)
