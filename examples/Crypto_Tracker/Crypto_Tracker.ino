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

    /* Fetch and display cryptocurrency data */
    /* TODO: Put in some kind of loop */
    Paperdink_Crypto.font = &PAPERDINK_FONT_SML;
    Paperdink_Crypto.font_bold = &PAPERDINK_FONT_SML_BOLD;
    Paperdink_Crypto.primary_color = GxEPD_BLACK;
    Paperdink_Crypto.secondary_color = Paperdink.has_color ? GxEPD_RED : GxEPD_BLACK;
    Paperdink_Crypto.tertiary_color = GxEPD_WHITE;

    if(Paperdink_Crypto.fetch_data(CRYPTO_TICKER_1) < 0){
        DEBUG.println("Unable to fetch data for " CRYPTO_TICKER_1);
    }
	Paperdink_Crypto.display_med_box(Paperdink.epd, 0, 15, CRYPTO_TICKER_1);
	
    if(Paperdink_Crypto.fetch_data(CRYPTO_TICKER_2) < 0){
        DEBUG.println("Unable to fetch data for " CRYPTO_TICKER_2);
    }
    Paperdink_Crypto.display_med_box(Paperdink.epd, 0, 85, CRYPTO_TICKER_2);
	
    if(Paperdink_Crypto.fetch_data(CRYPTO_TICKER_3) < 0){
        DEBUG.println("Unable to fetch data for " CRYPTO_TICKER_3);
    }
    Paperdink_Crypto.display_med_box(Paperdink.epd, 0, 155, CRYPTO_TICKER_3);

    if(Paperdink_Crypto.fetch_data(CRYPTO_TICKER_4) < 0){
        DEBUG.println("Unable to fetch data for " CRYPTO_TICKER_4);
    }
    Paperdink_Crypto.display_med_box(Paperdink.epd, 0, 225, CRYPTO_TICKER_4);
	
    if(Paperdink_Crypto.fetch_data(CRYPTO_TICKER_5) < 0){
        DEBUG.println("Unable to fetch data for " CRYPTO_TICKER_5);
    }
    Paperdink_Crypto.display_med_box(Paperdink.epd, 200, 155, CRYPTO_TICKER_5);
	
    if(Paperdink_Crypto.fetch_data(CRYPTO_TICKER_6) < 0){
        DEBUG.println("Unable to fetch data for " CRYPTO_TICKER_6);
    }
    Paperdink_Crypto.display_med_box(Paperdink.epd, 200, 225, CRYPTO_TICKER_6);

    /* Fetch date data */
    if(Paperdink_Date.fetch_data(TIME_ZONE) < 0){
        DEBUG.println("Unable to fetch Date Time data");
    }
    /* Display date */
    Paperdink_Date.font = &PAPERDINK_FONT_LRG;
    Paperdink_Date.primary_color = Paperdink.color;
	Paperdink_Date.display_day_date_style1_center(Paperdink.epd, 200, 20, 200);

    /* Display last updated time */
	Paperdink_Date.font = &PAPERDINK_FONT_SML;
    Paperdink_Date.primary_color = GxEPD_BLACK;
    Paperdink_Date.display_last_updated_time_style1_center(Paperdink.epd, 200, 125, 200);

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
