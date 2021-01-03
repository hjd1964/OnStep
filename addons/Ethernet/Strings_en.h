// -----------------------------------------------------------------------------------
// Locale English (default,) ISO ISO639-1 language code "en"

// General
#define L_UPLOAD "Upload"
#define L_DOWNLOAD "Download"
#define L_ACCEPT "Accept"
#define L_CONTINUE "Continue"
#define L_ARE_YOU_SURE "Are you sure"
#define L_LOGOUT "Logout"

#define L_UT "UT"
#define L_LST "LST"
#define L_LONG "Long."
#define L_LAT "Lat."
#define L_EAST "East"
#define L_WEST "West"
#define L_ZENITH "Zenith"
#define L_NCP "NCP"
#define L_SCP "SCP"

#define L_NONE "None"
#define L_AUTO "Auto"
#define L_OK "Ok"
#define L_ON "On"
#define L_OFF "Off"
#define L_START "Start"
#define L_DELTA "Delta"
#define L_ZERO "Zero"
#define L_SPAN "Span"
#define L_POWER "Pwr"
#define L_GUIDE "Guide"
#define L_CENTER "Center"
#define L_BLUE "Blue"
#define L_GREEN "Green"
#define L_RATE "rate"
#define L_RANGE "range"
#define L_UNKNOWN "Unknown"
#define L_FAULT "Fault"

// ---------------------------- Index ----------------------------
// date/time
#define L_TIME_BROWSER_UT "UT (web browser)"

// weather
#define L_TELE_TEMPERATURE "Telescope Temperature"
#define L_TEMPERATURE "Ambient Temperature"
#define L_PRESSURE "Barometric Pressure"
#define L_HUMIDITY "Relative Humidity"
#define L_DEW_POINT "Dew Point Temperature"

// operation
#define L_TRACKING "Tracking"
#define L_SITE "Site"
#define L_COORDINATES "Coordinates"
#define L_CURRENT "Current"
#define L_TARGET "Target&nbsp;&nbsp;"
#define L_ENCODER "Encodr"
#define L_POLAR_ALIGN "Polar Alignment"

// pier side
#define L_PIER_SIDE "Pier Side"
#define L_MERIDIAN_FLIPS "meridian flips"
#define L_ALIGN_MESSAGE "Mount relative to"
#define L_MERIDIAN_FLIP_W_TO_E "Meridian Flip, West to East"
#define L_MERIDIAN_FLIP_E_TO_W "Meridian Flip, East to West"

#define L_OPERATIONS "Operations"

// parking
#define L_PARKING "Parking"
#define L_PARKED "Parked"
#define L_NOT_PARKED "Not Parked"
#define L_PARKING "Parking"
#define L_PARK_FAILED "Park Failed"

// home
#define L_AT_HOME "At Home"
#define L_SLEWING "Slewing"
#define L_PPS_SYNC "PPS Sync"

// tracking
#define L_REFR_COMP_RA "Refr Comp RA Axis"
#define L_REFR_COMP_BOTH "Refr Comp Both Axis"
#define L_FULL_COMP_RA "Full Comp RA Axis"
#define L_FULL_COMP_BOTH "Full Comp Both Axis"
#define L_TRACKING_RATE "Tracking Rate"
#define L_CURRENT_MAXRATE "Current MaxRate"
#define L_DEFAULT_MAXRATE "Default MaxRate"
#define L_CURRENT_MAXSLEW "Maximum slew speed"

// driver status
#define L_DRIVER_STATUS "Driver"
#define L_STATE "State"
#define L_COMMS_FAILURE "Comms failure"
#define L_STANDSTILL "Standstill"
#define L_OPEN_LOAD "Open Load"
#define L_SHORT_GND "Short Gnd"
#define L_SHUTDOWN_OVER "Shutdown Over"
#define L_PRE_WARNING "Pre-warning"

// misc. errors/workload
#define L_INTERNAL_TEMP "Controller Internal Temperature"
#define L_LAST_GENERAL_ERROR "Last General (Background) Error"
#define L_WORKLOAD "Workload"
#define L_WIRELESS_SIGNAL_STRENGTH "Wireless signal strength"

