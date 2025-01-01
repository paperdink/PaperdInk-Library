/** Examples
 * @example Calther.ino
 * Displays a calendar along with weather and todo list.
 *
 * @example Crypto_Tracker.ino
 * Shows 6 crypto tickers with last updated time and date.
 *
 * @example Hello_World.ino
 * Displays "Hello World" in the middle of the screen.
 *
 * @example Quotes.ino
 * Displays quotes from quotable API.
 *
 * @example Wave.ino
 * Displays day and date along with weather and todo list.
 *
 * @example Youtube_Stats.ino
 * Shows subscription and total views count.
 */

#ifndef _PAPERDINK_H_
#define _PAPERDINK_H_

#include <Arduino.h>
#include "pin_assignment.h"

// UI components
#include "ui/crypto/crypto.h"
#include "ui/date_time/date_time.h"
#include "ui/weather/weather.h"
#include "ui/youtube/youtube.h"
#include "ui/todo_list/todo_list.h"

// Paperdink Devices
#include "devices/classic.h"
//#include "devices/merlot.h"

// Conversion factor for seconds to microseconds
#define S_TO_uS_FACTOR (1000000)
#define M_TO_uS_FACTOR (6e7)
#define H_TO_uS_FACTOR (3.6e9)

extern PAPERDINK_DEVICE Paperdink;

#endif /* _PAPERDINK_H_ */
