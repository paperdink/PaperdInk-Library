#ifndef _PAPERDINK_WEATHER_H_
#define _PAPERDINK_WEATHER_H_

#include "../../common.h"

#define DEBUG Serial

class PaperdinkUIWeatherClass : public PaperdinkUIBaseClass
{
    public:
        char weather_string[10];

        int8_t fetch_data(const char* city, const char* country, const char* owm_api_id);
        void display_weather_text_center(GxEPD2_GFX& display, uint16_t x, uint16_t y, uint16_t w);
};

extern PaperdinkUIWeatherClass Paperdink_Weather;

#endif /* _PAPERDINK_WEATHER_H_ */
