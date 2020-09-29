// -----------------------------------------------------------------------------------
// Locale English (default,) ISO ISO639-1 language code "en"

// Fonts
// see https://github.com/olikraus/u8g2/wiki/fntlistall for the complete list of fonts
#define LF_CATALOGS u8g2_font_6x13_tf       //  9 pixel height
#define LF_GREEK u8g2_font_unifont_t_greek  // 10 pixel height
#define LF_STANDARD u8g2_font_helvR10_tf    // 11 pixel height
#define LF_LARGE u8g2_font_helvR12_tf       // 12 pixel height

// General
#define L_OK "OK"
#define L_ON "ON"
#define L_OFF "OFF"
#define L_YES "Yes"
#define L_NO "No"
#define L_NOW "Now"
#define L_SETV "Set"
#define L_ALL "All"
#define L_SET_STATE "Set State"
#define L_VALUE "Value"
#define L_WITHIN "Within"
#define L_DISABLE "Disable"
#define L_CANCELED "Canceled"
#define L_FAILED "Failed"
#define L_DEGREE "degree"
#define L_ARCSEC "arc-sec"
#define L_MICRON_PER_C "um/C"
#define L_DAYS "Days"

// common abbreviations
#define L_TELESCOPE "Scope" // telescope
#define L_SEC_ABV "sec"     // seconds
#define L_MIN_ABV "min"     // minutes
#define L_HRS_ABV "hrs"     // hours
#define L_OFFSET_ABV "Ofs"  // offset
#define L_TZ_ABV "Zone"     // time-zone
#define L_RA "RA"           // Right Ascension
#define L_DE "De"           // Declination
#define L_DEC "Dec"
#define L_AZ "Az"           // Azimuth
#define L_ALT "Alt"         // Altitude

// --------------------- menu, alignment -------------------

#define L_ALGN_RESUME "Resume Align"
#define L_ALGN_SHOW_CORR "Show Corr"
#define L_ALGN_CLEAR_CORR "Clear Corr"
#define L_ALGN_RESET_HOME "Reset Home"
#define L_ALIGNMENT "Alignment"
#define L_ALGN_STAR_ALIGN "Star Align"
#define L_ALGN_SHOW_MODEL "Show Model"
#define L_ALGN_CLEAR_MODEL "Clear Model"
#define L_ALGN_RESET_HOME "Reset Home"

#define L_ALGN_RESUME0 "Reset"
#define L_ALGN_RESUME1 "Move to Home"
#define L_ALGN_RESUME2 "Date/Time"
#define L_ALGN_RESUME3 "Not Set!"
#define L_ALGN_RESUME4 "Guide Rate"
#define L_ALGN_RESUME5 "48X Set"

#define L_ALGN_REFINE_MSG1 "Setup & 3+ star"
#define L_ALGN_REFINE_MSG2 "align mount."
#define L_ALGN_REFINE_MSG3 "Goto bright star"
#define L_ALGN_REFINE_MSG4 "near NCP/SCP w/"
#define L_ALGN_REFINE_MSG5 "Dec in 50 to 80"
#define L_ALGN_REFINE_MSG6 "deg range N/S."
#define L_ALGN_REFINE_MSG7 "Answr YES below."
#define L_ALGN_REFINE_MSG8 "Use Polar Align"
#define L_ALGN_REFINE_MSG9 "adjust controls to"
#define L_ALGN_REFINE_MSG10 "center star again."
#define L_ALGN_REFINE_MSG11 "Optionally align"
#define L_ALGN_REFINE_MSG12 "the mount again."
#define L_ALGN_REFINE_MSG13 "Refine PA?"
#define L_ALGN_REFINE_MSG14 "Center star again"
#define L_ALGN_REFINE_MSG15 "using PA controls"

// ----------------------- menu, main ----------------------

