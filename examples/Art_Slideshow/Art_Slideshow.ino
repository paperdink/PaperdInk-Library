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

#include "config.h"
#include <Paperdink.h>

PAPERDINK_DEVICE Paperdink;
/* Store image_counter in RTC memory
 * for persistence across deep sleep.
 */
RTC_DATA_ATTR uint8_t image_counter;

void setup() {
  char img_string[15];

  /* Enable debug */
  DEBUG.begin(115200);

  /* Initialize device */
  Paperdink.begin();
  /* Enable power to display */
  Paperdink.enable_display();
  /* Clear the background */
  Paperdink.epd.fillScreen(GxEPD_WHITE);

  if(Paperdink.enable_sd() < 0){
	  Paperdink.epd.setTextColor(GxEPD_BLACK);
	  Paperdink.epd.setRotation(0);
    Paperdink.epd.setFont(&PAPERDINK_FONT_SML);
    Paperdink.epd.setCursor(0, 120);
    Paperdink.epd.println("   SD card failed to be initialized.");
    Paperdink.epd.println("     1) Is a card inserted?");
    Paperdink.epd.println("     2) Is it FAT32 formatted?");
    Paperdink.epd.println("     3) Is it less than or equal to 32GB in size?");
    Serial.println("SD card failed to be initialized.");
  } else {
  
    Serial.println("SD OK!");
    /* Display image from SD card */
    #define NUM_IMAGES 5
    sprintf(img_string, "Art_%d.bmp", (image_counter++%NUM_IMAGES)+1);
    Paperdink_UI.display_bitmap_fs(Paperdink.epd, SD, img_string, 0, 0, Paperdink.has_color);
  }

  /* Send data to display for the update */
  Paperdink.epd.display();
	
  DEBUG.println("Turning off everything");
  
  /* Update after sleep_time microsecond or when button 1 is pressed. */
  Paperdink.deep_sleep_timer_wakeup(UPDATE_INTERVAL*M_TO_uS_FACTOR); // Consumes lower current
	//Paperdink.deep_sleep_timer_button_wakeup(sleep_time*S_TO_uS_FACTOR, BUTTON_1_PIN); // Consumes higher current
}

void loop() {}
