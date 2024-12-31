#include "classicv2.h"

int8_t Paperdink_Classic_v2::enable_display()
{
	PaperdinkDeviceBaseClass::enable_display();
	epd.init();

	return 0;
}

int8_t Paperdink_Classic_v2::disable_display()
{
	digitalWrite(EPD_EN, HIGH);

	return 0;
}
