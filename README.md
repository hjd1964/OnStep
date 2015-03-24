OnStep
======
www.stellarjourney.com

Computerized telescope goto control based on Arduino driven stepper motors for german equatorial mounts.  Preliminary fork mount support is also now included.

Since I aquired an Losmandy G11 mount without any goto provision I decided to create my own solution. I was a bit reluctant to use a Gemini, mainly because of the cost, but also due to the reliablity since the servo motors are somewhat prone to failure. I also like to have complete control over the software/firmware so that no "show stopper" issues will ever be a problem for me.

OnStep is a computerized goto system for stepper motor equipped GEM mounts. It was designed, from the beginning, as a more or less general purpose system and provisions were made in the firmware to allow for use on other-than-G11 GEM mounts. It uses a Meade Autostar/LX200 like computer command set with a few extensions to suit hand-controllerless operation.

The OnStep works through either an ASCOM driver* or a Android (2.3.3 or later) Phone/Tablet (with Bluetooth).
