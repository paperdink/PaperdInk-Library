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

/* config.h should be before paperdink.h */
#include "config.h"
#include <paperdink.h>
#include "youtube_logo.h"

/* Create Paperdink object */
PAPERDINK_DEVICE Paperdink;

void setup()
{
	DEBUG.begin(115200);

    /** Initialize paperd.ink device */
	Paperdink.begin();
    
    /** Enable power to the display */
	Paperdink.enable_display();

    /** Connect to wifi network */
	Paperdink.connect_wifi(SSID, PASSWORD);

    /** Clear the background */
	Paperdink.epd.fillScreen(GxEPD_WHITE);
   
    /** Fetch data from Youtube */
	if(Paperdink_Youtube.fetch_data(YOUTUBE_CHANNEL_ID, YOUTUBE_API_ID) < 0){
		Serial.println("Unable to initialize youtube");
        return;
	}

    /* Show youtube logo */
    Paperdink.epd.drawBitmap(100, 60, youtube_logo, youtube_logo_width, youtube_logo_height, GxEPD_BLACK);
    if(Paperdink.epd.epd2.hasColor){
        Paperdink.epd.drawBitmap(100, 60, youtube_logo_red, youtube_logo_width, youtube_logo_height, GxEPD_RED);
    }

    /* Print name of the channel */
    Paperdink_UI.display_text(Paperdink.epd, 150, 125, YOUTUBE_CHANNEL_NAME, 100, 50, &Code_New_Roman_Bold10pt7b);

    /* Display number of subscribers and views */
    Paperdink_Youtube.display_subscribers_sml(Paperdink.epd, 150, 160, 100, 50);
    Paperdink_Youtube.display_views_sml(Paperdink.epd, 150, 200, 100, 50);

    /* Update the actual display */
	Paperdink.epd.display();

	DEBUG.println("Turning off everything");

    /** Sleep till update time.
     * Align updates to 12am so that date change aligns
     * with actual day change.
     */
	uint64_t sleep_time = (86400/(UPDATES_PER_DAY))-(((pdink_now.mil_hour*3600)+(pdink_now.min*60)+(pdink_now.sec))%(86400/UPDATES_PER_DAY));

    //** Update after sleep_time microsecond or when button 1 is pressed. */
    // Paperdink.deep_sleep_timer_wakeup(sleep_time*S_TO_uS_FACTOR); // Consumes lower current
	Paperdink.deep_sleep_timer_button_wakeup(sleep_time*S_TO_uS_FACTOR, BUTTON_1_PIN); // Consumes higher current
}

void loop()
{

}