// --------------------------- Control ---------------------------

#if DISPLAY_SPECIAL_CHARS == ON
  #define HOME_CH "&#x1F3E0;"
  #define ARROW_DR "&#x27A5;"
  #define ARROW_UR "&#x27A6;"
  #define ARROW_R2 "&#x27A4;"
  #define CAUTION_CH "&#9888;"
  #define CLOCK_CH "&#x1F565;"
  #define ARROW_LL "&lt;&lt;"
  #define ARROW_L "&lt;"
  #define ARROW_R "&gt;"
  #define ARROW_RR "&gt;&gt;"
  #define ARROW_DD "&lt;&lt;"
  #define ARROW_D "&lt;"
  #define ARROW_U "&gt;"
  #define ARROW_UU "&gt;&gt;"
  #define SIDEREAL_CH "&#9733;"
  #define LUNAR_CH "&#9790;"
  #define SOLAR_CH "&#9737;"
#else
  #define HOME_CH "H"
  #define ARROW_DR "-&gt;"
  #define ARROW_UR "-&gt;"
  #define ARROW_R2 "&gt;"
  #define CAUTION_CH "!"
  #define CLOCK_CH "T"
  #define ARROW_LL "&lt;&lt;"
  #define ARROW_L "&lt;"
  #define ARROW_R "&gt;"
  #define ARROW_RR "&gt;&gt;"
  #define ARROW_DD "&lt;&lt;"
  #define ARROW_D "&lt;"
  #define ARROW_U "&gt;"
  #define ARROW_UU "&gt;&gt;"
  #define SIDEREAL_CH "*"
  #define LUNAR_CH "("
  #define SOLAR_CH "O"
#endif

#define BUTTON_N "N"
#define BUTTON_S "S"
#define BUTTON_E "E"
#define BUTTON_W "W"
#define BUTTON_SYNC "@"

#define L_CENTER_RATE "Cntr"
#define L_FIND_RATE "Find"
#define L_FAST_RATE "Fast"
#define L_VFAST_RATE "VF"

#define L_HINT_CLOCK "Set Date/Time"
#define L_HINT_CONTINUE_GOTO "Continue Goto (when paused at home)"
#define L_HINT_FIND_HOME "Find Home"
#define L_HINT_RESET_HOME "Home (Reset)"
#define L_HINT_UNPARK "Un-Park"
#define L_HINT_PARK "Park"
#define L_HINT_EMERGENCY_STOP "Emergency Stop!"
#define L_HINT_RATE_SIDEREAL "Sidereal Rate"
#define L_HINT_RATE_LUNAR "Lunar Rate"
#define L_HINT_RATE_SOLAR "Solar Rate"
#define L_HINT_FOC_SEL "Select Focuser"
#define L_HINT_FOC_FIND_HOME "Find Focuser Home"
#define L_HINT_FOC_RESET_HOME "Home (Reset) Focuser"
#define L_HINT_ROT_FIND_HOME "Find Rotator Home"
#define L_HINT_ROT_RESET_HOME "Home (Reset) Rotator"
#define L_HINT_DEROTATE_ON "De-Rotate On"
#define L_HINT_ROTATOR_REV "Reverse Rotator Motion"
#define L_HINT_ROTATOR_PAR "Move Rotator to Parallactic angle"
#define L_HINT_DEROTATE_OFF "De-Rotate Off"

#define L_ALIGN "Align"
#define L_AUX_FEATURES "Auxiliary Features"

#define L_FOCUSER "Focuser"
#define L_ROTATOR "Rotator"
#define L_CAMERA_EXPOSURE "Exposure"
#define L_CAMERA_DELAY "Delay"
#define L_CAMERA_COUNT "Count"

// ---------------------------- Library --------------------------

#define L_CATALOG "Catalog"
#define L_CATALOGS "Catalogs"
#define L_RECS_AVAIL "records available"
#define L_NO_CATALOG "No catalog selected"
#define L_CAT_DATA "Data (downloaded OR to be uploaded.)"

