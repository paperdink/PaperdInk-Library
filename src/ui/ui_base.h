
#ifndef _UI_BASE__H_
#define _UI_BASE__H_

#include "../common.h"

class PaperdinkUIBaseClass
{
    public:
        //const GFXfont* font_lrg = &Mont_HeavyDEMO25pt7b; 
        //const GFXfont* font_med = &Code_New_Roman15pt7b;
        //const GFXfont* font_med_bold = &Code_New_Roman_Bold15pt7b;
        const GFXfont* font = &PAPERDINK_FONT_SML; 
        const GFXfont* font_bold = &PAPERDINK_FONT_SML_BOLD; 

        void display_grid(GxEPD2_GFX& display);

        void display_text(GxEPD2_GFX& display, int16_t x, int16_t y, const char* string, int16_t w = 0, int16_t h = 0, const GFXfont* font = &PAPERDINK_FONT_SML, uint8_t color = GxEPD_BLACK, uint8_t size = 1);

};

extern PaperdinkUIBaseClass Paperdink_UI;
#endif /* _UI_BASE__H_ */
