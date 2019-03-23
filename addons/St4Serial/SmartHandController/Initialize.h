// -----------------------------------------------------------------------------------
// Functions for variables on startup

#define initKey 59301350  // a code to identify the SHC

#define EE_autoInitKey       0
#define EE_reticule          4
#define EE_dispMaxContrast   8
#define EE_dispDimTimeout   12
#define EE_dispBlankTimeout 16

void SmartHandController::initInitNvValues() {
  // EEPROM automatic initialization
  long autoInitKey = initKey;
  long thisAutoInitKey;
  thisAutoInitKey=nv.readLong(EE_autoInitKey);
  if (autoInitKey!=thisAutoInitKey) {

    // stop the init from happening again
    nv.writeLong(EE_autoInitKey,autoInitKey);

    nv.writeLong(EE_dispMaxContrast,(long)maxContrast);
    nv.writeLong(EE_dispDimTimeout,(long)display_dim_time);
    nv.writeLong(EE_dispBlankTimeout,(long)display_blank_time);
  }
}

void SmartHandController::initReadNvValues() {
#ifndef DISABLE_EEPROM_COMMIT_ON
  maxContrast = nv.readLong(EE_dispMaxContrast);
  display_dim_time = nv.readLong(EE_dispDimTimeout);
  display_blank_time = nv.readLong(EE_dispBlankTimeout);
#endif
}
