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
#include "youtube_logo.h"
#include "youtube_icon.h"

/* Create Paperdink object */
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

    /* Fetch date data */
    if(Paperdink_Date.fetch_data(TIME_ZONE) < 0){
        DEBUG.println("Unable to fetch Date Time data");
    }
   
    /* Fetch data from Youtube */
	if(Paperdink_Youtube.fetch_data(YOUTUBE_CHANNEL_ID, YOUTUBE_API_ID) < 0){
		DEBUG.println("Unable to initialize youtube");
	}

#ifdef DESIGN_1
    /* Show youtube logo */
    Paperdink.epd.drawBitmap(100, 55, youtube_logo, youtube_logo_width, youtube_logo_height, GxEPD_BLACK);
    if(Paperdink.has_color){
        Paperdink.epd.drawBitmap(100, 55, youtube_logo_red, youtube_logo_width, youtube_logo_height, Paperdink.color);
    }

    /* Print name of the channel */
    Paperdink_UI.display_text_center(Paperdink.epd, 0, 130, YOUTUBE_CHANNEL_NAME, 400, 50, &PAPERDINK_FONT_MED_BOLD);

#elif DESIGN_2
    /* Show youtube icon */
    Paperdink.epd.drawBitmap(50, 60, youtube_icon, youtube_icon_width, youtube_icon_height, GxEPD_BLACK);
    if(Paperdink.has_color){
        Paperdink.epd.drawBitmap(50, 60, youtube_icon_red, youtube_icon_width, youtube_icon_height, Paperdink.color);
    }

    /* Print name of the channel */
    Paperdink_UI.display_text_center(Paperdink.epd, 170, 65, YOUTUBE_CHANNEL_NAME, 0, 0, &PAPERDINK_FONT_MED_BOLD);
    Paperdink_UI.display_text_center(Paperdink.epd, 170, 100, DESIGN_2_STRING, 0, 0, &PAPERDINK_FONT_SML_BOLD);

#endif

    /* Display number of subscribers and views */
    Paperdink_Youtube.font = &PAPERDINK_FONT_MED;
    Paperdink_Youtube.primary_color = GxEPD_BLACK;

    Paperdink_Youtube.display_subscribers_med(Paperdink.epd, 0, 170, 400, 50);
    Paperdink_Youtube.display_views_med(Paperdink.epd, 0, 220, 400, 50);

    /* Send data to display for the update */
	Paperdink.epd.display();

	DEBUG.println("Turning off everything");

    /* Sleep till update time.
     * Align updates to 12am so that date change aligns
     * with actual day change.
     */
	uint64_t sleep_time = (86400/(UPDATES_PER_DAY))-(((Paperdink_Date.mil_hour*3600)+(Paperdink_Date.min*60)+(Paperdink_Date.sec))%(86400/UPDATES_PER_DAY));

    /* Update after sleep_time microsecond or when button 1 is pressed. */
    Paperdink.deep_sleep_timer_wakeup(sleep_time*S_TO_uS_FACTOR); // Consumes lower current
	// Paperdink.deep_sleep_timer_button_wakeup(sleep_time*S_TO_uS_FACTOR, BUTTON_1_PIN); // Consumes higher current
}

void loop()
{

}
