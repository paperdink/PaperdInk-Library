#include "classic.h"

int8_t Paperdink_Classic::enable_display()
{
	PaperdinkDeviceBaseClass::enable_display();
	epd.init();

	return 0;
}

int8_t Paperdink_Classic::disable_display()
{
	digitalWrite(EPD_EN, HIGH);

	return 0;
}

