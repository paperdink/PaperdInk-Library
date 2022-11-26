
#ifndef CONFIG_H
#define CONFIG_H

// CONFIGURATION
// Uncomment only one of the below #define statements
// based on the paperd.ink device you have
//#define PAPERDINK_DEVICE Paperdink_Classic
#define PAPERDINK_DEVICE Paperdink_Merlot

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

/* Youtube configuration TODO: Add how-to
 * Get ID from username. Replace <API_KEY> and <USERNAME> 
 * https://www.googleapis.com/youtube/v3/channels?key=<API_KEY>&forUsername=<USERNAME>&part=id
 * 
 * Uncomment either DESIGN_1 or DESIGN_2
 */
//#define DESIGN_1 (true)
#define DESIGN_2 (true)
#define DESIGN_2_STRING       "@XYZChannel"
#define YOUTUBE_CHANNEL_NAME  "XYZ Channel"
#define YOUTUBE_CHANNEL_ID    "*****"
#define YOUTUBE_API_ID        "*****"

#endif /* CONFIG_H */
