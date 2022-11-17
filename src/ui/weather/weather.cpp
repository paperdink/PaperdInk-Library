
#include <WiFiClientSecure.h>
#include <HTTPClient.h>

#include "weather.h"
#include "weather_parser.h"

WeatherJsonListener weather_listener;
static String openweathermap_url = "https://api.openweathermap.org/data/2.5/forecast?q={0},{1}&appid={2}";
char weather_string[10] = "XXXXXX";

int8_t fetch_weather(const char* city, const char* country, const char* owm_api_id)
{
	int httpCode, ret = 0;
	HTTPClient https;
	WiFiClientSecure *client = new WiFiClientSecure;

	String owm_url = openweathermap_url;
	owm_url.replace("{0}", city);
	owm_url.replace("{1}", country);
	owm_url.replace("{2}", owm_api_id);

	https.begin(*client, owm_url.c_str());
	https.addHeader("Content-Type", "application/json", 0, 0);

	httpCode = https.GET();

	if(httpCode == HTTP_CODE_OK){ //Check for the return code
                                  // Parser updates weather_string directly
		ArudinoStreamParser parser;
		parser.setListener(&weather_listener);
		https.writeToStream(&parser);
	} else {
		Serial.println("Error on HTTP request");
		ret = -1;
	}

	https.end();
	return ret;
}

void display_weather_text_abs(GxEPD2_GFX& display, uint16_t x, uint16_t y, const char* city, const char* country, const char* owm_api_id)
{
	int16_t xt, yt;
	uint16_t wt, ht;

	if(fetch_weather(city, country, owm_api_id) < 0){
		return;
	}

	Serial.print("It is ");
	Serial.print(weather_string);
	Serial.printf(" in %s.\r\n", city);

	display.setFont(WEATHER_MED_FONT);
	display.setTextColor(GxEPD_BLACK);
	display.getTextBounds(F("Clouds"), 0, 0, &xt, &yt, &wt, &ht);
	display.setCursor(x, y+ht);
	display.println(weather_string);
}

void display_weather_text_center(GxEPD2_GFX& display, uint16_t x, uint16_t y,
								 uint16_t width, const char* city, const char* country, const char* owm_api_id)
{

	int16_t xt, yt;
	uint16_t wt, ht;

	if(fetch_weather(city, country, owm_api_id) < 0){
		return;
	}

	Serial.print("It is ");
	Serial.print(weather_string);
	Serial.printf(" in %s.\r\n", city);

	display.setFont(WEATHER_MED_FONT);
	display.setTextColor(GxEPD_BLACK);
	display.getTextBounds(F("Clouds"), 0, 0, &xt, &yt, &wt, &ht);
	display.setCursor(x+((width-wt)/2), y+ht);
	display.println(weather_string);

}
