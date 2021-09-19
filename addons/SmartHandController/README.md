OnStep Smart Hand Controller (SHC)
==================================

The SHC adds the functionality of a "handbox/hand controller" to an OnStep telescope controller. 
This firmware is desgined to run on supported hardware, and communicates with OnStep over the 
ST4 guiding port. It can replace most of the functions provided by the OnStep2 Android App. 

The SHC is almost entirely based on [Charles Lemaire](https://pixelstelescopes.wordpress.com/)'s excellent [TeenAstro fork](https://groups.io/g/TeenAstro/wiki/home).

# Features
The SHC can:

* Set Site (Latitude/Longitude) and Time.
* Configure most options.  Backlash, limits, etc.
* Center objects and operate focuser/rotator.
* Goto stars, Messier, and Herschel deep sky objects.

# Hardware
At present, there are two hardware designs:

a) Teensy3.2 microcontroller
An inexpensive [EasyEDA Teensy SHC 3.2 PCB](https://easyeda.com/hdutton/HC-20e242d665db4c85bb565a0cd0b52233) is available. For those who choose a more complete DIY experience the EasyEDA schematic still can serve as wiring instructions for your perfboard project. :)

b) ESP32 microcontroller.
This offers a lower cost, and easier to source alternative to the Teensy, with the same features.
The microcontroller is faster than the Teensy 3.2, and has more flash memory.

# Documentation
See this [Wiki page](https://groups.io/g/onstep/wiki/Smart-Hand-Controller) for more info.

A quick summary:

The main display is the RA and Dec that the telescope is currently pointing at.
A small telescope icon with an H under it, means the telescope is in the Home position
When the telescope is slewing, a double triangle with show. When it is tracking, a single
triangle will show, as well as the pier side, and the tracking rate ('s' for sidereal).

- Press any of the N, S, W, E buttons to move the telescope 
- Press the center button to wake up the screen
- Press it again to cycle through status displays (RA/Dec coordinates, Alt/Az, Local/Sideral Time)

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

# Catalogs
At present, the SHC contains the following catalogs:
- Stars:
  408 stars up to magnitude 3.85. The list includes most Named Stars.
- Messier Objects:
  All 109 objects are included.
  The identity of M102 is uncertain and therefore not included. See more [here](http://www.messier.seds.org/m/m102d.html) and [here](https://en.wikipedia.org/wiki/Messier_102)
- Caldwell Objects:
  All 109 objects
- Herschel
  All objects are effectively included (398 out of 400 objects)

  Two objects are 'missing', but they are actually included under a different number:
    NGC 2372 is the same object as 2371, which is already in the catalog
    NGC 6885 is the same object as 6882, which is already in the catalog

  Ten other objects were replaced by what they are duplicates of, per the OpenNGC database.
    NGC  651 Duplicate of 650
    NGC 2244 Duplicate of 2239
    NGC 2527 Duplicate of 2520
    NGC 3190 Duplicate of 3189
    NGC 3384 Duplicate of 3371
    NGC 3912 Duplicate of 3899
    NGC 4665 Duplicate of 4624
    NGC 5364 Duplicate of 5317
    NGC 5907 Duplicate of 5906
    NGC 7296 Duplicate of 7295

The SHC's catalogs incorporates data from various sources, with licenses that allow incorporation
in a GPL-licensed project.

At present these are:
- [OpenNGC](https://github.com/mattiaverga/OpenNGC) for the Messier and Herschel catalogs.
- [KStars](https://git.launchpad.net/kstars-bleeding/plain/kstars/data/stars.dat) for bright alignment stars.

The data is extracted and formatted using custom Python scripts authored by [Khalid Baheyeldin](https://baheyeldin.com).

# License
OnStep is open source free software, licensed under the GPL.

See [LICENSE.txt](./LICENSE.txt) file.

# Author
Charles Lemaire, [Howard Dutton](http://www.stellarjourney.com), and [Khalid Baheyeldin](https://baheyeldin.com)
