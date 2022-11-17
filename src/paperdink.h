
/** Examples
 * @example crypto_tracker.ino
 * Shows 6 crypto tickers with last updated time and date. 
 */

#ifndef _PAPERDINK_H_
#define _PAPERDINK_H_

#include <Arduino.h>
#include "bitmap/bitmap_parser.h"
#include "pin_assignment.h"

// UI components
#include "ui/crypto/crypto.h"
#include "ui/date_time/date_time.h"
#include "ui/weather/weather.h"

// Paperdink Devices
#include "devices/classic.h"
#include "devices/merlot.h"

// Conversion factor for seconds to microseconds
#define S_TO_uS_FACTOR (1000000)
#define M_TO_uS_FACTOR (6e7)
#define H_TO_uS_FACTOR (3.6e9)

extern PDINK_DEVICE pdink;
extern RTC_DATA_ATTR struct time_struct pdink_now;

#endif /* _PAPERDINK_H_ */