#define L_CAT_EXAMPLE1 "Fields:<br /><pre>"
#define L_CAT_EXAMPLE2 "  A Catalog Name record is required for row 1, this<br />"
#define L_CAT_EXAMPLE3 "  consists of a '$' followed by up to 10 characters.<br /><br />"
#define L_CAT_EXAMPLE4 "  Object Name is a sequence of up to 11 characters.<br /><br />"
#define L_CAT_EXAMPLE5 "  Cat is a category code as follows:<br />"
#define L_CAT_EXAMPLE6 "  RA (Jnow) is in Hours, Minutes, and Seconds.<br />"
#define L_CAT_EXAMPLE7 "  Dec (Jnow) is in +/- Degrees, Minutes, Seconds.<br />"

#define L_CAT_NO_NAME "Catalog selected, has no name."
#define L_CAT_GET_NAME_FAIL "Catalog selected, get name failed."
#define L_CAT_NO_CAT "No catalog selected."
#define L_SELECTED "selected"
#define L_CAT_DOWNLOAD_FAIL "Download failed, bad data."
#define L_CAT_DOWNLOAD_SUCCESS "Download success."
#define L_CAT_DOWNLOAD_INDEX_FAIL "Download failed, couldn't index to catalog."
#define L_CAT_DATA_REMOVED "Catalog data removed"
#define L_CAT_UPLOAD_FAIL "Upload failed, bad catalog name."
#define L_CAT_UPLOAD_INDEX_FAIL "Upload failed, indexing catalog name rec."
#define L_CAT_DELETE_FAIL "Upload failed, deleting catalog name rec."
#define L_CAT_WRITE_NAME_FAIL "Upload failed, writing catalog name rec."
#define L_CAT_UPLOAD_NO_NAME_FAIL "Upload failed, line 1 must contain catalog name."
#define L_CAT_BAD_FORM "Upload failed, bad format in line# "
#define L_CAT_UPLOAD_BAD_OBJECT_NAME "Upload failed, bad object name in line# "
#define L_CAT_BAD_CATEGORY "Upload failed, bad category in line# "
#define L_CAT_BAD_RA "Upload failed, bad RA format in line# "
#define L_CAT_BAD_DEC "Upload failed, bad Dec format in line# "
#define L_CAT_UPLOAD_RA_FAIL "Upload failed, sending RA in line# "
#define L_CAT_UPLOAD_DEC_FAIL "Upload failed, sending Dec in line# "
#define L_CAT_UPLOAD_LINE_FAIL "Upload failed, sending line# "
#define L_CAT_UPLOAD_SUCCESS "Upload success"
#define L_CAT_UPLOAD_LINES_WRITTEN "lines written"
#define L_CAT_UPLOAD_SELECT_FAIL "Upload failed, unable to select catalog."
#define L_CAT_UPLOAD_NO_CAT "Upload failed, no catalog selected."
#define L_CAT_CLEAR "Clear Catalog"
#define L_CAT_CLEAR_LIB "Clear Library"

// --------------------------- Encoders --------------------------

// general settings
#define L_ENC_SYNC_TO_ENC "Sync OnStep to Encoders"
#define L_ENC_SYNC_TO_ONS "Sync Encoders to OnStep"
#define L_ENC_ZERO_TO_ONS "Set Absolute Encoder Zero to OnStep"
#define L_ENC_AUTO_SYNC "Automatically sync. OnStep to Encoders"
#define L_ENC_MAX_ANGLE "Max angular distance (Encoders vs. OnStep)"
#define L_ENC_MAX_ANGLE_AXIS1 "(Axis1, arc-sec 0 to 9999)"
#define L_ENC_MAX_ANGLE_AXIS2 "(Axis2, arc-sec 0 to 9999)"
#define L_ENC_AUTO_RATE "OnStep rate control"
#define L_ENC_SYNC_NOW "Now"

