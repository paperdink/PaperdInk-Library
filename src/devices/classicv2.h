#ifndef _PDINK_CLASSIC_V2_H_
#define _PDINK_CLASSIC_V2_H_

#include "base.h"

class Paperdink_Classic_v2 : public PaperdinkDeviceBaseClass
{
	public:
		uint8_t has_color = false;
		int color = GxEPD_BLACK;

		//GxEPD2_4G_4G<GxEPD2_420, GxEPD2_420::HEIGHT> epd = GxEPD2_420(EPD_CS, EPD_DC, EPD_RST, EPD_BUSY);
		GxEPD2_BW<GxEPD2_420, GxEPD2_420::HEIGHT> epd = GxEPD2_420(EPD_CS, EPD_DC, EPD_RST, EPD_BUSY);

		int8_t enable_display() override;
		int8_t disable_display() override;
};

#endif /* _PDINK_CLASSIC_V2_H_ */
