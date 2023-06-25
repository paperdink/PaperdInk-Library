#ifndef _PDINK_BASE_H_
#define _PDINK_BASE_H_

#define ENABLE_GxEPD2_GFX 1
#include <GxEPD2_BW.h>
#include <GxEPD2_3C.h>

#include "pin_assignment.h"
#include "../paperdink_common.h"

class PaperdinkDeviceBaseClass
{
public:
uint8_t has_color = false;
int color = GxEPD_BLACK;

int8_t begin();
virtual int8_t enable_display();
virtual int8_t disable_display();
int8_t enable_sd();
int8_t disable_sd();
int8_t disable_everything();

int8_t deep_sleep_timer_wakeup(uint64_t sleep_time_us);
int8_t deep_sleep_button_wakeup(uint8_t gpio_num);
int8_t deep_sleep_timer_button_wakeup(uint64_t sleep_time_us, uint8_t gpio_num);

int8_t connect_wifi(const char *ssid, const char *password, uint8_t attempts);
int8_t connect_wifi(const char *ssid, const char *password);
};

#endif /* _PDINK_BASE_H_ */
