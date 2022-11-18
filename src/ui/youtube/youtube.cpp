
#include <WiFiClientSecure.h>
#include <HTTPClient.h>

#include "youtube.h"
#include "youtube_parser.h"

static String url = "https://www.googleapis.com/youtube/v3/channels?part=statistics&id={0}&key={1}";
YoutubeJsonListener youtube_listener;

int8_t PaperdinkUIYoutubeClass::fetch_data(const char *channel_id, const char *api_id)
{
	int httpCode, ret = 0;
	String payload;
	HTTPClient https;
	WiFiClientSecure *client = new WiFiClientSecure;

	String get_url = url;
    get_url.replace("{0}", channel_id);
    get_url.replace("{1}", api_id);

	https.begin(*client, get_url.c_str());
	https.addHeader("Content-Type", "application/json", 0, 0);

	httpCode = https.GET();

	if(httpCode == HTTP_CODE_OK){
        // HTTP header has been send and Server response header has been handled
		DEBUG.printf("[HTTP] GET SUCCESS\r\n");
        //String payload = https.getString();
        //Serial.println(payload);
		ArudinoStreamParser parser;
		parser.setListener(&youtube_listener);
		https.writeToStream(&parser);
	} else {
		DEBUG.printf("[HTTP] Failed, error: %s\r\n", https.errorToString(httpCode).c_str());
		ret = -1;
	}

	https.end();

	DEBUG.printf("[HTTP] COMPLETED \r\n");
	return ret;
}

void PaperdinkUIYoutubeClass::display_subscribers_sml(GxEPD2_GFX& display, int16_t x, int16_t y, int16_t w, int16_t h)
{
	int16_t xt, yt;
	uint16_t wt, ht, prev_height = y, prev_width = x;
    char subscribers_str[MAX_YOUTUBE_SUBSCRIBER_COUNT];

    display.setFont(font_med_bold);
	display.setTextColor(GxEPD_BLACK);
	display.setTextSize(1);

    // Display subscribers count
	snprintf(subscribers_str, MAX_YOUTUBE_SUBSCRIBER_COUNT, "%lld", subscribers);
    display.getTextBounds(subscribers_str, 0, 0, &xt, &yt, &wt, &ht);
	if(w == 0 || h == 0){
        /* No centering */
        // 5px and 4px margin
        display.setCursor(prev_width+5, prev_height+ht+4);
    } else {
        display.setCursor(prev_width+5+((w-wt)/2), prev_height+4+((h-ht)/2));
        prev_width += 5+((w+wt)/2);
        prev_height += 4+(ht);
    }
    display.print(subscribers_str);
    
    // 3 px margin
    display.drawBitmap(prev_width-wt-user_sml_width-3, prev_height-(user_sml_height/2), user_sml, user_sml_width, user_sml_height, GxEPD_BLACK);
}

void PaperdinkUIYoutubeClass::display_views_sml(GxEPD2_GFX& display, int16_t x, int16_t y, int16_t w, int16_t h)
{
	int16_t xt, yt;
	uint16_t wt, ht, prev_height = y, prev_width = x;
	const int16_t block_width = 100;
    char views_str[MAX_YOUTUBE_VIEWS_COUNT];

    display.setFont(font_med_bold);
	display.setTextColor(GxEPD_BLACK);
	display.setTextSize(1);

    // Display ticker
	snprintf(views_str, MAX_YOUTUBE_VIEWS_COUNT, "%lld", views);
    display.getTextBounds(views_str, 0, 0, &xt, &yt, &wt, &ht);
	if(w == 0 || h == 0){
        /* No centering */
        // 5px and 4px margin
        display.setCursor(prev_width+5, prev_height+ht+4);
    } else {
        display.setCursor(prev_width+5+((w-wt)/2), prev_height+4+((h-ht)/2));
        prev_width += 5+((w+wt)/2);
        prev_height += 4+(ht);
    }
	display.print(views_str);

    // 3 px margin
    display.drawBitmap(prev_width-wt-eye_sml_width-3, prev_height-(eye_sml_height/2), eye_sml, eye_sml_width, eye_sml_height, GxEPD_BLACK);
}

PaperdinkUIYoutubeClass Paperdink_Youtube;
