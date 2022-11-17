
#include "crypto.h"
#include "crypto_parser.h"

enum CRYPTO_STATE {NOT_FOUND, FOUND_PRICE, FOUND_CHANGE};
static CRYPTO_STATE key_state;

extern char price[CRYPTO_MAX_PRICE_LENGTH_STR];
extern char change[CRYPTO_MAX_CHANGE_LENGTH_STR];

void CryptoJsonListener::whitespace(char c)
{
}

void CryptoJsonListener::startDocument()
{
	key_state = NOT_FOUND;
}

void CryptoJsonListener::key(String key)
{
	if(key.equals("usd")){
		key_state = FOUND_PRICE;
	} else if(key.equals("usd_24h_change")){
		key_state = FOUND_CHANGE;
	}
}

void CryptoJsonListener::value(String value)
{
	if(key_state == FOUND_PRICE){
		strncpy(&price[4], value.c_str(), CRYPTO_MAX_PRICE_LENGTH);
        //price[CRYPTO_MAX_CHANGE_LENGTH_STR] = '\0';
		Serial.printf("Price: %s\r\n", price);
		key_state = NOT_FOUND;
	} else if(key_state == FOUND_CHANGE){
		strncpy(change, value.c_str(), CRYPTO_MAX_CHANGE_LENGTH);
        //change[CRYPTO_MAX_CHANGE_LENGTH_STR] = '\0';
		Serial.printf("Change: %s\r\n", change);
		key_state = NOT_FOUND;
	}
}

void CryptoJsonListener::endArray()
{
}

void CryptoJsonListener::endObject()
{
}

void CryptoJsonListener::endDocument()
{
}

void CryptoJsonListener::startArray()
{
}

void CryptoJsonListener::startObject()
{
}