// feature key menu
#define L_FKEY_GUIDE_RATE "Guide Rate"
#define L_FKEY_PULSE_GUIDE_RATE "Pulse Guide Rate"
#define L_FKEY_UTILITY_LIGHT "Utility Light"
#define L_FKEY_RETICLE "Reticle"
#define L_FKEY_FOCUSER1 "Focuser 1"
#define L_FKEY_FOCUSER "Focuser"
#define L_FKEY_FOCUSER2 "Focuser 2"
#define L_FKEY_ROTATOR "Rotator"
#define L_FKEY_FEATURE_KEYS "Feature Keys"

// main menu root
#define L_MM_GOTO "Goto"
#define L_MM_SYNC "Sync"
#define L_MM_ALIGN "Align"
#define L_MM_PARKING "Parking"
#define L_MM_TRACKING "Tracking"
#define L_MM_PEC "PEC"
#define L_MM_SETTINGS "Settings"
#define L_MM_MAIN_MENU "Main Menu"
	  
// park menu
#define L_PARK "Park"
#define L_SETPARK "Set-Park"
#define L_UNPARK "Un-Park"
#define L_PARKING "Parking"
#define L_UNPARKING "Un-Parking"

// tracking menu
#define L_TRK_START "Start"
#define L_TRK_STOP "Stop"
#define L_TRK_SIDEREAL "Sidereal"
#define L_TRK_SOLAR "Solar"
#define L_TRK_LUNAR "Lunar"
#define L_TRK_RESET "Rate Reset"
#define L_TRK_FASTER "Rate +0.02Hz"
#define L_TRK_SLOWER "Rate -0.02Hz"
#define L_TRACKING "Tracking"
#define L_TRK_CF "Comp Full"
#define L_TRK_CR "Comp Refr"
#define L_TRK_CO "Comp Off"
#define L_TRK_CS "Comp Sngl Ax"
#define L_TRK_CD "Comp Dual Ax"

// PEC menu
#define L_PEC_PLAY "Play"
#define L_PEC_STOP "Stop"
#define L_PEC_CLEAR "Clear"
#define L_PEC_RECORD "Record"
#define L_PEC_WRITENV "Write to NV"
#define L_PEC "PEC"
#define L_PEC_PLAYING "Playing"
#define L_PEC_STOPPED "Stopped"
#define L_PEC_RECORDING "Recording"

// ---------------------- menu, mount ----------------------

// root menu
#define L_MOUNT_SPEED "Goto Speed"
#define L_MOUNT_BL "Backlash"
#define L_MOUNT_LIMITS "Limits"
#define L_MOUNT_PIER "Pier Side"
#define L_MOUNT_CONFIG "Configuration"

#define L_MOUNT_FASTEST "Fastest"
#define L_MOUNT_FASTER "Faster"
#define L_MOUNT_DEFAULT_SPEED "Default Speed"
#define L_MOUNT_SLOWER "Slower"
#define L_MOUNT_SLOWEST "Slowest"
#define L_MOUNT_SPEED "Goto Speed"

#define L_MOUNT_BL "Backlash"
#define L_MOUNT_LIMIT_H "Horizon"
#define L_MOUNT_LIMIT_O "Overhead"
#define L_MOUNT_LIMIT_ME "Meridian E"
#define L_MOUNT_LIMIT_MW "Meridian W"

#define L_MOUNT_LIMITS "Limits"
#define L_MOUNT_LIMIT_HORIZON "Horizon Limit"
#define L_MOUNT_LIMIT_OVERHEAD "Overhead Limit"
#define L_MOUNT_LIMIT_MERIDIAN_EAST "Meridn Limit E"
#define L_MOUNT_LIMIT_MERIDIAN_WEST "Meridn Limit W"
#define L_MOUNT_PPS "Preferred Pier Side"
#define L_PPS_BEST "Best"
#define L_PPS_EAST "East"
#define L_PPS_WEST "West"

// -------------------- menu, settings ---------------------

// root menu
#define L_SET_DATE_TIME "Date/Time"
#define L_SET_SITE "Site"
#define L_SET_FOCUSER "Focuser"
#define L_SET_FOCUSER1 "Focuser1"
#define L_SET_FOCUSER2 "Focuser2"
#define L_SET_ROTATOR "Rotator"
#define L_SET_DISPLAY "Display"
#define L_SET_BUZ "Buzzer"
#define L_SET_MERIDIAN_FLIP "Meridian Flip"
#define L_SET_CONFIG "Configuration"
#define L_SET_VERSION "Firmware Ver"
#define L_SETTINGS "Settings"

