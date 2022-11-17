
#ifndef _CRYPTO_H_
#define _CRYPTO_H_

#include "../../common.h"

#define DEBUG Serial

#ifndef CRYPTO_MAX_PRICE_LENGTH
#define CRYPTO_MAX_PRICE_LENGTH 10 // includes decimal character
#endif

#ifndef CRYPTO_MAX_CHANGE_LENGTH
#define CRYPTO_MAX_CHANGE_LENGTH 4 // includes decimal character
#endif

#define CRYPTO_MAX_PRICE_LENGTH_STR (CRYPTO_MAX_PRICE_LENGTH+5)   // USD 123.456789\0
#define CRYPTO_MAX_CHANGE_LENGTH_STR (CRYPTO_MAX_CHANGE_LENGTH+3) // -3.27%\0

#define CRYPTO_MED_FONT &Code_New_Roman9pt7b
#define CRYPTO_MED_FONT_BOLD &Code_New_Roman_Bold10pt7b

void display_crypto_med_box(GxEPD2_GFX& display, int16_t x, int16_t y, const char *ticker);

#endif /* _CRYPTO_H_ */
