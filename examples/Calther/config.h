#ifndef CONFIG_H
#define CONFIG_H

/* CONFIGURATION
 * Uncomment only one of the below #define statements
 * based on the paperd.ink device you have
 */
#define PAPERDINK_DEVICE Paperdink_Classic
//#define PAPERDINK_DEVICE Paperdink_Merlot

#define SSID     "*****"        // Wifi Network SSID (name of wifi network)
#define PASSWORD "*****"        // Wifi Network password

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

/* Weather information */
#define CITY    "*****" // City for weather
#define COUNTRY "*****" // Country for weather
#define OWM_ID  "*****" // Open weather map APP ID
#define UNITS   "M"     // M for metric (celsius), I for Imperial (fahrenheit)

/* To-Do list config
 * Select Todo list provider between Taiga or Todoist
 * by uncommenting their respective lines.
 */
/* Todoist configuration */
//#define TODOIST (true)
//#define TODOIST_TOKEN "Bearer *****" // Todoist Token

/* Taiga configuration */
#define TAIGA (true)
#define TAIGA_PROJECT_USERNAME  "*****" // Taiga username
#define TAIGA_PROJECT_PASSWORD  "*****" // Taiga login password
#define TAIGA_PROJECT_NAME      "*****" // Taiga project name


#endif /* CONFIG_H */
