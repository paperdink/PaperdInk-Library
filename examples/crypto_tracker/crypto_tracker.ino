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

// config.h should be before paperdink.h
#include "config.h"
#include <paperdink.h>

// Create paperdink object
PDINK_DEVICE pdink;

void setup()
{
	DEBUG.begin(115200);
	DEBUG.println();
	DEBUG.println("paperd.ink");

	pdink.begin();
	pdink.enable_display();

	pdink.connect_wifi(SSID, PASSWORD);

	pdink.display.fillScreen(GxEPD_WHITE);

	display_crypto_med_box(pdink.display, 0, 15, "Bitcoin");
	display_crypto_med_box(pdink.display, 0, 85, "Ethereum");
	display_crypto_med_box(pdink.display, 0, 155, "Dogecoin");
	display_crypto_med_box(pdink.display, 0, 225, "Monero");
	display_crypto_med_box(pdink.display, 200, 155, "USD-coin");
	display_crypto_med_box(pdink.display, 200, 225, "Solana");

	diplay_day_date_lrg_style1(pdink.display, 200, 20, TIME_ZONE);

	display_last_updated_time_med_style1(pdink.display, 215, 125, TIME_ZONE);

	pdink.display.display();

	DEBUG.println("Turning off everything");

    // Sleep till update time.
    // Align updates to 12am so that date change aligns
    // with actual day change.
	uint64_t sleep_time = (86400/(UPDATES_PER_DAY))-(((pdink_now.mil_hour*3600)+(pdink_now.min*60)+(pdink_now.sec))%(86400/UPDATES_PER_DAY));

    // Update after sleep_time microsecond or when button 1 is pressed.
    //pdink.deep_sleep_timer_wakeup(sleep_time*S_TO_uS_FACTOR); // Consumes lower current
	pdink.deep_sleep_timer_button_wakeup(sleep_time*S_TO_uS_FACTOR, BUTTON_1_PIN); // Consumes higher current
}

void loop()
{
    // put your main code here, to run repeatedly:
}
