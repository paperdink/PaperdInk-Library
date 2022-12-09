/* The MIT License (MIT)
 * Copyright (c) 2022 Paperd.Ink

 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/* config.h should be before Paperdink.h */
#include "config.h"
#include <Paperdink.h>

/* Create paperdink object */
PAPERDINK_DEVICE Paperdink;

void setup()
{
	DEBUG.begin(115200);

    /* Initialize paperd.ink device */
	Paperdink.begin();
    
    /* Enable power to the display */
	Paperdink.enable_display();
    /* Clear the background */
	Paperdink.epd.fillScreen(GxEPD_WHITE);

    /* Connect to wifi network */
	if(Paperdink.connect_wifi(SSID, PASSWORD) < 0){
        DEBUG.println("Unable to connect to WiFi");
        Paperdink.epd.drawBitmap(370, 4, wifi_off_sml, wifi_off_sml_width, wifi_off_sml_height, GxEPD_BLACK);
    }

    /* Display background */
	Paperdink.epd.fillRect(  0, 200, 400, 100, Paperdink.color); // bottom colored rect
	Paperdink.epd.fillRect(115,   0,   5, 200, Paperdink.color); // top colored line
	Paperdink.epd.fillRect(115, 200,   5, 100, GxEPD_WHITE);     // bottom colored line

    /* Fetch date data */
    if(Paperdink_Date.fetch_data(TIME_ZONE) < 0){
        DEBUG.println("Unable to fetch Date Time data");
    }
    
    /* Display calender */
    Paperdink_Date.display_calender(Paperdink.epd, 125, 40); 

    Paperdink_Date.font = &PAPERDINK_FONT_LRG;
    /* Display day and month */
    Paperdink_Date.display_day_date_style2_center(Paperdink.epd, 0, 205, 110);

    /* Fetch weather data*/
    if(Paperdink_Weather.fetch_data(CITY, COUNTRY, OWM_ID, UNITS) < 0){
        DEBUG.println("Unable to fetch weather data");
    }

    /* Display weather information */
    Paperdink_Weather.font = &Gobold_Thin9pt7b;
    Paperdink_Weather.display_weather_style1(Paperdink.epd, 5, 22);
    Paperdink_Weather.display_weather_style2_center(Paperdink.epd, 0, 130, 110);

    /* Fetch and display Todo list */
#ifdef TODOIST
    Paperdink_TodoListTodoist.font_bold = &PAPERDINK_FONT_SML_BOLD;
    Paperdink_TodoListTodoist.font = &PAPERDINK_FONT_SML;
    Paperdink_TodoListTodoist.primary_color = GxEPD_WHITE;

    if(Paperdink_TodoListTodoist.fetch_data(TODOIST_TOKEN) < 0 ){
        DEBUG.println("Unable to fetch to-do list from Todoist");
    }
    Paperdink_TodoListTodoist.display_list_style1(Paperdink.epd, 123, 205, 4, 2, 125);

#elif TAIGA
    Paperdink_TodoListTaiga.font_bold = &PAPERDINK_FONT_SML_BOLD;
    Paperdink_TodoListTaiga.font = &PAPERDINK_FONT_SML;
    Paperdink_TodoListTaiga.primary_color = GxEPD_WHITE;

    if(Paperdink_TodoListTaiga.fetch_data(TAIGA_PROJECT_NAME, TAIGA_PROJECT_USERNAME, TAIGA_PROJECT_PASSWORD) < 0 ){
        DEBUG.println("Unable to fetch to-do list from Taiga");
    }
    Paperdink_TodoListTaiga.display_list_style1(Paperdink.epd, 123, 205, 4, 2, 125);

#endif

    /* Send data to display for the update */ 
	Paperdink.epd.display();

	DEBUG.println("Turning off everything");

    /* Sleep till update time.
     * Align updates to 12am so that date change aligns
     * with actual day change.
     */
	uint64_t sleep_time = (86400/(UPDATES_PER_DAY))-(((Paperdink_Date.mil_hour*3600)+(Paperdink_Date.min*60)+(Paperdink_Date.sec))%(86400/UPDATES_PER_DAY));

    /* Update after sleep_time microsecond or when button 1 is pressed. */
    // Paperdink.deep_sleep_timer_wakeup(sleep_time*S_TO_uS_FACTOR); // Consumes lower current
	Paperdink.deep_sleep_timer_button_wakeup(sleep_time*S_TO_uS_FACTOR, BUTTON_1_PIN); // Consumes higher current
}

void loop()
{

}