// encoder rate control
#define L_ENC_BINNED_SAMPLES "Encoder rate averaging, binned samples"
#define L_ENC_SAMPLES "Encoder rate averaging, samples"
#define L_ENC_PROPORTIONAL "Proportional response"
#define L_ENC_MIN_GUIDE "Minimum guide"
#define L_ENC_RATE_COMP "Encoder rate comp"
#define L_ENC_MAGN "Magnitude"
#define L_ENC_PHASE "Phase"
#define L_ENC_COUNTS "Counts"
#define L_ENC_STAT_RATE_AXIS1 "Axis1 rates (sidereal)"
#define L_ENC_STAT_INTPOL_COMP "Intpol Comp"
#define L_ENC_STAT_INTPOL_PHASE "Intpol Phase"

// ------------------------------ PEC ----------------------------

#define L_PEC_STATUS "PEC Status"
#define L_PEC_CONTROL "PEC Control"
#define L_PEC_PLAY "Play"
#define L_STOP "Stop"
#define L_PEC_CLEAR "Clear"
#define L_PEC_REC "Record"
#define L_PEC_CLEAR_MESSAGE "Clear erases the memory buffer not EEPROM.  During recording corrections are averaged 3:1 favoring the buffer unless cleared in which case the full correction is used."
#define L_PEC_EEWRITE "Write to EEPROM"
#define L_PEC_EEWRITE_MESSAGE "Writes PEC data to EEPROM so it's remembered if OnStep is restarted.  Writing the data can take a few seconds."
#define L_PEC_NO_PEC_MESSAGE "PEC CONTROLS DISABLED"
#define L_PEC_IDLE "Idle"
#define L_PEC_WAIT_PLAY "Play waiting to start"
#define L_PEC_PLAYING "Playing"
#define L_PEC_WAIT_REC "Record waiting for index to arrive"
#define L_PEC_RECORDING "Recording"
#define L_PEC_UNK "Unknown"
#define L_PEC_EEWRITING "writing to EEPROM"

// --------------------------- Settings --------------------------

#define L_REFINE_POLAR_ALIGN "Refine Polar Alignment"
#define L_REFINE_PA "Refine PA"
#define L_REFINE_MESSAGE1 "Setup &amp; 3+ Star align mount. Goto bright star near NCP or SCP with Dec in 50 to 80&deg; range (N or S.) "
#define L_REFINE_MESSAGE2 "Press [Refine PA] button. Use mount's PA adjust controls to center the star again. "
#define L_REFINE_MESSAGE3 "Optionally align the mount again."

#define L_PARK "Park"
#define L_SET_PARK "Set-Park"

#define L_TRK_FASTER "faster"
#define L_TRK_SLOWER "slower"
#define L_TRK_RESET "Reset (default)"
#define L_TRK_COMP "Compensated Tracking Rate (Pointing Model/Refraction)"
#define L_TRK_FULL "Full"
#define L_TRK_REFR "Refraction Only"
#define L_TRK_DUAL "Dual Axis"
#define L_TRK_SINGLE "Single Axis"

#define L_BUZZER "Goto Alert, Buzzer"

#define L_MERIDIAN_FLIP_AUTO "Automatic Meridian Flip at Limit"
#define L_MERIDIAN_FLIP_NOW "Now"
#define L_MERIDIAN_FLIP_PAUSE "Meridian Flip, Pause at Home"

// ------------------------ Configuration ------------------------

#define L_BASIC_SET_TITLE "Basic:"

#define L_MAX_SLEW_SPEED "Maximum Slew Speed"

// goto Rate
#define L_VSLOW "VSlow (0.5x)"
#define L_SLOW "Slow (0.75x)"
#define L_NORMAL "Normal (1x)"
#define L_FAST "Fast (1.5x)"
#define L_VFAST "VFast (2x)"

// backlash
#define L_BACKLASH_RANGE_AXIS1 "Backlash, in arc-seconds 0 to 3600"
#define L_BACKLASH_RANGE_AXIS2 "Backlash, in arc-seconds 0 to 3600"
#define L_BACKLASH_RANGE_AXIS345 "Backlash, in steps 0 to 32767"

// deadband/TCF
#define L_DEADBAND_RANGE_AXIS45 "TCF Deadband, in steps 1 to 32767"
#define L_TCF_COEF_RANGE_AXIS45 "TCF Coef, in microns/deg. C -999.0 to 999.0"
#define L_TCF_COEF_EN_AXIS45 "TCF Enable, 0 false or 1 true"

