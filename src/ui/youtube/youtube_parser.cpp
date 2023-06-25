#include "youtube.h"
#include "youtube_parser.h"

enum YOUTUBE_STATE { NOT_FOUND, FOUND_VIEWS, FOUND_SUBSCRIBERS };
static YOUTUBE_STATE key_state;

void YoutubeJsonListener::whitespace(char c)
{
}

void YoutubeJsonListener::startDocument()
{
	key_state = NOT_FOUND;
}

void YoutubeJsonListener::key(String key)
{
	if (key.equals("viewCount"))
		key_state = FOUND_VIEWS;
	else if (key.equals("subscriberCount"))
		key_state = FOUND_SUBSCRIBERS;
}

void YoutubeJsonListener::value(String value)
{
	if (key_state == FOUND_VIEWS) {
		sscanf(value.c_str(), "%lld", &Paperdink_Youtube.views);
		Serial.printf("Views: %lld\r\n", Paperdink_Youtube.views);
		key_state = NOT_FOUND;
	} else if (key_state == FOUND_SUBSCRIBERS) {
		sscanf(value.c_str(), "%lld", &Paperdink_Youtube.subscribers);
		Serial.printf("Subscribers: %lld\r\n", Paperdink_Youtube.subscribers);
		key_state = NOT_FOUND;
	}
}

void YoutubeJsonListener::endArray()
{
}

void YoutubeJsonListener::endObject()
{
}

void YoutubeJsonListener::endDocument()
{
}

void YoutubeJsonListener::startArray()
{
}

void YoutubeJsonListener::startObject()
{
}
