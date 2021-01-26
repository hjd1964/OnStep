// ---------------------------------------------------------------------------------------------------------------------------------
// Configuration for OnStep Ethernet Add-on

/*
 *          For more information on setting OnStep up see https://onstep.groups.io/g/main/wiki/7149
 *                      and join the OnStep Groups.io at https://groups.io/g/onstep
 * 
 *           *** Read the compiler warnings and errors, they are there to help guard against invalid configurations ***
*/

// ---------------------------------------------------------------------------------------------------------------------------------
// ADJUST THE FOLLOWING TO CONFIGURE YOUR ADD-ON'S FEATURES ------------------------------------------------------------------------
// <-Req'd = always must set, <-Often = usually must set, Option = optional, Adjust = adjust as req'd, Infreq = infrequently changed

//      Parameter Name              Value   Default  Notes                                                                      Hint
// ETHERNET HARDWARE ---------------------------------------------------------------------------------------------------------------
#define W5500                         OFF //    OFF, ON If using W5500 Ethernet Adapter: RST ctrl Pin9, SPI default pins,     Option
                                          //         CS on Pin10. OFF for W5100 adapter. Uses standard Arduino libraries.

// ETHERNET SETTINGS ---------------------------------------------------------------------------------------------------------------
// Enter a unique MAC address for your controller if you like:
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
// The IP addresses below will be dependent on your local network:
IPAddress ip(192, 168, 1, 55);
IPAddress myDns(192,168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

// SERIAL PORTS --------------------------------------------------------------------------------------------------------------------
#define SERIAL_BAUD_DEFAULT          9600 //   9600, Common baud rates for these parameters are 9600,19200,57600,115200.      Infreq
#define SERIAL_BAUD                 57600 //  57600, Use 19200 if talking to a Mega2560 OnStep                               <-Req'd
                                          //         At startup this firmware will attempt to switch OnStep's baud rate to this
                                          //         faster speed (vs. SERIAL_BAUD_DEFAULT) and AFTER success, start WiFi, etc.

// USER FEEDBACK -------------------------------------------------------------------------------------------------------------------
#define LED_STATUS                    OFF //    OFF, GPIO LED pin. Flashes connecting then steady on connected.               Infreq

// DISPLAY -------------------------------------------------------------------------------------------------------------------------
#define DISPLAY_LANGUAGE             L_en //   L_en, English. Specify language with two letter country code, if supported.    Adjust
#define DISPLAY_HIGH_PRECISION_COORDS OFF //    OFF, ON for pull high precision coordinates from OnStep.                      Option
#define DISPLAY_WEATHER               OFF //    OFF, ON ambient conditions in locale default units.                           Option
#define DISPLAY_INTERNAL_TEMPERATURE  OFF //    OFF, ON internal MCU temp. in locale default units.                           Option

#define DISPLAY_SPECIAL_CHARS          ON //     ON, For standard ASCII special symbols (compatibility.)                      Infreq
#define DISPLAY_ADVANCED_CHARS         ON //     ON, For standard "RA/Dec" instead of symbols.                                Infreq

// ENCODER SUPPORT -----------------------------------------------------------------------------------------------------------------
#define ENC_AUTO_SYNC_DEFAULT          ON //     ON, Automatically sync Encoders to OnStep.                                   Option

#define AXIS1_ENC                     OFF //    OFF, CWCCW, AB, BC_BISSC. Dec/Alt Axis (A/CW/MA) & (B/CCW/SLO.)               Option
#define AXIS1_ENC_REVERSE             OFF //    OFF, ON to reverse the count direction.                                       Adjust
#define AXIS1_ENC_TICKS_DEG      22.22222 // 22.222, n, (ticks/degree.) Encoder ticks per degree.                             Adjust
#define AXIS1_ENC_DIFF_LIMIT_TO       300 //    300, n, (arcsec.) Minimum diff. between encoder/OnStep for sync. to OnStep.   Adjust
#define AXIS1_ENC_DIFF_LIMIT_FROM     OFF //    OFF, n, (arcsec.) Maximum diff. between encoder/OnStep for sync. from OnStep. Adjust
                                          //         For absolute encoders, leave off when setting Zero, then enable.
                                          
#define AXIS2_ENC                     OFF //    OFF, CWCCW, AB, BC_BISSC. Dec/Alt Axis (A/CW/MA) & (B/CCW/SLO.)               Option
#define AXIS2_ENC_REVERSE             OFF //    OFF, ON to reverse the count direction.                                       Option
#define AXIS2_ENC_TICKS_DEG      22.22222 // 22.222, n, (ticks/degree.) Encoder ticks per degree.                             Adjust
#define AXIS2_ENC_DIFF_LIMIT_TO       300 //    300, n, (arcsec.) Minimum diff. between encoder/OnStep for sync. to OnStep.   Adjust
#define AXIS2_ENC_DIFF_LIMIT_FROM     OFF //    OFF, n, (arcsec.) Maximum diff. between encoder/OnStep for sync. from OnStep. Adjust
                                          //         For absolute encoders, leave off when setting Zero, then enable.
                                          
// ENCODER RATE CONTROL
#define AXIS1_ENC_RATE_CONTROL        OFF //    OFF, ON Rate control for RA high resolution encoder. EQ mounts only.          Infreq
#define AXIS1_ENC_INTPOL_COS          OFF //    OFF, ON enables cosine compensation feature.                                  Infreq
#define AXIS1_ENC_RATE_AUTO           OFF //    OFF, n, (Worm period in seconds.) Adjusts avg encoder pulse rate to account.  Option
                                          //         For skew in the average guide rate over the last worm period.            Option
#define AXIS1_ENC_BIN_AVG             OFF //    OFF, n, (Number of bins.)  Enables binned rolling average feature.            Option

// THAT'S IT FOR USER CONFIGURATION!
// ---------------------------------------------------------------------------------------------------------------------------------
