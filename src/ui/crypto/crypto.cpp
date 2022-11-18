
#include <WiFiClientSecure.h>
#include <HTTPClient.h>

#include "crypto.h"
#include "crypto_parser.h"

static String url = "https://api.coingecko.com/api/v3/simple/price?vs_currencies=USD&include_24hr_change=true&precision=5&ids=";
CryptoJsonListener crypto_listener;

int8_t PaperdinkUICryptoClass::fetch_data(const char *ticker)
{
	int httpCode, ret = 0;
	String payload;
	HTTPClient https;
	WiFiClientSecure *client = new WiFiClientSecure;

	String get_url = url + ticker;

	https.begin(*client, get_url.c_str());
	https.addHeader("Content-Type", "application/json", 0, 0);

	httpCode = https.GET();

	if(httpCode == HTTP_CODE_OK){
        // HTTP header has been send and Server response header has been handled
		DEBUG.printf("[HTTP] GET SUCCESS\r\n");
        //String payload = https.getString();
        //Serial.println(payload);
		ArudinoStreamParser parser;
		parser.setListener(&crypto_listener);
		https.writeToStream(&parser);
	} else {
		DEBUG.printf("[HTTP] Failed, error: %s\r\n", https.errorToString(httpCode).c_str());
		ret = -1;
	}

	https.end();

	DEBUG.printf("[HTTP] COMPLETED \r\n");
	return ret;
}

void PaperdinkUICryptoClass::display_med_box(GxEPD2_GFX& display, int16_t x, int16_t y, const char *ticker)
{
	int16_t xt, yt;
	uint16_t wt, ht, prev_height = 0, prev_width = 0;
	const int16_t block_width = 100;
	const int16_t block_height = 50;

    //const char ticker[] = "Bitcoin";
	const char dummy_price[] = "USD 7.77777777"; // decides width of the box
    //const char change[] = "1.32%";

	display.setFont(font_med_bold);
	display.setTextColor(GxEPD_BLACK);
	display.setTextSize(1);

    /* Display ticker string */
	display.getTextBounds(F("Bitcoin"), 0, 0, &xt, &yt, &wt, &ht);
	display.setCursor(x+14, y+ht+3);
	display.print(ticker);

	display.setFont(font_med);
    /* Display background */
	display.getTextBounds(dummy_price, 0, 0, &xt, &yt, &wt, &ht);
	display.fillRoundRect(x+5, y+30, wt+20, ht+10, ht+10, display.epd2.hasColor ? GxEPD_RED : GxEPD_BLACK);
	prev_width += x+5+wt+20;
	prev_height += y+30;

    /* Display current price */
	display.setTextColor(GxEPD_WHITE);
	display.getTextBounds(price, 0, 0, &xt, &yt, &wt, &ht);
	display.setCursor(5+((x+prev_width-wt)/2), prev_height+ht+4);
	display.print(price);
	prev_width += 4;
	prev_height += -2;

    /* Display arrow */
	const uint8_t *arrow = up_arrow_sml;
	uint16_t color = GxEPD_BLACK;
	if(change[0] == '-'){
		arrow = down_arrow_sml;
		if(display.epd2.hasColor){
			color = GxEPD_RED;
		}
	}
	display.drawBitmap(prev_width, prev_height, arrow, 24, 24, color);
	prev_width += 24; // Size of bitmap

    // Display percentage change
	display.setTextColor(GxEPD_BLACK);
	display.getTextBounds(change, 0, 0, &xt, &yt, &wt, &ht);
	display.setCursor(prev_width-wt, y+ht+3);
	display.print(change);
}

PaperdinkUICryptoClass Paperdink_Crypto;
