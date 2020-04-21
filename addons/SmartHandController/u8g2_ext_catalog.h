#pragma once
#include <U8g2lib.h>
#include "u8g2_ext_event.h"

#include "Constants.h"
#include "Locales.h"
#include "Config.h"
#include "Locale.h"

bool ext_UserInterfaceCatalog(u8g2_t *u8g2, Pad *extPad, const char *title);
bool ext_UserInterfaceUserCatalog(u8g2_t *u8g2, Pad *extPad, const char *title);