// limits
#define L_LIMITS_TITLE "Horizon and Overhead Limits"
#define L_LIMITS_RANGE_HORIZON "Horizon, minimum altitude in degrees +/- 30"
#define L_LIMITS_RANGE_OVERHEAD "Overhead, maximum altitude in degrees 60 to 90"
#define L_LIMITS_RANGE_MERIDIAN_E "Past Meridian when East of the pier, in degrees +/- 270"
#define L_LIMITS_RANGE_MERIDIAN_W "Past Meridian when West of the pier, in degrees +/- 270"

// location
#define L_LOCATION_TITLE "Site Latitude, Longitude, UTC Offset"
#define L_LOCATION_LONG "Longitude deg. min. sec. +/- 180, W is +"
#define L_LOCATION_LAT  "Latitude deg. min. sec. +/- 90, N is +"
#define L_LOCATION_RANGE_UTC_OFFSET "UTC Offset, in hours and min. -14 to +12"
#define L_LOCATION_MESSAGE_UTC_OFFSET "Opposite of a time-zone value, this is for Standard Time (not Daylight Time.)"

// advanced configuration
#define L_REVERT "Revert to Defaults"
#define L_ADV_SET_TITLE "Advanced"
#define L_ADV_SET_HEADER_MSG "changes below take effect after rebooting OnStep:"
#define L_ADV_MOUNT_TYPE "Select 1 GEM, 2 EQ Fork, or 3 Alt/Azm"

// reset control
#define L_RESET_TITLE "OnStep Reboot Control:"
#define L_RESET "Reboot OnStep"
#define L_RESET_FWU "Reboot OnStep for firmware upload"
#define L_RESET_MSG1 "OnStep must be At Home or Parked, otherwise the reset request is ignored."

// ----------------------------  WiFi ----------------------------

#define L_WIFI_PERFORMANCE "Performance and compatibility"
#define L_WIFI_CMD_TO "Command channel serial read time-out"
#define L_WIFI_WWW_TO "Web channel serial read time-out"

#define L_WIFI_STA_TITLE "Station mode (connect to an Access-Point)"
#define L_WIFI_PWD "Password"
#define L_WIFI_PWD_MSG "8 char min."
#define L_WIFI_EN_DHCP "Enable DHCP"
#define L_WIFI_EN_DHCP_MSG "Note: above addresses are ignored if DHCP is enabled"
#define L_WIFI_EN_STA "Enable Station Mode"
#define L_IP_ADDRESS "IP Address"
#define L_GATEWAY "Gateway"
#define L_SUBNET "Subnet"
#define L_WIFI_CHA "Channel"
#define L_WIFI_EN_AP_MODE "Enable Access-Point Mode"

#define L_WIFI_CONFIG_SECURITY "WiFi Configuration Security"

#define L_WIFI_AP "Access-Point mode"

#define L_WIFI_RESTART_MSG "You must <u>manually</u> restart for changes to take effect."

#define L_WIFI_TITLE "Enter password to change WiFi configuration:"

#define L_WIFI_ADVICE1 "Setup:<br/><br/>"
#define L_WIFI_ADVICE2 "Enable either Station <b>OR</b> Access Point mode, enabling both can cause performance issues.<br/><br/>"
#define L_WIFI_ADVICE3 "If you want OnStep to connect to your home WiFi network, enable only Station mode and "
#define L_WIFI_ADVICE4 "disable Access Point mode. If OnStep cannot connect to your home WiFi network it will "
#define L_WIFI_ADVICE5 "automatically re-enable Station mode; this is useful when you are away at a dark site and/or "
#define L_WIFI_ADVICE6 "to guard against becoming locked out.<br/><br/>"
#define L_WIFI_ADVICE7 "If locked out of WiFi you can use the Arduino IDE to reset the ESP flash memory (and "
#define L_WIFI_ADVICE8 "hence go back to defaults.) From the Arduino Tools menu, change Erase Flash 'Only Sketch' "
#define L_WIFI_ADVICE9 "to Erase Flash 'All Flash Contents' and upload the WiFi-Bluetooth sketch again."

// --------------------------- Constants -------------------------

