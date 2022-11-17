
#ifndef BITMAP_PARSER_H
#define BITMAP_PARSER_H

#include <FS.h>
#include <GxEPD2_GFX.h>

// TODO: Make debug print handling better
#ifndef DEBUG
#define DEBUG Serial
#endif

void drawBitmapFrom_SD_ToBuffer(GxEPD2_GFX& display, fs::FS &fs, const char *filename, int16_t x, int16_t y, bool with_color);

#endif /* BITMAP_PARSER_H */
