# Wave
This example display day and date along with weather and todo list.
Todo list is fetched from [Taiga](taiga.io) currently and weather from [Openweathermap](openweathermap.org)

<img src="Wave.png" width="500" alt="Wave image">

## Usage

### Update `config.h`

Update the `config.h` file with configuration details

```
/* CONFIGURATION
 * Uncomment only one of the below #define statements
 * based on the paperd.ink device you have
 */
#define PAPERDINK_DEVICE Paperdink_Classic
//#define PAPERDINK_DEVICE Paperdink_Merlot

#define SSID     "*****" // Wifi Network SSID (name of wifi network)
#define PASSWORD "*****" // Wifi Network password

/* Weather information */
#define CITY    "*****" // City for weather
#define COUNTRY "*****" // Country for weather
#define OWM_ID  "*****" // Open weather map APP ID

/* Show forecasted weather this many hours from update time.
 * 0 for current weather
 */
#define FORECAST_HOURS 3

/* Taiga server information */
#define TAIGA_PROJECT_USERNAME  "*****" // Taiga username
#define TAIGA_PROJECT_PASSWORD  "*****" // Taiga login password
#define TAIGA_PROJECT_NAME      "*****" // Taiga project name

/* Time zone from list https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv */
#define TIME_ZONE "PST8PDT,M3.2.0,M11.1.0"

/* Number of times to update starting 12am
 * 1 = Updates every 24 hours
 * 2 = Updates every 12 hours
 * 3 = Updates every 8 hours. Not a good idea since it won't align with day changes.
 * 4 = Updates every 6 hours
 * ... and so on
 * Higher number means lower battery life
 */
#define UPDATES_PER_DAY 4

/* Offset to change start day of week. 0=>Sun, 1=>Sat, 2=>Fri ... 6=>Mon */
#define START_DAY_OFFSET 0

/* Number of images in data folder.
 * Background will cycle through these images 
 */
#define NUM_IMAGES 5
```