#define L_DOWN_TITLE "Serial Interface to OnStep is Down!"
#define L_DOWN_MESSAGE1 "If you just rebooted OnStep please wait for several seconds for everything to startup again, then check to be sure you're still connected to WiFi.<br /><br />"
#define L_DOWN_MESSAGE2 "If this is a persistant problem here are other possible causes:<br /><ul>"
#define L_DOWN_MESSAGE3 "Baud rate mismatch due to reboot of only OnStep, power everything off/on again."
#define L_DOWN_MESSAGE4 "Incorrectly configured Addon baud rate."
#define L_DOWN_MESSAGE5 "Incorrectly configured OnStep baud rate."
#define L_DOWN_MESSAGE6 "Incorrect wiring of the com ports."
#define L_DOWN_MESSAGE7 "Incorrect wiring of power supply pins.  Gnd must be shared between OnStep's MCU and the Addon's MCU."

#define L_PAGE_STATUS "Status"
#define L_PAGE_CONTROL "Control"
#define L_PAGE_AUX "Aux"
#define L_PAGE_LIBRARY "Library"
#define L_PAGE_ENCODERS "Encoders"
#define L_PAGE_PEC "PEC"
#define L_PAGE_SETTINGS "Settings"
#define L_PAGE_CONFIG "Config"
#define L_PAGE_WIFI "WiFi"

// ------------------------- MountStatus -------------------------

// general (background) errors
#define L_GE_NONE "None"
#define L_GE_MOTOR_FAULT "Motor/driver fault"
#define L_GE_ALT_MIN "Below horizon limit" 
#define L_GE_LIMIT_SENSE "Limit sense"
#define L_GE_DEC "Dec limit exceeded"
#define L_GE_AZM "Azm limit exceeded"
#define L_GE_UNDER_POLE "Under pole limit exceeded"
#define L_GE_MERIDIAN "Meridian limit exceeded"
#define L_GE_SYNC "Sync safety limit exceeded"
#define L_GE_PARK "Park failed"
#define L_GE_GOTO_SYNC "Goto sync failed"
#define L_GE_UNSPECIFIED "Unknown error"
#define L_GE_ALT_MAX "Above overhead limit"
#define L_GE_WEATHER_INIT "Weather sensor init failed"
#define L_GE_SITE_INIT "Time or loc. not updated"
#define L_GE_NV_INIT "Init NV/EEPROM bad"
#define L_GE_OTHER "Unknown Error, code"

// command errors
#define L_CE_NONE "No Errors"
#define L_CE_0 "Reply 0"
#define L_CE_CMD_UNKNOWN "command unknown"
#define L_CE_REPLY_UNKNOWN "invalid reply"
#define L_CE_PARAM_RANGE "parameter out of range"
#define L_CE_PARAM_FORM "bad parameter format"
#define L_CE_ALIGN_FAIL "align failed"
#define L_CE_ALIGN_NOT_ACTIVE "align not active"
#define L_CE_NOT_PARKED_OR_AT_HOME "not parked or at home"
#define L_CE_PARKED "already parked"
#define L_CE_PARK_FAILED "park failed"
#define L_CE_NOT_PARKED "not parked"
#define L_CE_NO_PARK_POSITION_SET "no park position set"
#define L_CE_GOTO_FAIL "goto failed"
#define L_CE_LIBRARY_FULL "library full"
#define L_CE_GOTO_ERR_BELOW_HORIZON "goto below horizon"
#define L_CE_GOTO_ERR_ABOVE_OVERHEAD "goto above overhead"
#define L_CE_SLEW_ERR_IN_STANDBY "slew in standby"
#define L_CE_SLEW_ERR_IN_PARK "slew in park"
#define L_CE_GOTO_ERR_GOTO "already in goto"
#define L_CE_GOTO_ERR_OUTSIDE_LIMITS "goto outside limits"
#define L_CE_SLEW_ERR_HARDWARE_FAULT "hardware fault"
#define L_CE_MOUNT_IN_MOTION "mount in motion"
#define L_CE_GOTO_ERR_UNSPECIFIED "other"
#define L_CE_UNK "unknown"
