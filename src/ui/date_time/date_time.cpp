
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
	        delay(1000);
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

void PaperdinkUIDateClass::display_day_date_style1_center(GxEPD2_GFX& display, uint16_t x, uint16_t y, uint16_t w)
{
	int16_t xt, yt;
	uint16_t wt, ht, prev_height = 0, prev_width = 0;
	char date_str[7];

	display.setFont(font);
	display.setTextColor(primary_color);
	display.setTextSize(1);

    // Display day
	display.getTextBounds(wday, 0, 0, &xt, &yt, &wt, &ht);
	display.setCursor(x+((w-wt)/2), y+ht);
	display.println(wday);
	prev_height += y+ht;

    // Display date
	sprintf(date_str, "%d %s", mday, month);
	display.getTextBounds(date_str, 0, 0, &xt, &yt, &wt, &ht);
	display.setCursor(x+((w-wt)/2), prev_height+ht+20);
	display.println(date_str);
}

void PaperdinkUIDateClass::display_day_date_style2_center(GxEPD2_GFX& display, uint16_t x, uint16_t y, uint16_t w)
{
	int16_t xt, yt;
	uint16_t wt, ht, prev_height = 0, prev_width = 0;
    char mday_str[3];

	display.setFont(font);
	display.setTextColor(secondary_color);
	display.setTextSize(1);
    
    // Display day
	sprintf(mday_str, "%02d", Paperdink_Date.mday);
    display.getTextBounds(mday_str, 0, 0, &xt, &yt, &wt, &ht);
    display.setCursor(x+((w-wt)/2), y+ht);
	display.println(mday_str);
    prev_height += y+ht;

    // Display month
	display.getTextBounds(Paperdink_Date.month, 0, 0, &xt, &yt, &wt, &ht);
    display.setCursor(x+((w-wt)/2), prev_height+ht+5);
	display.println(Paperdink_Date.month);
}

void PaperdinkUIDateClass::display_last_updated_time_style1_center(GxEPD2_GFX& display, uint16_t x, uint16_t y, uint16_t w)
{
	int16_t xt, yt;
	uint16_t wt, ht, prev_height = 0, prev_width = 0;
	char time_str[] = "Updated at: XX:XX";

	display.setFont(font);
	display.setTextColor(primary_color);
	display.setTextSize(1);

    sprintf(&time_str[12], "%02d:%02d", mil_hour, min);
	display.getTextBounds(time_str, 0, 0, &xt, &yt, &wt, &ht);
	display.setCursor(x+((w-wt)/2), y+ht);
	display.print(time_str);
}

void PaperdinkUIDateClass::display_calender(GxEPD2_GFX& display, uint16_t x, uint16_t y)
{
	int16_t xt, yt;
	uint16_t wt, ht;

    /* Space between text depends on the font
     * hence font is hardcoded.
     */
	display.setFont(&Gobold_Thin9pt7b);
	display.setTextSize(1);
    display.setTextColor(primary_color);

#define CAL_STRING "Mon   Tue   Wed   Thu   Fri   Sat   Sun"

	display.setCursor(x, y);
	display.print(CAL_STRING);
	display.getTextBounds(CAL_STRING, 0, 0, &xt, &yt, &wt, &ht);
	uint8_t num_offset, print_valid = 0;
	uint8_t day = 1;

	for(uint8_t j = 0; j <= 5; j++){
		for(uint8_t i = 1; i <= 7 && day <= 31; i++){
            // you can hack around with this value to align your text properly based on what font, font size etc you are using
			num_offset = 21;            // 21 is what works for me for the first 2 columns
			if(i >= 3 && i <= 7){
				num_offset = 17;        // then I need to reduce to 17
			}
			if(j == 0 && ((i - 1) == Paperdink_Date.day_offset || Paperdink_Date.day_offset == 0)){
                // start from the offset in the month, ie which day does 1st of the month lie on
				print_valid = 1;
			}
			if(print_valid){
				display.setCursor(x + (i * (wt / 7)) - num_offset, y + ((j + 1) * ht) + ((j + 1) * 7));
				if(day == Paperdink_Date.mday){
					char str[3];
					sprintf(str, "%d", day);
					int16_t x2, y2;
					uint16_t w2, h2;
					display.getTextBounds(str, x + (i * (wt / 7)) - num_offset, y + ((j + 1) * ht) + ((j + 1) * 7), &x2, &y2, &w2, &h2);
					display.fillRect(x2 - 4, y2 - 4, w2 + 8, h2 + 8, primary_color);
					display.setTextColor(secondary_color);
				} else{
					display.setTextColor(primary_color);
				}
                // once the offset is reached, start incrementing
				display.println(day);
				day += 1;
			}
		}
	}
}

PaperdinkUIDateClass Paperdink_Date;
