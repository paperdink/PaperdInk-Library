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
	/* Comment this line to disable Serial debug output */
	DEBUG.begin(115200);

	/* Initialize device */
	Paperdink.begin();
	/* Enable power to display */
	Paperdink.enable_display();
	/* Clear the background */
	Paperdink.epd.fillScreen(GxEPD_WHITE);
	/* Connect to Wifi */
	if (Paperdink.connect_wifi(WIFI_NAME, PASSWORD) < 0) {
		DEBUG.println("Unable to connect to WiFi");
		Paperdink.epd.drawBitmap(370, 4, wifi_off_sml, wifi_off_sml_width, wifi_off_sml_height, GxEPD_BLACK);
	} else {
		Paperdink_UI.display_bitmap_https(Paperdink.epd, HOST, PORT, PATH, IMG_FILE, 0, 0, false);

		/* Send data to display for the update */
		Paperdink.epd.display();
	}

	DEBUG.println("Turning off everything");

	/* Sleep till update time.
	 * Align updates to 12am so that date change aligns
	 * with actual day change.
	 */
	uint64_t sleep_time = (86400 / (UPDATES_PER_DAY)) - (((Paperdink_Date.mil_hour * 3600) + (Paperdink_Date.min * 60) + (Paperdink_Date.sec)) % (86400 / UPDATES_PER_DAY));

	/* Update after sleep_time microsecond or when button 1 is pressed. */
	// Paperdink.deep_sleep_timer_wakeup(sleep_time*S_TO_uS_FACTOR); // Consumes lower current
	Paperdink.deep_sleep_timer_button_wakeup(sleep_time * S_TO_uS_FACTOR, BUTTON_1_PIN); // Consumes higher current
}

void loop()
{
}

