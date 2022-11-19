
#include "date_time.h"

// Time APIs
int8_t PaperdinkUIDateClass::fetch_data(const char *time_zone, uint8_t week_start_offset)
{
	struct tm timeinfo;
	time_t epoch;

    for(uint8_t i = 0; i<CONFIG_TIME_RETRY_COUNT; i++){
        configTime(0, 0, NTP_SERVER);

	    Serial.printf("TZ: %s\r\n", time_zone);
	    setenv("TZ", time_zone, 1);

	    if(!getLocalTime(&timeinfo)){
		    Serial.println("Failed to obtain time. Retrying...");
	        if (i == CONFIG_TIME_RETRY_COUNT){
                return -1;
            }
        } else {
            break;
        }
    }

	epoch = mktime(&timeinfo);

	sscanf(ctime(&epoch), "%s %s %hhd %hhd:%hhd:%hhd %d", wday, month, &mday, &mil_hour, &min, &sec, &year);

	month_num = timeinfo.tm_mon + 1;
    // Gives offset of first day of the month with respect to Monday
    // https://www.tondering.dk/claus/cal/chrweek.php#calcdow
    // 0=Sunday, 1=Monday ... 6=Saturday
	uint16_t a = (14 - month_num) / 12;
	uint16_t y = year - a;
	uint16_t m = month_num + (12 * a) - 2;

	day_offset = (1 + y + (y / 4) - (y / 100) + (y / 400) + ((31 * m) / 12)) % 7;
    // Change the offset based on user preference
	day_offset = (day_offset + week_start_offset) % 7;

    // Convert to 12 hour
	if(mil_hour > 12){
		hour = mil_hour - 12;
	}else{
		hour = mil_hour;
	}

	Serial.printf("Time is %d %d:%d:%d on %s on the %d/%d/%d . It is the month of %s. day_offset: %d\r\n", mil_hour, hour, min, sec, wday, mday, month_num, year, month, day_offset);
	return 0;
}

void PaperdinkUIDateClass::diplay_day_date_lrg_style1(GxEPD2_GFX& display, uint16_t x, uint16_t y)
{
	int16_t xt, yt;
	uint16_t wt, ht, prev_height = 0, prev_width = 0;
	char date_str[7];
	const uint16_t WIDTH = 200;

	display.setFont(font_lrg);
	display.setTextColor(display.epd2.hasColor ? GxEPD_RED : GxEPD_BLACK);
	display.setTextSize(1);

    // Display day
	display.getTextBounds(wday, 0, 0, &xt, &yt, &wt, &ht);
	display.setCursor(x+((WIDTH-wt)/2), y+ht);
	display.println(wday);
	prev_height += y+ht;

    // Display date
	sprintf(date_str, "%d %s", mday, month);
	display.getTextBounds(date_str, 0, 0, &xt, &yt, &wt, &ht);
	display.setCursor(x+((WIDTH-wt)/2), prev_height+ht+20);
	display.println(date_str);
}

void PaperdinkUIDateClass::display_last_updated_time_med_style1(GxEPD2_GFX& display, uint16_t x, uint16_t y)
{
	int16_t xt, yt;
	uint16_t wt, ht, prev_height = 0, prev_width = 0;
	char time_str[5];

	display.setFont(font_sml);
	display.setTextColor(GxEPD_BLACK);
	display.setTextSize(1);

	display.getTextBounds(wday, 0, 0, &xt, &yt, &wt, &ht);
	display.setCursor(x, y+ht);

	sprintf(time_str, "%02d:%02d", mil_hour, min);
	display.printf("Updated at: %s", time_str);
}

PaperdinkUIDateClass Paperdink_Date;
