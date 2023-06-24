# Jokes

 This is an example that displays a joke of the day using the [Official Jokes API](https://github.com/15Dkatz/official_joke_api). It fetches a new joke from the API every day and then immediately goes into deep sleep to  preserve battery. It also displays a rough estimate of the battery charge remaining on the screen that is also updated once per day.

<img src="Jokes.png" width="500" alt="Jokes image">

## Usage

### Update `Jokes.ino`

Update the `Jokes.ino` file with configuration details. You don't really need to change anything other than selecting paperd.ink device, WiFi details and Time zone. Other configuration is provided to make it easy to change the font.

```c
/* CONFIGURATION
 * Uncomment only one of the below #define statements
 * based on the paperd.ink device you have
 */
#define PAPERDINK_DEVICE Paperdink_Classic
//#define PAPERDINK_DEVICE Paperdink_Merlot

#define SSID     "MY_WIFI_NETWORK" // Wifi Network SSID (name of wifi network)
#define PASSWORD "MY_WIFI_PASSWORD" // Wifi Network password

/* Time zone from list https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv */
#define TIME_ZONE "PST8PDT,M3.2.0,M11.1.0"

#define UPDATES_PER_DAY 4
/* Number of times to update starting 12am
 * 1 = Updates every 24 hours
 * 2 = Updates every 12 hours
 * 3 = Updates every 8 hours. Not a good idea since it won't align with day changes.
 * 4 = Updates every 6 hours
 * ... and so on
 * Higher number means lower battery life
 */

#define MAX_JOKE_LENGTH 256   // Maximum characters in a joke
#define MAX_PUNCHLINE_LENGTH 128  // Maximum characters in a punchline

#define FIRST_LINE_MARGIN 18   // Margin to leave on the left for the first line (in pixels)
#define JOKE_MARGIN_LEFT 18   // Margin to leave on the left of the joke 2nd line onwards (in pixels)
#define JOKE_MARGIN_RIGHT 18  // Margin to leave on the right of the joke (in pixels)

#define EXPECTED_LINE_CHARS 40 // Maximum expected characters in a line
#define LINE_HEIGHT_FACTOR 30  // Dividing string length with this to get number of lines
```
