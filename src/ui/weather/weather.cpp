
#include <WiFiClientSecure.h>
#include <HTTPClient.h>

#include "weather.h"
#include "weather_parser.h"

WeatherJsonListener weather_listener;
static String openweathermap_url = "https://api.openweathermap.org/data/2.5/forecast?q={0},{1}&appid={2}";

int8_t PaperdinkUIWeatherClass::fetch_data(const char* city, const char* country, const char* owm_api_id)
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
		DEBUG.printf("[HTTP] GET SUCCESS\r\n");
        ArudinoStreamParser parser;
		parser.setListener(&weather_listener);
		https.writeToStream(&parser);
	} else {
        DEBUG.printf("[HTTP] Failed, error: %s\r\n", https.errorToString(httpCode).c_str());
		ret = -1;
	}

	https.end();
    DEBUG.printf("[HTTP] COMPLETED \r\n");
	return ret;
}

void PaperdinkUIWeatherClass::display_weather_text_center(GxEPD2_GFX& display, uint16_t x, uint16_t y, uint16_t w)
{
	int16_t xt, yt;
	uint16_t wt, ht;

	display.setFont(font);
	display.setTextColor(GxEPD_BLACK);
	display.getTextBounds(Paperdink_Weather.weather_string, 0, 0, &xt, &yt, &wt, &ht);
	display.setCursor(x+((w-wt)/2), y+ht);
	display.print(weather_string);
}

PaperdinkUIWeatherClass Paperdink_Weather;

