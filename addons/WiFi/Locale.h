// -----------------------------------------------------------------------------------
// Locale

// see Strings_xx.h for individual locale translations

// ISO639-1 language codes (these control the inclusion of individual Strings_xx.h files as specified in Config.h)
#if DISPLAY_LANGUAGE == L_de
  #include "Strings_de.h"
#endif
#if DISPLAY_LANGUAGE == L_en
  #include "Strings_en.h"
#endif
#if DISPLAY_LANGUAGE == L_es
  #include "Strings_es.h"
#endif
#if DISPLAY_LANGUAGE == L_fr
  #include "Strings_fr.h"
#endif
