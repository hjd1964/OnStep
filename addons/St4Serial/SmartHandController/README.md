OnStep Smart Hand Controller
===========================

# What is a smart hand controller (SHC)?
This firmware is designed to run on a supported smart hand controller hardware to communicate
with an OnStep controller over the ST4 guiding port. Most of the setup and control functions
are available.

This is almost entirely based on Charles Lemaire's excellent TeenAstro work.

At present, there are two hardware designs:
a) Teensy3.2 microcontroller

While you can point to point wire/assemble you own hardware design many will opt to use the
inexpensive [EasyEDA Teensy SHC PCB](https://easyeda.com/hdutton/HC-20e242d665db4c85bb565a0cd0b52233).
For those who choose a more complete DIY experience the EasyEDA schematic still can serve as wiring
instructions. :)

b) ESP32 microcontroller. This offers a lower cost easier to source alternative to the 
Teensy above, with the same features. 

The PCB can be found in the following link (to be announced soon).

# Features
The SHC can:

* Set Site (Latitude/Longitude) and Time.
* Configure most options.  Backlash, limits, etc.
* Center objects and operate focuser/rotator.
* Goto stars, Messier, and Herschel deep sky objects.

# Documentation

See this [Wiki page](https://groups.io/g/onstep/wiki/Smart-Hand-Controller) for more info.

In a nutshell ...

The main display is the RA and Dec that the telescope is currently pointing at.
A small telescope icon with an H under it, means the telescope is in the Home position
When the telescope is slewing, a double triangle with show. When it is tracking, a single
triangle will show, as well as the pier side, and the tracking rate ('s' for sidereal).

- Press any of the N, S, W, E buttons to move the telescope 
- Press the center button to wake up the screen
- Press it again to cycle through status displays (RA/Dec coordinates, Alt-Az coords, Time)

- Quickly press the center button twice to get to the Speed menu
- Long press the center button for the menu
  * Goto
  * Sync
  * Align
  * Parking
  * PEC
  * Settings
- Select a menu by the Up or Down button, then Right to enter that menu

# Support
Questions and discussion should be on the mailing list (also accessible via the
web) at the [OnStep Group](https://groups.io/g/onstep/).

# License
OnStep is open source free software, licensed under the GPL.

See [LICENSE.txt](./LICENSE.txt) file.

# Author
Charles Lemaire, [Howard Dutton](http://www.stellarjourney.com), and [Khalid Baheyeldin](https://baheyeldin.com)
