
#ifndef _PAPERDINK_CRYPTO_H_
#define _PAPERDINK_CRYPTO_H_

#include "../../paperdink_common.h"

#define DEBUG Serial

#ifndef CRYPTO_MAX_PRICE_LENGTH
#define CRYPTO_MAX_PRICE_LENGTH 10 // includes decimal character
#endif

#ifndef CRYPTO_MAX_CHANGE_LENGTH
#define CRYPTO_MAX_CHANGE_LENGTH 4 // includes decimal character
#endif

#define CRYPTO_MAX_PRICE_LENGTH_STR (CRYPTO_MAX_PRICE_LENGTH+5)   // USD 123.456789\0
#define CRYPTO_MAX_CHANGE_LENGTH_STR (CRYPTO_MAX_CHANGE_LENGTH+3) // -3.27%\0

class PaperdinkUICryptoClass : public PaperdinkUIBaseClass
{
    public:
        char price[CRYPTO_MAX_PRICE_LENGTH_STR] = "USD XXX";
        char change[CRYPTO_MAX_CHANGE_LENGTH_STR] = "XXXX%";

        int8_t fetch_data(const char *ticker);
        
        void display_med_box(GxEPD2_GFX& display, int16_t x, int16_t y, const char *ticker);
};

extern PaperdinkUICryptoClass Paperdink_Crypto;

#endif /* _PAPERDINK_CRYPTO_H_ */
