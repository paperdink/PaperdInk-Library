
#include "ui_base.h"

void PaperdinkUIBaseClass::display_grid(GxEPD2_GFX& display)
{
    /* Vertical line at half width */
    display.drawLine(display.width()*0.5, 0, display.width()*0.5, display.height(), GxEPD_BLACK);
    /* Vertical line at 1/4 width */
    display.drawLine(display.width()*0.25, 0, display.width()*0.25, display.height(), GxEPD_BLACK);
    /* Vertical line at 3/4 width */
    display.drawLine(display.width()*0.75, 0, display.width()*0.75, display.height(), GxEPD_BLACK);
    
    /* Horizontal line at half height */
    display.drawLine(0, display.height()*0.5, display.width(), display.height()*0.5, GxEPD_BLACK);
    /* Horizontal line at 1/4 width */
    display.drawLine(0, display.height()*0.25, display.width(), display.height()*0.25, GxEPD_BLACK);
    /* Horizontal line at 3/4 width */
    display.drawLine(0, display.height()*0.75, display.width(), display.height()*0.75, GxEPD_BLACK);
}


void PaperdinkUIBaseClass::display_text(GxEPD2_GFX& display, int16_t x, int16_t y, const char *string, int16_t w, int16_t h, const GFXfont* font, uint8_t color)
{
	int16_t xt, yt;
	uint16_t wt, ht, prev_height = y, prev_width = x;
    
    display.setFont(font);
	display.setTextColor(color);
	display.setTextSize(1);

    display.getTextBounds(string, 0, 0, &xt, &yt, &wt, &ht);
	if(w == 0 || h == 0){
        /* No centering */
        display.setCursor(prev_width, prev_height+ht);
    } else {
        display.setCursor(prev_width+((w-wt)/2), prev_height+((h-ht)/2));
    }

	display.print(string);
}

PaperdinkUIBaseClass Paperdink_UI;
