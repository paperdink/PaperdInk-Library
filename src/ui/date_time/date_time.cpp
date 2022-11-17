
#include "date_time.h"

RTC_DATA_ATTR struct time_struct pdink_now; // keep track of time

// Time APIs
int8_t fetch_date_time(const char *time_zone, uint8_t week_start_offset)
{
	struct tm timeinfo;
	time_t epoch;

	Serial.printf("TZ: %s\r\n", time_zone);
	setenv("TZ", time_zone, 1);

	if(!getLocalTime(&timeinfo)){
		Serial.println("Failed to obtain time");
		return -1;
	}
	epoch = mktime(&timeinfo);

	sscanf(ctime(&epoch), "%s %s %hhd %hhd:%hhd:%hhd %d", pdink_now.wday, pdink_now.month, &pdink_now.mday, &pdink_now.mil_hour, &pdink_now.min, &pdink_now.sec, &pdink_now.year);

	pdink_now.month_num = timeinfo.tm_mon + 1;
    // Gives offset of first day of the month with respect to Monday
    // https://www.tondering.dk/claus/cal/chrweek.php#calcdow
    // 0=Sunday, 1=Monday ... 6=Saturday
	uint16_t a = (14 - pdink_now.month_num) / 12;
	uint16_t y = pdink_now.year - a;
	uint16_t m = pdink_now.month_num + (12 * a) - 2;

	pdink_now.day_offset = (1 + y + (y / 4) - (y / 100) + (y / 400) + ((31 * m) / 12)) % 7;
    // Change the offset based on user preference
	pdink_now.day_offset = (pdink_now.day_offset + week_start_offset) % 7;

    // Convert to 12 hour
	if(pdink_now.mil_hour > 12){
		pdink_now.hour = pdink_now.mil_hour - 12;
	}else{
		pdink_now.hour = pdink_now.mil_hour;
	}

	Serial.printf("Time is %d %d:%d:%d on %s on the %d/%d/%d . It is the month of %s. day_offset: %d\r\n", pdink_now.mil_hour, pdink_now.hour, pdink_now.min, pdink_now.sec, pdink_now.wday, pdink_now.mday, pdink_now.month_num, pdink_now.year, pdink_now.month, pdink_now.day_offset);
	return 0;
}

void diplay_day_date_lrg_style1(GxEPD2_GFX& display, uint16_t x, uint16_t y, const char *time_zone, uint8_t week_start_offset)
{
	int16_t xt, yt;
	uint16_t wt, ht, prev_height = 0, prev_width = 0;
	char date_str[7];
	const uint16_t WIDTH = 200;

	configTime(0, 0, "pool.ntp.org");
	if(fetch_date_time(time_zone, week_start_offset) < 0){
		configTime(0, 0, "pool.ntp.org");
	}

	display.setFont(DATE_TIME_LRG_FONT);
	display.setTextColor(display.epd2.hasColor ? GxEPD_RED : GxEPD_BLACK);
	display.setTextSize(1);

    // Display day
	display.getTextBounds(pdink_now.wday, 0, 0, &xt, &yt, &wt, &ht);
	display.setCursor(x+((WIDTH-wt)/2), y+ht);
	display.println(pdink_now.wday);
	prev_height += y+ht;

    // Display date
	sprintf(date_str, "%d %s", pdink_now.mday, pdink_now.month);
	display.getTextBounds(date_str, 0, 0, &xt, &yt, &wt, &ht);
	display.setCursor(x+((WIDTH-wt)/2), prev_height+ht+20);
	display.println(date_str);
}

void display_last_updated_time_med_style1(GxEPD2_GFX& display, uint16_t x, uint16_t y, const char *time_zone, uint8_t week_start_offset)
{
	int16_t xt, yt;
	uint16_t wt, ht, prev_height = 0, prev_width = 0;
	char time_str[5];

	configTime(0, 0, "pool.ntp.org");
	if(fetch_date_time(time_zone, week_start_offset) < 0){
		configTime(0, 0, "pool.ntp.org");
	}

	display.setFont(DATE_TIME_MED_FONT);
	display.setTextColor(GxEPD_BLACK);
	display.setTextSize(1);

	display.getTextBounds(pdink_now.wday, 0, 0, &xt, &yt, &wt, &ht);
	display.setCursor(x, y+ht);

	sprintf(time_str, "%02d:%02d", pdink_now.mil_hour, pdink_now.min);
	display.printf("Updated at: %s", time_str);
}
