#include "merlot.h"

int8_t Paperdink_Merlot::enable_display()
{
	PaperdinkDeviceBaseClass::enable_display();
	epd.init();

	return 0;
}

int8_t Paperdink_Merlot::disable_display()
{
	digitalWrite(EPD_EN, HIGH);

	return 0;
}

