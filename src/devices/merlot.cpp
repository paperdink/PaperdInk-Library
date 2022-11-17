#include "merlot.h"

int8_t Paperdink_Merlot::enable_display()
{
	Paperdink_Base_Class::enable_display();
	display.init();

	return 0;
}

int8_t Paperdink_Merlot::disable_display()
{
	digitalWrite(EPD_EN, HIGH);

	return 0;
}

