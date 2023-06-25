#ifndef _YOUTUBE_H_
#define _YOUTUBE_H_

#include "../ui_base.h"

#define DEBUG Serial

#define MAX_YOUTUBE_SUBSCRIBER_COUNT 20
#define MAX_YOUTUBE_VIEWS_COUNT 20

class PaperdinkUIYoutubeClass : public PaperdinkUIBaseClass
{
public:
uint64_t subscribers = 0;
uint64_t views = 0;

int8_t fetch_data(const char *channel_id, const char *api_id);

void display_subscribers_med(GxEPD2_GFX& display, int16_t x, int16_t y, int16_t w = 0, int16_t h = 0);
void display_views_med(GxEPD2_GFX& display, int16_t x, int16_t y, int16_t w = 0, int16_t h = 0);
};

extern PaperdinkUIYoutubeClass Paperdink_Youtube;

#endif /* _YOUTUBE_H_ */
