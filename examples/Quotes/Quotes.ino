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

#include <WiFiClientSecure.h>
#include <HTTPClient.h>
/* config.h should be before Paperdink.h */
#include "config.h"
#include <Paperdink.h>

int8_t fetch_quote();
void display_quote(GxEPD2_GFX& display);

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

    /* Fetch date data */
    if(Paperdink_Date.fetch_data(TIME_ZONE) < 0){
        DEBUG.println("Unable to fetch Date Time data");
    }

    /* Fetch and display quote */
	fetch_quote();
	display_quote(Paperdink.epd);

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

#include "quotes_parser.h"

char quote_string[MAX_QUOTE_LENGTH];
char author_string[MAX_AUTHOR_LENGTH];

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define MAX_QUOTE_LENGTH_STR TOSTRING(MAX_QUOTE_LENGTH)
const char* url = "https://api.quotable.io/random?maxLength="MAX_QUOTE_LENGTH_STR "&tags=";
quoteListener quote_listener;

int8_t fetch_quote()
{
    int httpCode, ret = 0;
    String payload;
    HTTPClient https;
    WiFiClientSecure *client = new WiFiClientSecure;

    https.begin(*client, url);
    https.addHeader("Content-Type", "application/json", 0, 0);

    httpCode = https.GET();

    if(httpCode == HTTP_CODE_OK){
        // HTTP header has been send and Server response header has been handled
        DEBUG.printf("[HTTP] GET SUCCESS\r\n");
        //String payload = https.getString();
        //Serial.println(payload);
        ArudinoStreamParser parser;
        parser.setListener(&quote_listener);
        https.writeToStream(&parser);
    } else {
        DEBUG.printf("[HTTP] AUTH... failed, error: %d::%s\r\n", httpCode, https.errorToString(httpCode).c_str());
        ret = -1;
    }

    https.end();

    DEBUG.printf("[HTTP] COMPLETED \r\n");
    return ret;
}

int32_t get_chars_words(GxEPD2_GFX& display, char *string, char *first_line, uint8_t margin)
{
    int16_t x = 0, y = 0;
    uint16_t w = 0, h = 0;
    int32_t length_count = 0;
    int32_t space_index = 0;
    char *temp = first_line;

    while(*string != '\0' && w <= display.width()-margin){
        length_count++;
        *temp = *string;

        if(*string == ' '){
            space_index = length_count;
        }

        *(temp+1) = '\0';
        display.getTextBounds(first_line, 0, 0, &x, &y, &w, &h);
        temp++;
        string++;
    }

    if(w <= display.width()-margin){
        space_index = length_count;
    }

    first_line[space_index] = '\0';

    return space_index;
}

void display_quote(GxEPD2_GFX& display)
{
    int16_t x, y;
    uint16_t w, h;
    char line[EXPECTED_LINE_CHARS];
    uint32_t first_line_chars, num_lines = 0;
    int32_t str_len, completed = 0;

    str_len = strlen(quote_string);

    display.fillScreen(GxEPD_WHITE);
    display.setFont(QUOTE_FONT);
    display.setTextColor(GxEPD_BLACK);
    display.setTextSize(1);

    // To get height of the font
    display.getTextBounds(F("A"), 0, 0, &x, &y, &w, &h);
    num_lines = (str_len/LINE_HEIGHT_FACTOR)+1;
    display.setCursor(FIRST_LINE_MARGIN, (display.height()-(num_lines*h))/2);

    completed = get_chars_words(display, quote_string, line, (FIRST_LINE_MARGIN+QUOTE_MARGIN_RIGHT));
    display.println(line);

    while(completed < str_len){
        completed += get_chars_words(display, &quote_string[completed], line, (QUOTE_MARGIN_LEFT+QUOTE_MARGIN_RIGHT));
        DEBUG.printf("Completed: %d/%d\r\n", completed, str_len);

        display.getTextBounds(line, 0, 0, &x, &y, &w, &h);
        display.setCursor(QUOTE_MARGIN_LEFT+display.getCursorX(), display.getCursorY());
        display.println(line);
    }

    display.setFont(AUTHOR_FONT);
    display.setTextColor(display.epd2.hasColor ? GxEPD_RED : GxEPD_BLACK);
    display.getTextBounds(author_string, 0, 0, &x, &y, &w, &h);
    display.setCursor((display.width()-w)/2, display.getCursorY()+10);
    display.print("-");
    display.println(author_string);
}


