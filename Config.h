// -----------------------------------------------------------------------------------
// Configuration

// -------------------------------------------------------------------------------------------------------------------------
// ADJUST THE FOLLOWING TO CONFIGURE YOUR CONTROLLER FEATURES --------------------------------------------------------------

// turns debugging on, used during testing, default=OFF
#define DEBUG_OFF
// for getting control of the 'scope when things go horribly wrong, default=OFF
#define RESCUE_MODE_OFF

// PPS sense rising edge on pin 21 for optional precision clock source (GPS, for example), default=OFF
#define PPS_SENSE_OFF
// PEC sense rising edge on pin 2 for optional PEC index, default=OFF (not tested)
#define PEC_SENSE_OFF
// switch close (to ground) on pin 3 for optional limit sense (stops gotos and/or tracking), default=OFF (not tested)
#define LIMIT_SENSE_OFF
// light status LED by sink to ground (pin 9) and source +5V (pin 8), default=ON
#define STATUS_LED_PINS_ON
// lights 2nd status LED by sink to ground (pin 10), default=OFF
#define STATUS_LED2_PINS_OFF
// optional +5V on pins 5 and 11 to Pololu or other stepper drivers without on-board 5V voltage regulators, default=OFF
#define POWER_SUPPLY_PINS_OFF
// optional stepper driver Enable support is always on (no setting,) just wire Enable to Pins 25 (HA) and 30 (Dec) and OnStep will pull these pins High
// to disable stepper drivers on startup and when Parked. An Align or UnPark will enable the drivers.  (Pin assignments for the Mega2560, see OnStep.ino for the Teensy3.1)

// enables goTo speed equalization for differing right ascension and declination StepsPerDegreeHA/Dec, default=OFF (limited testing done)
#define DEC_RATIO_OFF

// enables alignment on two or three stars, default=ON
#define ALIGN_TWO_AND_THREE_STAR_ON

// enables Horizon coordinate goto functions, default=ON
#define ALT_AZM_GOTO_ON

// enables code to clean-up PEC readings after record (use PECprep or a spreadsheet to fix readings otherwise)
// this cleans up any tracking rate variations that would be introduced by recording more guiding corrections to either the east or west, default=ON
#define PEC_CLEANUP_ON

// optionally adjust tracking rate to compensate for atmospheric refraction, default=OFF (not tested)
// can be turned on/off with the :Te# and :Td# commands regardless of this setting
#define TRACK_REFRACTION_RATE_DEFAULT_OFF

// these turn on and off checksum error correction on the serial ports, default=OFF
#define CHKSUM0_OFF     // default _OFF: required for OnStep ASCOM driver
#define CHKSUM1_OFF     // default _OFF: required for OnStep Controller2 Android App (and others)

// ADJUST THE FOLLOWING TO MATCH YOUR MOUNT --------------------------------------------------------------------------------
#define MaxRate                   96 // this is the minimum number of micro-seconds between micro-steps
                                     // minimum* (fastest goto) is around 16 (Teensy3.1) or 32 (Mega2560), default is 96, higher is ok
                                     // too low and OnStep communicates slowly and/or freezes as the motor timers use up all the MCU time
                                     // * = minimum can be lower, when both HA & DE_MODE_GOTO are used by HA/DE_STEP_GOTO times.
                                     
#define StepsForRateChange  200000.0 // number of steps during acceleration and de-acceleration: higher values=longer acceleration/de-acceleration
                                     // for the most part this doesn't need to be changed, but adjust when needed

#define BacklashTakeupRate        25 // backlash takeup rate (in multipules of the sidereal rate): too fast and your motors will stall,
                                     // too slow and the mount will be sluggish while it moves through the backlash
                                     // for the most part this doesn't need to be changed, but adjust when needed
                                     // this should be 60 or lower

                                     // for my mounts both RA and Dec axis have the same gear train
#define StepsPerDegreeHA      19200L // calculated as    :  stepper_steps * micro_steps * gear_reduction1 * (gear_reduction2/360)
                                     // Takahashi EM10b  :  48            * 16          * 50 * (40/32)    *  144/360              = 19200
                                     // Losmandy G11     :  400           * 32          * 1               *  360/360              = 12800
#define StepsPerDegreeDec     19200L // calculated as    :  stepper_steps * micro_steps * gear_reduction1 * (gear_reduction2/360)
                                     // Takahashi EM10b  :  48            * 16          * 50 * (40/32)    *  144/360              = 19200
                                     // Losmandy G11     :  400           * 32          * 1               *  360/360              = 12800
                                     // the EM10b has two spur gears that drive the RA/Dec worms, they are 60 tooth and 48 tooth gears
                                     // for an 1.25x reduction in addition to the 50:1 gear heads on the steppers for a 62.5:1 final ratio
                                     // before the worm/wheels 144:1
                                     
