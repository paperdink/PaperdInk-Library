#include "weather.h"
#include "weather_parser.h"

enum WEATHER_STATE { NOT_FOUND, FOUND_TIME, FOUND_WEATHER, FOUND_MAIN, COMPLETED };
static WEATHER_STATE key_state;

void WeatherJsonListener::whitespace(char c)
{
}

void WeatherJsonListener::startDocument()
{
	//weather_count = 0;
	key_state = NOT_FOUND;
}

void WeatherJsonListener::key(String key)
{
	switch (key_state) {
	case NOT_FOUND:
		if (key.equals("dt")) {
			//if(weather_count == (FORECAST_HOURS/3)){
			key_state = FOUND_TIME;
			//}
			//weather_count++;
		}
		break;
	case FOUND_TIME:
		if (key.equals("weather"))
			key_state = FOUND_WEATHER;
		break;
	case FOUND_WEATHER:
		if (key.equals("main"))
			key_state = FOUND_MAIN;
		break;
	}
}

void WeatherJsonListener::value(String value)
{
	if (key_state == FOUND_MAIN) {
		DEBUG.printf("Got weather: %s\r\n", value);
		strncpy(Paperdink_Weather.weather_string, (char *)value.c_str(), 10);
		//key_state = NOT_FOUND;
		key_state = COMPLETED;
	}
}

void WeatherJsonListener::endArray()
{
}

void WeatherJsonListener::endObject()
{
}

void WeatherJsonListener::endDocument()
{
}

void WeatherJsonListener::startArray()
{
}

void WeatherJsonListener::startObject()
{
}
