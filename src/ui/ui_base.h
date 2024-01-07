#ifndef _UI_BASE__H_
#define _UI_BASE__H_

#include "../paperdink_common.h"

class PaperdinkUIBaseClass
{
	public:
		const GFXfont *font = &PAPERDINK_FONT_SML;
		const GFXfont *font_bold = &PAPERDINK_FONT_SML_BOLD;
		int primary_color = GxEPD_BLACK;
		int secondary_color = GxEPD_WHITE;
		int tertiary_color = GxEPD_BLACK;
};

class PaperdinkUIClass : public PaperdinkUIBaseClass
{
	public:

		void display_grid(GxEPD2_GFX& display);

		void display_text_center(GxEPD2_GFX& display, int16_t x, int16_t y, const char *string, int16_t w = 0, int16_t h = 0, const GFXfont *font = &PAPERDINK_FONT_SML, uint8_t color = GxEPD_BLACK, uint8_t size = 1);


		uint16_t read16(File& f);
		uint16_t read16(WiFiClient& client);
		uint32_t read32(File& f);
		uint32_t read32(WiFiClient& client);
		uint32_t skip(WiFiClient& client, int32_t bytes);
		uint32_t read8n(WiFiClient& client, uint8_t* buffer, int32_t bytes);

		void display_bitmap_fs(GxEPD2_GFX& display, fs::FS &fs, const char *filename, int16_t x, int16_t y, bool with_color);
		void display_bitmap_https(GxEPD2_GFX& display, const char* host, const int httpsPort, const char* path, const char* filename, int16_t x, int16_t y, bool with_color, const char* certificate = false);

};

extern PaperdinkUIClass Paperdink_UI;
#endif /* _UI_BASE__H_ */