// date/time
#define L_SET_LOCAL_DATE "Local Date"
#define L_SET_LOCAL_PM "Local Time PM"
#define L_SET_LOCAL_DST "Local Time DST"

// display
#define L_SET_DISP_OFF "Turn Off"
#define L_SET_DISP_CONT "Contrast"
#define L_SET_DISP_DIM_TO "Dim Timeout"
#define L_SET_DISP_BLANK_TO "Blank Timeout"
#define L_DISPLAY "Display"
#define L_SET_DISP_MSG1 "Press any key"
#define L_SET_DISP_MSG2 "to turn on"
#define L_SET_DISP_MIN "Min"
#define L_SET_DISP_LOW "Low"
#define L_SET_DISP_HIGH "High"
#define L_SET_DISP_MAX "Max"
#define L_SET_DISP_CONTRAST "Set Contrast"
#define L_SET_BUZZER "Buzzer On?"

// meridian flips
#define L_SET_MF_AUTO "Automatic"
#define L_SET_MF_PAUSE "Pause at Home"
#define L_SET_MF "Meridian Flip"
#define L_SET_MF_AF "Auto Flip?"
#define L_SET_MF_PF "Pause Flip?"

// site
#define L_SET_SITE_SELECT "Select Site"
#define L_SET_SITE_LAT "Latitude"
#define L_SET_SITE_LONG "Longitude"
#define L_SET_SITE_UTC "UTC Offset"
#define L_SET_SITE_TITLE "Menu Site"
#define L_SET_SITE_NUM "Site"
#define L_SET_SITE_NUM_TITLE "Menu Sites"

// focuser
#define L_FOC_RET_HOME "Return Home"
#define L_FOC_AT_HOME  "At Home"
#define L_FOCUSER "Focuser"
#define L_FOC_AT_HALF "At Half Trvl?"
#define L_FOC_TC "Temp. Comp?"
#define L_FOC_TC_COEF "TC Coef."
#define L_FOC_TC_DEADBAND "TC Deadband"
#define L_FOC_TC_DB_UNITS "micron(s)"
#define L_FOC_BACKLASH "Backlash"
#define L_FOC_BL_UNITS "micron(s)"

// rotator
#define L_ROT_RET_HOME "Return Home"
#define L_ROT_AT_HOME "At Home"
#define L_ROT_DEROT "De-rotate"
#define L_ROT_PA "Move to PA"
#define L_ROT_REV "Reverse"
#define L_ROTATOR "Rotator"
#define L_ROT_AT_HOME_ZERO "At Home/Zero?"
#define L_ROT_REVERSE "Reverse"

// -------------------- menu, sync/goto --------------------

// root menu
#define L_SG_SOLSYS "Solar System"
#define L_SG_HERE "Here"
#define L_SG_USER "User"
#define L_SG_FILTERS "Filters \xa5"
#define L_SG_COORDS "Coordinates"
#define L_SG_HOME "Home"
#define L_SG_SYNC "Sync"
#define L_SG_GOTO "Goto"

// return home or reset at home
#define L_SG_HOME1 "Goto Home will"
#define L_SG_HOME2 "clear the Model"
#define L_SG_HOME3 "Goto Home"
#define L_SG_HOME4 "Reset at"
#define L_SG_HOME5 "Goto"
#define L_SG_HOME6 "Home Position"
#define L_SG_NO_OBJECT "No Object"
#define L_SG_NO_INIT "Not Init'd"

// solsys
#define L_SG_SUN "Sun"
#define L_SG_MER "Mercury"
#define L_SG_VEN "Venus"
#define L_SG_MAR "Mars"
#define L_SG_JUP "Jupiter"
#define L_SG_SAT "Saturn"
#define L_SG_URA "Uranus"
#define L_SG_NEP "Neptune"
#define L_SG_MOON "Moon"
#define L_SG_SSOL "Sync Sol Sys"
#define L_SG_GSOL "Goto Sol Sys"
#define L_SG_SOL_WARN1 "Pointing at the Sun"
#define L_SG_SOL_WARN2 "can be dangerous"
#define L_SG_GSUN "Goto Sun"

