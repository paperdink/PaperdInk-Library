#ifndef _PAPERDINK_WEATHER_H_
#define _PAPERDINK_WEATHER_H_

#include "../../paperdink_common.h"

#define DEBUG Serial

class PaperdinkUIWeatherClass : public PaperdinkUIBaseClass
{
    public:
        char city[20];
        char country[20];
        char weather_string[10];
        float day_max_temp;
        float day_min_temp;
        float rain;
        float snow;

        int8_t fetch_data(const char* city, const char* country, const char* owm_api_id, const char* units = "M");
        void display_weather_text_center(GxEPD2_GFX& display, uint16_t x, uint16_t y, uint16_t w);
        void display_weather_style1(GxEPD2_GFX& display, uint16_t x, uint16_t y);
        void display_weather_style2_center(GxEPD2_GFX& display, uint16_t x, uint16_t y, uint16_t w);

};

extern PaperdinkUIWeatherClass Paperdink_Weather;

#endif /* _PAPERDINK_WEATHER_H_ */
