
#include <WiFiClientSecure.h>
#include <HTTPClient.h>

#include "youtube.h"
#include "youtube_parser.h"

static String url = "https://www.googleapis.com/youtube/v3/channels?part=statistics&id={0}&key={1}";
YoutubeJsonListener youtube_listener;

static const char* cert = "-----BEGIN CERTIFICATE-----\n" \
"MIIFVzCCAz+gAwIBAgINAgPlk28xsBNJiGuiFzANBgkqhkiG9w0BAQwFADBHMQsw\n" \
"CQYDVQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZpY2VzIExMQzEU\n" \
"MBIGA1UEAxMLR1RTIFJvb3QgUjEwHhcNMTYwNjIyMDAwMDAwWhcNMzYwNjIyMDAw\n" \
"MDAwWjBHMQswCQYDVQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZp\n" \
"Y2VzIExMQzEUMBIGA1UEAxMLR1RTIFJvb3QgUjEwggIiMA0GCSqGSIb3DQEBAQUA\n" \
"A4ICDwAwggIKAoICAQC2EQKLHuOhd5s73L+UPreVp0A8of2C+X0yBoJx9vaMf/vo\n" \
"27xqLpeXo4xL+Sv2sfnOhB2x+cWX3u+58qPpvBKJXqeqUqv4IyfLpLGcY9vXmX7w\n" \
"Cl7raKb0xlpHDU0QM+NOsROjyBhsS+z8CZDfnWQpJSMHobTSPS5g4M/SCYe7zUjw\n" \
"TcLCeoiKu7rPWRnWr4+wB7CeMfGCwcDfLqZtbBkOtdh+JhpFAz2weaSUKK0Pfybl\n" \
"qAj+lug8aJRT7oM6iCsVlgmy4HqMLnXWnOunVmSPlk9orj2XwoSPwLxAwAtcvfaH\n" \
"szVsrBhQf4TgTM2S0yDpM7xSma8ytSmzJSq0SPly4cpk9+aCEI3oncKKiPo4Zor8\n" \
"Y/kB+Xj9e1x3+naH+uzfsQ55lVe0vSbv1gHR6xYKu44LtcXFilWr06zqkUspzBmk\n" \
"MiVOKvFlRNACzqrOSbTqn3yDsEB750Orp2yjj32JgfpMpf/VjsPOS+C12LOORc92\n" \
"wO1AK/1TD7Cn1TsNsYqiA94xrcx36m97PtbfkSIS5r762DL8EGMUUXLeXdYWk70p\n" \
"aDPvOmbsB4om3xPXV2V4J95eSRQAogB/mqghtqmxlbCluQ0WEdrHbEg8QOB+DVrN\n" \
"VjzRlwW5y0vtOUucxD/SVRNuJLDWcfr0wbrM7Rv1/oFB2ACYPTrIrnqYNxgFlQID\n" \
"AQABo0IwQDAOBgNVHQ8BAf8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4E\n" \
"FgQU5K8rJnEaK0gnhS9SZizv8IkTcT4wDQYJKoZIhvcNAQEMBQADggIBAJ+qQibb\n" \
"C5u+/x6Wki4+omVKapi6Ist9wTrYggoGxval3sBOh2Z5ofmmWJyq+bXmYOfg6LEe\n" \
"QkEzCzc9zolwFcq1JKjPa7XSQCGYzyI0zzvFIoTgxQ6KfF2I5DUkzps+GlQebtuy\n" \
"h6f88/qBVRRiClmpIgUxPoLW7ttXNLwzldMXG+gnoot7TiYaelpkttGsN/H9oPM4\n" \
"7HLwEXWdyzRSjeZ2axfG34arJ45JK3VmgRAhpuo+9K4l/3wV3s6MJT/KYnAK9y8J\n" \
"ZgfIPxz88NtFMN9iiMG1D53Dn0reWVlHxYciNuaCp+0KueIHoI17eko8cdLiA6Ef\n" \
"MgfdG+RCzgwARWGAtQsgWSl4vflVy2PFPEz0tv/bal8xa5meLMFrUKTX5hgUvYU/\n" \
"Z6tGn6D/Qqc6f1zLXbBwHSs09dR2CQzreExZBfMzQsNhFRAbd03OIozUhfJFfbdT\n" \
"6u9AWpQKXCBfTkBdYiJ23//OYb2MI3jSNwLgjt7RETeJ9r/tSQdirpLsQBqvFAnZ\n" \
"0E6yove+7u7Y/9waLd64NnHi/Hm3lCXRSHNboTXns5lndcEZOitHTtNCjv0xyBZm\n" \
"2tIMPNuzjsmhDYAPexZ3FL//2wmUspO8IFgV6dtxQ/PeEMMA3KgqlbbC1j+Qa3bb\n" \
"bP6MvPJwNQzcmRk13NfIRmPVNnGuV/u3gm3c\n" \
"-----END CERTIFICATE-----\n";

