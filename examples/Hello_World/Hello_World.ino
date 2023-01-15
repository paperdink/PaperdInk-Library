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

#define PAPERDINK_DEVICE Paperdink_Classic // or Paperdink_Merlot
#include <Paperdink.h>

PAPERDINK_DEVICE Paperdink;

void setup()
{
  /* Initialize device */
  Paperdink.begin();
  /* Enable power to display */
  Paperdink.enable_display();
  /* Clear the background */
  Paperdink.epd.fillScreen(GxEPD_WHITE);

  Paperdink.epd.setTextColor(GxEPD_BLACK);

  /* Print text to  display */
  GxEPD2_GFX& display = Paperdink.epd;
  display.setCursor(50, display.height()/2);
  display.print("Hello World");
  
  /* Send data to display for the update */ 
  Paperdink.epd.display();
}

void loop() {}
