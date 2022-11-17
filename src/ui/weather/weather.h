#ifndef _WEATHER_H_
#define _WEATHER_H_

#include "../../common.h"

#define WEATHER_MED_FONT &Code_New_Roman_Bold10pt7b

void display_weather_text(GxEPD2_GFX& display, uint16_t x, uint16_t y,
						  const char* city, const char* country, const char* owm_api_id);

void display_weather_text_center(GxEPD2_GFX& display, uint16_t x, uint16_t y,
								 uint16_t width, const char* city, const char* country, const char* owm_api_id);
#endif /* _WEATHER_H_ */