// user catalogs
#define L_SG_USER_MSG1 "Select User Cat"
#define L_SG_USER_MSG2 "No Catalogs"
#define L_SG_SYNC_USER "Sync User"
#define L_SG_GOTO_USER "Goto User"
#define L_SG_SYNC_USER_ITEM "Sync User Item"
#define L_SG_GOTO_USER_ITEM "Goto User Item"

// --------------------- menu, filters ---------------------

// root menu
#define L_SG_FILT_RESET "Reset filters"
#define L_SG_FILT_HOR "Above Horizon"
#define L_SG_FILT_CON "Constellation"
#define L_SG_FILT_TYP "Type"
#define L_SG_FILT_MAG "Magnitude"
#define L_SG_FILT_NEAR "Nearby"
#define L_SG_FILT_VAR_MAX "Var* Max Per."
#define L_SG_FILT_DBL_MIN "Dbl* Min Sep."
#define L_SG_FILT_DBL_MAX "Dbl* Max Sep."
#define L_SG_FILT_ALLOW "Filters Allow"
#define L_SG_FILT_MSG1 "Filters"
#define L_SG_FILT_MSG2 "Reset"
#define L_SG_FILT_MSG3 "Above Only"
#define L_SG_FILTER "Filter"

// filter constellation
#define L_SG_FILT_BY_CON "Filter by Con"

// filter type
#define L_SG_FILT_BY_TYPE "Filter by Type"

// filter magn
#define L_SG_FILT_BY_MAG "Filter Magnitude"

// filter nearby
#define L_SG_FILT_BY_NEAR "Filter Nearby"

// filter dbl min sep
#define L_SG_FILT_BY_SEP_MIN "Filter Dbl* Sep."
#define L_SG_SEP_MIN_MSG1 "Min Sep must"
#define L_SG_SEP_MIN_MSG2 "be < Max Sep."

// filter dbl max sep
#define L_SG_FILT_BY_SEP_MAX "Filter Dbl* Sep."
#define L_SG_SEP_MAX_MSG1 "Max Sep must"
#define L_SG_SEP_MAX_MSG2 "be > Min Sep."

// filter var max per
#define L_SG_FILT_BY_PER_MAX "Filter Var* Period"

