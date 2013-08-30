OnStep
======

Arduino based goto system for stepper motor driven german equatorial telescope mounts


Since I aquired an Losmandy G11 mount without any goto provision I decided to create my own solution. I was a bit reluctant to use a Gemini, mainly because of the cost, but also due to the reliablity since the servo motors are somewhat prone to failure. I also like to have complete control over the software/firmware so that no "show stopper" issues will ever be a problem for me.

OnStep is a computerized goto system for stepper motor equipped GEM mounts. It was designed, from the beginning, as a more or less general purpose system and provisions were made in the firmware to allow for use on other-than-G11 GEM mounts. It uses a Meade Autostar/LX200 like computer command set with a few extensions to suit hand-controllerless operation.

The OnStep works through either an ASCOM driver* or a Android (4.0 or later) Phone/Tablet (with Bluetooth), functionality supported includes:

My setup slews at about 0.67°/Second (conservative setting). Faster slews are possible with larger stepper motors and/or higher voltages.
Initialization of date/time/location, and align with one, two, or three stars.
Meridian flips happen automatically as needed, the software doesn't allow you to pick the meridian side during slews, but it does provide a command to inform you as to what side of the mount the telescope is on.
Align is currently tested and working with one-star only. Two and three star align corrects for polar misalignment (untested code). Further on, work has started with more advanced corrections for mount geometry and tube flexture - this is the last area of development before version 1.0 is released.
The guide commands are for pulse-guiding, centering objects, etc.
Parking functionality includes the ability to set the park position (anywhere you like), park, and to unpark.
Home commands including the ability to move the telescope to the home position and stop tracking or reset the controller when at the home position for initialization.
PEC commands include the ability to record, play-back, read, and write PEC data.
The Android App allows for initialization, parking, alignment, and PEC programming. Several catalogs of objects are included for Goto's: Messier, NGC/IC, Herschel 400, and a named bright star catalog.

* Stellarium is supported directly through the LX200 protocol, other ASCOM compatible planetarium programs are confirmed to work also. Stellarium's the first confirmed LX200 protocol compatible software package. Others might be compatible as well, but hardware modification (or alternate serial comms provision) might be necessary to prevent an "inadvertent reset on connect" problem that many Arduino's have. Also, the Bluetooth (hand-controller App) option might be necessary to allow you to get unparked/aligned and going while connected with Stellarium. In a future revision I'll probably add an automatic unpark on power up option to get around this limitation.

A detailed command description and the software/firmware downloads are on the Telescope Driver/On-Step Firmware page.