#define StepsPerSecond          80.0 // the steps per second sidereal rate = (19200/3600)*15 = 80 - OnStep can handle between 12 and 100 steps/second
                                     // StepsPerSecond doesn't need to be an integer
                                     // StepsPerWormRotation (for PEC) needs to be evenly divisible by StepsPerSecond

#define StepsPerWormRotation  38400L // PEC, number of steps for a complete worm rotation (in RA), (StepsPerDegreeHA*360)/gear_reduction2 
                                     // the EM10b has a worm-wheel with 144 teeth (19200*360)/144 = 48000
                                     // According to this the EM10b needs 600 (seconds) of PEC buffer.  Since the transfer gears have a rather large effect on the periodic error
                                     // I have two options 1. use 4x the 48000 = 192000 (2400 seconds.)  This is 4 complete rotations of the worm and 5 complete rotations of the transfer gear.
                                     //                       I expect the results would be very good, but since PECPrep is limited to 1000S I don't bother.
                                     //                    2. use 480 (seconds) of PEC buffer.  This is easier to program and keeps the PE at low levels
                                     //                       so pretending we have 180 teeth (19200*360)/180 = 38400

#define PECBufferSize           2400 // PEC, buffer size, max should be no more than 3384, your required buffer size >= StepsPerWormRotation/StepsPerSecond
                                     // for the most part this doesn't need to be changed, but adjust when needed.  824 seconds is the default

#define REVERSE_HA_OFF               // reverse the direction of movement for the HA/RA axis, adjust as needed or reverse your wiring so things move in the right direction
#define REVERSE_DEC_OFF              // reverse the direction of movement for the Dec axis (both reversed for my EM10b, both normal for G11)

#define minutesPastMeridianE      60 // for goto's, how far past the meridian to allow before we do a flip (if on the East side of the pier) - one hour of RA is the default = 60
#define minutesPastMeridianW      60 // as above, if on the West side of the pier.  If left alone, the mount will stop tracking when it hits the this limit
#define underPoleLimit             9 // maximum allowed hour angle (+/-) under the celestial pole. OnStep will flip the mount and move the Dec. past 90 degrees (+/-) once past this limit
                                     // to arrive at the location.  If left alone, the mount will stop tracking when it hits this limit.  Valid range is 7 to 11 hours
#define minDec                   -91 // minimum allowed declination, default = -91 (off)
#define maxDec                   +91 // maximum allowed declination, default =  91 (off)
                                     // For example, a value of +80 would stop gotos/tracking near the north celestial pole.
                                     // For a Northern Hemisphere user, this would stop tracking when the mount is in the polar home position but
                                     // that can be easily worked around by doing an alignment once and saving a park position (assuming a 
                                     // fork/yolk mount with meridian flips turned off by setting the minutesPastMeridian values to cover the whole sky)

// this group is for advanced configuration and is not well tested yet, leave it alone or just use the HA_MODE and DEC_MODE values if you didn't hard wire the micro-stepping mode
// M0, M1, and M2 are on Pins 22,23, and 24 for RA.  M0, M1, M2 are on Pins 27,28,29 for Dec.  (For the Mega2560, see OnStep.ino for the Teensy3.1)
// DRV8825: 5=32x, 4=16x, 3=8x, 2=4x, 1=2x, 0=1x
#define HA_MODE_OFF                  // programs the HA uStep mode M0/M1/M2, optional and default _OFF. Other values 0 to 7 (0xb000 to 111): for example "#define HA_MODE 4"
#define HA_MODE_GOTO_OFF             // programs the HA uStep mode M0/M1/M2, used during gotos, optional and default _OFF. Other values 0 to 7 (0xb000 to 111): for example "#define HA_MODE_GOTO 4"
#define HA_STEP_GOTO 1               // 1=goto mode is same as normal mode: for example if normal tracking mode is 32x and goto is 8x this would be 4
#define DE_MODE_OFF                  // programs the Dec uStep mode M0/M1/M2, optional and default _OFF. Other values 0 to 7 (0xb000 to 111)
#define DE_MODE_GOTO_OFF             // programs the Dec uStep mode M0/M1/M2, used during gotos, optional and default _OFF. Other values 0 to 7 (0xb000 to 111)
#define DE_STEP_GOTO 1               // 1=goto mode is same as normal mode: for example if normal tracking mode is 32x and goto is 8x this would be 4

// THAT'S IT FOR USER CONFIGURATION!

// -------------------------------------------------------------------------------------------------------------------------