// ------------------- SmartController.h -----------------------
#define L_ESTABLISHING "Establishing"
#define L_CONNECTION "Connection"
#define L_WARNING "Warning"
#define L_COORDINATES "Coordinates"
#define L_OBSERVED_PLACE "Observed Place."
#define L_WARNING "Warning"
#define L_FAILED "Failed"
#define L_REBOOT "REBOOT"
#define L_DEVICE "DEVICE"
#define L_STAR "Star"
#define L_ALIGN_MSG1 "Select a Star"
#define L_ALIGN_MSG2 "From following list"
#define L_ABORTED "Aborted"
#define L_DISCONNECT_MSG1 "Hand controler"
#define L_DISCONNECT_MSG2 "not connected"
#define L_SLEW_MSG1 "Slew to target"
#define L_SLEW_MSG2 "canceled"
#define L_FKEY_GUIDE_DN "Guide Slower"
#define L_FKEY_GUIDE_UP "Guide Faster"
#define L_FKEY_PGUIDE_DN "PGuide Slower"
#define L_FKEY_PGUIDE_UP "PGuide Faster"
#define L_FKEY_LAMP_DN "Util Dimmer"
#define L_FKEY_LAMP_UP "Util Brighter"
#define L_FKEY_RETI_DN "Reticle Dimmer"
#define L_FKEY_RETI_UP "Reticle Brighter"
#define L_FKEY_FOC_DN "Focus Out"
#define L_FKEY_FOC_UP "Focus In"
#define L_FKEY_FOCF_DN "Focus Fast"
#define L_FKEY_FOCF_UP "Focus Fast"
#define L_FKEY_ROT_DN "Rotate Ccw"
#define L_FKEY_ROT_UP "Rotate Cw"
#define L_FKEY_ROTF_DN "Rotate Fast"
#define L_FKEY_ROTF_UP "Rotate Fast"
#define L_SUCCESS "Success"
#define L_ADD_STAR "Add Star"
#define L_SUCCESS "Success"
#define L_FAILED "Failed"
#define L_SLEWING_TO_STAR "Slewing to Star"
#define L_RECENTER_STAR "Recenter Star"
#define L_SELECT_STAR "Select Star"
#define L_LX200_NOTOK_1 "LX200 Command"
#define L_LX200_NOTOK_2 "has failed!"
#define L_LX200_SETVF_1 "Set Value"
#define L_LX200_SETVF_2 "has failed!"
#define L_LX200_GETVF_1 "Get Value"
#define L_LX200_GETVF_2 "has failed!"
#define L_LX200_SETTG_1 "Set Target"
#define L_LX200_SETTG_2 "has failed!"
#define L_LX200_OBJSE_1 "No Object"
#define L_LX200_OBJSE_2 "Selected!"
#define L_LX200_TGHOR_1 "Target is"
#define L_LX200_TGHOR_2 "Below Horizon!"
#define L_LX200_TGOVH_1 "Target is"
#define L_LX200_TGOVH_2 "Above Limit!"
#define L_LX200_STNBF_1 "Telescope"
#define L_LX200_STNBF_2 "in standby!"
#define L_LX200_PARKF_1 "Telescope"
#define L_LX200_PARKF_2 "is Parked!"
#define L_LX200_GOGOF_1 "Goto already"
#define L_LX200_GOGOF_2 "in progress!"
#define L_LX200_LIMTF_1 "Target"
#define L_LX200_LIMTF_2 "outside lmts!"
#define L_LX200_HARDF_1 "Telescope"
#define L_LX200_HARDF_2 "h/w fault!"
#define L_LX200_GOMOF_1 "Telescope"
#define L_LX200_GOMOF_2 "in motion!"
#define L_LX200_UNSPF_1 "Goto unknown"
#define L_LX200_UNSPF_2 "error!"
#define L_LX200_ERROR "Error"
#define L_LX200_ISAOK_1 "LX200 Command"
#define L_LX200_ISAOK_2 "Done!"
#define L_LX200_SETOK_1 "Value"
#define L_LX200_SETOK_2 "Set!"
#define L_LX200_GETOK_1 "Value"
#define L_LX200_GETOK_2 "Get!"
#define L_LX200_SNCOK_1 "Telescope"
#define L_LX200_SNCOK_2 "Synced!"
#define L_LX200_GOTOK_1 "Slew to"
#define L_LX200_GOTOK_2 "Target"

// ----------------------- u8g2_ext_catalog.cpp -------------------------

#define L_CAT_PER_UNK "Period Unknown"
#define L_CAT_PER_IRR "Period Irregular"
#define L_CAT_PER "Period"
#define L_CAT_UNK "Unknown"
#define L_CAT_OC "Open Cluster"
#define L_CAT_GC "Globular Cluster"
#define L_CAT_PN "Planetary Nebula"
#define L_CAT_SG "Spiral Galaxy"
#define L_CAT_EG "Eliptical Galaxy"
#define L_CAT_IG "Irregular Galaxy"
#define L_CAT_KNT "Knot"
#define L_CAT_SNR "SuperNova Rmnnt"
#define L_CAT_GAL "Galaxy"
#define L_CAT_CN "Cluster + Nebula"
#define L_CAT_STR "Star"
#define L_CAT_PLA "Planet"
#define L_CAT_CMT "Comet"
#define L_CAT_AST "Asteroid"

// ----------------------- u8g2_ext_input.cpp -------------------------

#define L_RIGHT_ASC "Right Asc."
#define L_DECLINATION "Declination"
#define L_LOCAL_TIME "Local Time"
#define L_LATITUDE "Latitude"
#define L_LONGITUDE "Longitude"
