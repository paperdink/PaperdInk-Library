
#include <WiFiClientSecure.h>
#include <HTTPClient.h>

#include "crypto.h"
#include "crypto_parser.h"

static String url = "https://api.coingecko.com/api/v3/simple/price?vs_currencies=USD&include_24hr_change=true&precision=5&ids=";
CryptoJsonListener crypto_listener;

// Valid till 3 July 2023
static const char* cert = "-----BEGIN CERTIFICATE-----\n" \
"MIIDdzCCAl+gAwIBAgIEAgAAuTANBgkqhkiG9w0BAQUFADBaMQswCQYDVQQGEwJJ\n" \
"RTESMBAGA1UEChMJQmFsdGltb3JlMRMwEQYDVQQLEwpDeWJlclRydXN0MSIwIAYD\n" \
"VQQDExlCYWx0aW1vcmUgQ3liZXJUcnVzdCBSb290MB4XDTAwMDUxMjE4NDYwMFoX\n" \
"DTI1MDUxMjIzNTkwMFowWjELMAkGA1UEBhMCSUUxEjAQBgNVBAoTCUJhbHRpbW9y\n" \
"ZTETMBEGA1UECxMKQ3liZXJUcnVzdDEiMCAGA1UEAxMZQmFsdGltb3JlIEN5YmVy\n" \
"VHJ1c3QgUm9vdDCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAKMEuyKr\n" \
"mD1X6CZymrV51Cni4eiVgLGw41uOKymaZN+hXe2wCQVt2yguzmKiYv60iNoS6zjr\n" \
"IZ3AQSsBUnuId9Mcj8e6uYi1agnnc+gRQKfRzMpijS3ljwumUNKoUMMo6vWrJYeK\n" \
"mpYcqWe4PwzV9/lSEy/CG9VwcPCPwBLKBsua4dnKM3p31vjsufFoREJIE9LAwqSu\n" \
"XmD+tqYF/LTdB1kC1FkYmGP1pWPgkAx9XbIGevOF6uvUA65ehD5f/xXtabz5OTZy\n" \
"dc93Uk3zyZAsuT3lySNTPx8kmCFcB5kpvcY67Oduhjprl3RjM71oGDHweI12v/ye\n" \
"jl0qhqdNkNwnGjkCAwEAAaNFMEMwHQYDVR0OBBYEFOWdWTCCR1jMrPoIVDaGezq1\n" \
"BE3wMBIGA1UdEwEB/wQIMAYBAf8CAQMwDgYDVR0PAQH/BAQDAgEGMA0GCSqGSIb3\n" \
"DQEBBQUAA4IBAQCFDF2O5G9RaEIFoN27TyclhAO992T9Ldcw46QQF+vaKSm2eT92\n" \
"9hkTI7gQCvlYpNRhcL0EYWoSihfVCr3FvDB81ukMJY2GQE/szKN+OMY3EU/t3Wgx\n" \
"jkzSswF07r51XgdIGn9w/xZchMB5hbgF/X++ZRGjD8ACtPhSNzkE1akxehi/oCr0\n" \
"Epn3o0WC4zxe9Z2etciefC7IpJ5OCBRLbf1wbWsaY71k5h+3zvDyny67G7fyUIhz\n" \
"ksLi4xaNmjICq44Y3ekQEe5+NauQrz4wlHrQMz2nZQ/1/I6eYs9HRCwBXbsdtTLS\n" \
"R9I4LtD+gdwyah617jzV/OeBHRnDJELqYzmp\n" \
"-----END CERTIFICATE-----\n";

int8_t PaperdinkUICryptoClass::fetch_data(const char *ticker)
{
	int httpCode, ret = 0;
	String payload;
	WiFiClientSecure *client = new WiFiClientSecure;
    client->setCACert(cert);
    
    {
        HTTPClient https;

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
    }

    delete client;
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

	display.setFont(font_bold);
	display.setTextColor(primary_color);
	display.setTextSize(1);

    /* Display ticker string */
	display.getTextBounds(F("Bitcoin"), 0, 0, &xt, &yt, &wt, &ht);
	display.setCursor(x+14, y+ht+3);
	display.print(ticker);

	display.setFont(font);
    /* Display background */
	display.getTextBounds(dummy_price, 0, 0, &xt, &yt, &wt, &ht);
	display.fillRoundRect(x+5, y+30, wt+20, ht+10, ht+10, secondary_color);
	prev_width += x+5+wt+20;
	prev_height += y+30;

    /* Display current price */
	display.setTextColor(tertiary_color);
	display.getTextBounds(price, 0, 0, &xt, &yt, &wt, &ht);
	display.setCursor(5+((x+prev_width-wt)/2), prev_height+ht+4);
	display.print(price);
	prev_width += 4;
	prev_height += -2;

    /* Display arrow */
	const uint8_t *arrow = up_arrow_sml;
	uint16_t color = primary_color;
	if(change[0] == '-'){
		arrow = down_arrow_sml;
		if(display.epd2.hasColor){
			color = secondary_color;
		}
	}
	display.drawBitmap(prev_width, prev_height, arrow, 24, 24, color);
	prev_width += 24; // Size of bitmap

    // Display percentage change
	display.setTextColor(primary_color);
	display.getTextBounds(change, 0, 0, &xt, &yt, &wt, &ht);
	display.setCursor(prev_width-wt, y+ht+3);
	display.print(change);
}

PaperdinkUICryptoClass Paperdink_Crypto;
