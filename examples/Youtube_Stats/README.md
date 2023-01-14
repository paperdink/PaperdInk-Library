# Youtube Stats
This example shows your youtube subscribers and total views count.

<img src="Youtube_Stats.png" width="500" alt="Youtube stats image">

### Device wake-up
The refresh can be controlled by the the following two lines. (Only un-comment one of the lines.)
- Line 1: For lower current consumption, it is possible to wake-up only after a specific period of time defined by the variable `sleep_time`.
- Line 2: For better control, it is possible to configure a button to trigger a wake-up in addition to a specific time period, but this leads to slightly higer current usage.

```
  /* Update after sleep_time microsecond or when button 1 is pressed. */
  // Paperdink.deep_sleep_timer_wakeup(sleep_time*S_TO_uS_FACTOR); // Consumes lower current
  Paperdink.deep_sleep_timer_button_wakeup(sleep_time*S_TO_uS_FACTOR, BUTTON_1_PIN); // Consumes higher current
```

## Usage
Update the `config.h` file with configuration details
```
// CONFIGURATION
// Uncomment only one of the below #define statements
// based on the paperd.ink device you have
#define PAPERDINK_DEVICE Paperdink_Classic
//#define PAPERDINK_DEVICE Paperdink_Merlot

#define SSID     "*****" // Wifi Network SSID (name of wifi network)
#define PASSWORD "*****" // Wifi Network password

// Time zone from list https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
#define TIME_ZONE "PST8PDT,M3.2.0,M11.1.0"

// Number of times to update starting 12am
// 1 = Updates every 24 hours
// 2 = Updates every 12 hours
// 3 = Updates every 8 hours. Not a good idea since it won't align with day changes.
// 4 = Updates every 6 hours
// ... and so on
// Higher number means lower battery life
#define UPDATES_PER_DAY 1

// Youtube configuration TODO: Add how-to
#define YOUTUBE_CHANNEL_NAME "YOUR-CHANNEL-NAME"
#define YOUTUBE_CHANNEL_ID   "YOUR-CHANNEL-ID"
#define YOUTUBE_API_ID       "YOUR-API-ID"
```
