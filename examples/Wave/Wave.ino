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
#include <SPIFFS.h>

/* Store image_counter in RTC memory
 * for persistence across deep sleep.
 */
RTC_DATA_ATTR uint8_t image_counter;

/* Create paperdink object */
PAPERDINK_DEVICE Paperdink;

void setup()
{
    char img_string[7];

    DEBUG.begin(115200);

    /* Initialize paperd.ink device */
    Paperdink.begin();

    /* Intialize on chip SPI file system */
    DEBUG.print("Initializing SPIFFS...");
    if(!SPIFFS.begin(true)){
        DEBUG.println("Failed!");
    }else{
        DEBUG.println("Success!");
    }
    delay(100);

    /* Enable power to the display */
    Paperdink.enable_display();
    /* Clear the background */
    Paperdink.epd.fillScreen(GxEPD_WHITE);

    /* Connect to wifi network */
    if(Paperdink.connect_wifi(SSID, PASSWORD) < 0){
        DEBUG.println("Unable to connect to WiFi");
        Paperdink.epd.drawBitmap(370, 4, wifi_off_sml, wifi_off_sml_width, wifi_off_sml_height, GxEPD_BLACK);
    }

    /* Show background image */
    sprintf(img_string, "%d.bmp", (image_counter++%NUM_IMAGES)+1);
    Paperdink_UI.display_bitmap_fs(Paperdink.epd, SPIFFS, img_string, 0, 0, Paperdink.has_color);

    /* Create white box to display tasks */
    Paperdink.epd.fillRect(20, 25, 140, 250, GxEPD_WHITE);
    Paperdink.epd.drawRect(20, 25, 140, 250, GxEPD_BLACK);

    /* Fetch and display Todo list */
#ifdef TODOIST
    Paperdink_TodoListTodoist.font_bold = &Gobold_Thin9pt7b;
    Paperdink_TodoListTodoist.font = &Mont_ExtraLight8pt7b;
    Paperdink_TodoListTodoist.primary_color = GxEPD_BLACK;

    if(Paperdink_TodoListTodoist.fetch_data(TODOIST_TOKEN) < 0 ){
        DEBUG.println("Unable to fetch to-do list from Todoist");
    }
    Paperdink_TodoListTodoist.display_list_style1(Paperdink.epd, 20, 30, 12, 1, 135);

#elif TAIGA
    Paperdink_TodoListTaiga.font_bold = &Gobold_Thin9pt7b;
    Paperdink_TodoListTaiga.font = &Mont_ExtraLight8pt7b;
    Paperdink_TodoListTaiga.primary_color = GxEPD_BLACK;

    if(Paperdink_TodoListTaiga.fetch_data(TAIGA_PROJECT_NAME, TAIGA_PROJECT_USERNAME, TAIGA_PROJECT_PASSWORD) < 0 ){
        DEBUG.println("Unable to fetch to-do list from Taiga");
    }
    Paperdink_TodoListTaiga.display_list_style1(Paperdink.epd, 20, 30, 12, 1, 135);

#endif

    /* Fetch date data */
    if(Paperdink_Date.fetch_data(TIME_ZONE) < 0){
        DEBUG.println("Unable to fetch Date Time data");
    }
    /* Create white box to display time and weather */
    Paperdink.epd.fillRect(180, 50, 200, 200, GxEPD_WHITE);
    Paperdink.epd.drawRect(180, 50, 200, 200, GxEPD_BLACK);

    /* Display date */
    Paperdink_Date.font = &PAPERDINK_FONT_LRG;
    Paperdink_Date.display_day_date_style1_center(Paperdink.epd, 180, 65, 200);

    /* Fetch weather data*/
    if(Paperdink_Weather.fetch_data(CITY, COUNTRY, OWM_ID) < 0){
        DEBUG.println("Unable to fetch weather data");
    }

    /* Display weather text */
    Paperdink_Weather.font = &Gobold_Thin9pt7b;
    Paperdink_Weather.primary_color = GxEPD_BLACK;
    Paperdink_Weather.display_weather_text_center(Paperdink.epd, 180, 200, 200);

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