int8_t PaperdinkUIYoutubeClass::fetch_data(const char *channel_id, const char *api_id)
{
	int httpCode, ret = 0;
	String payload;
	WiFiClientSecure *client = new WiFiClientSecure;
    client->setCACert(cert);

	String get_url = url;
    get_url.replace("{0}", channel_id);
    get_url.replace("{1}", api_id);

    {
        HTTPClient https;
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
    }
    
    delete client;
	DEBUG.printf("[HTTP] COMPLETED \r\n");
	return ret;
}

void PaperdinkUIYoutubeClass::display_subscribers_med(GxEPD2_GFX& display, int16_t x, int16_t y, int16_t w, int16_t h)
{
	int16_t xt, yt;
	uint16_t wt, ht, prev_height = y, prev_width = x;
    char subscribers_str[MAX_YOUTUBE_SUBSCRIBER_COUNT];

    display.setFont(font);
	display.setTextColor(primary_color);
	display.setTextSize(1);

    // Display subscribers count
	snprintf(subscribers_str, MAX_YOUTUBE_SUBSCRIBER_COUNT, "%lld", subscribers);
    display.getTextBounds(subscribers_str, 0, 0, &xt, &yt, &wt, &ht);
	if(w == 0 || h == 0){
        /* No centering */
        // 5px and 4px margin
        display.setCursor(prev_width+5, prev_height+ht+4);
    } else {
        display.setCursor(prev_width+5+((w-wt)/2)+(user_med_width/2), prev_height+4+((h-ht)/2));
        prev_width += 5+((w+wt)/2)+(user_med_width/2);
        prev_height += 4+(ht);
    }
    display.print(subscribers_str);
    
    // 3 px margin
    display.drawBitmap(prev_width-wt-user_med_width-5, prev_height-user_med_height+3, user_med, user_med_width, user_med_height, primary_color);
}

void PaperdinkUIYoutubeClass::display_views_med(GxEPD2_GFX& display, int16_t x, int16_t y, int16_t w, int16_t h)
{
	int16_t xt, yt;
	uint16_t wt, ht, prev_height = y, prev_width = x;
	const int16_t block_width = 100;
    char views_str[MAX_YOUTUBE_VIEWS_COUNT];

    display.setFont(font);
	display.setTextColor(primary_color);
	display.setTextSize(1);

    // Display ticker
	snprintf(views_str, MAX_YOUTUBE_VIEWS_COUNT, "%lld", views);
    display.getTextBounds(views_str, 0, 0, &xt, &yt, &wt, &ht);
	if(w == 0 || h == 0){
        /* No centering */
        // 5px and 4px margin
        display.setCursor(prev_width+5, prev_height+ht+4);
    } else {
        display.setCursor(prev_width+5+((w-wt)/2)+(eye_med_width/2), prev_height+4+((h-ht)/2));
        prev_width += 5+((w+wt)/2)+(eye_med_width/2);
        prev_height += 4+(ht);
    }
	display.print(views_str);

    // 3 px margin
    display.drawBitmap(prev_width-wt-eye_med_width-5, prev_height-eye_med_height+3, eye_med, eye_med_width, eye_med_height, primary_color);
}

PaperdinkUIYoutubeClass Paperdink_Youtube;
