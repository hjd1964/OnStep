// -----------------------------------------------------------------------------------
// Locale

// see Strings_xx.h for individual locale translations

// ISO639-1 language codes (these control the inclusion of individual Strings_xx.h files as specified in Config.h)
#if DISPLAY_LANGUAGE == L_cn
  #include "Strings_cn.h"
  #define UNITS METRIC
#endif
#if DISPLAY_LANGUAGE == L_de
  #include "Strings_de.h"
  #define UNITS METRIC
#endif
#if DISPLAY_LANGUAGE == L_en
  #include "Strings_en.h"
  #define UNITS METRIC
#endif
#if DISPLAY_LANGUAGE == L_us // not ISO639-1 but might be useful
  #include "Strings_en.h"
  #define UNITS IMPERIAL
#endif
#if DISPLAY_LANGUAGE == L_es
  #include "Strings_es.h"
  #define UNITS METRIC
#endif
#if DISPLAY_LANGUAGE == L_fr
  #include "Strings_fr.h"
  #define UNITS METRIC
#endif

// misc. locale support functions
#ifndef DISPLAY_UNITS
  #define DISPLAY_UNITS UNITS
#else
  #if DISPLAY_UNITS == LOCALE_DEFAULT
    #undef DISPLAY_UNITS
    #define DISPLAY_UNITS UNITS
  #endif
#endif

void localeTemperature(char temperatureStr[], char units[]) {
  float t=atof(temperatureStr);
  if (DISPLAY_UNITS == IMPERIAL) {
    t=t*(9.0/5.0)+32.0;
    dtostrf(t,3,1,temperatureStr);
    strcpy(units,"&deg;F");
  } else strcpy(units,"&deg;C");
}

void localePressure(char pressureStr[], char units[]) {
  float p=atof(pressureStr);
  if (DISPLAY_UNITS == IMPERIAL) {
    p=p/33.864;
    dtostrf(p,4,2,pressureStr);
    strcpy(units,"inHg");
  } else strcpy(units,"mb");
}
