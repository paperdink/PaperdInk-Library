
#ifndef _UI_BASE__H_
#define _UI_BASE__H_

#include <FS.h>
#include "../common.h"

class PaperdinkUIBaseClass
{
    public:
        const GFXfont* font = &PAPERDINK_FONT_SML; 
        const GFXfont* font_bold = &PAPERDINK_FONT_SML_BOLD; 
};

class PaperdinkUIClass : public PaperdinkUIBaseClass
{
    public:

        void display_grid(GxEPD2_GFX& display);

        void display_text_center(GxEPD2_GFX& display, int16_t x, int16_t y, const char* string, int16_t w = 0, int16_t h = 0, const GFXfont* font = &PAPERDINK_FONT_SML, uint8_t color = GxEPD_BLACK, uint8_t size = 1);


        uint16_t read16(File& f);
        uint32_t read32(File& f);
        void display_bitmap_fs(GxEPD2_GFX& display, fs::FS &fs, const char *filename, int16_t x, int16_t y, bool with_color);

}; 

extern PaperdinkUIClass Paperdink_UI;
#endif /* _UI_BASE__H_ */
