
#ifndef _PDINK_MERLOT_H_
#define _PDINK_MERLOT_H_

#include "base.h"

class Paperdink_Merlot : public PaperdinkDeviceBaseClass
{
    public:
        uint8_t has_color = true;
        uint8_t color = GxEPD_RED;

        GxEPD2_3C<GxEPD2_420c_Z21, GxEPD2_420c_Z21::HEIGHT> epd = GxEPD2_420c_Z21(EPD_CS, EPD_DC, EPD_RST, EPD_BUSY);
        int8_t enable_display() override;
        int8_t disable_display() override;

};

#endif /* _PDINK_MERLOT_H_ */
