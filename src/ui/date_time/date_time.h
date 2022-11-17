
#ifndef TIME_H
#define TIME_H

#include <stdint.h>
#include <esp_attr.h>

#include "../../common.h"

#define DATE_TIME_LRG_FONT &Mont_HeavyDEMO25pt7b
#define DATE_TIME_MED_FONT &Code_New_Roman9pt7b

// Time definitions
struct time_struct {
	char wday[4];
	char month[4];
	uint8_t month_num;
	uint8_t mday;
	uint8_t mil_hour;
	uint8_t hour;
	uint8_t min;
	uint8_t sec;
	uint8_t day_offset; // 1st day of the month offset, Monday = 0
	int year;
};

int8_t fetch_date_time(const char *time_zone, uint8_t week_start_offset);
void diplay_day_date_lrg_style1(GxEPD2_GFX& display, uint16_t x, uint16_t y, const char *time_zone, uint8_t week_start_offset = 0);
void display_last_updated_time_med_style1(GxEPD2_GFX& display, uint16_t x, uint16_t y, const char *time_zone, uint8_t week_start_offset = 0);

#endif /* TIME_H */
