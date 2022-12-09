# Wave
This example display day and date along with weather and todo list.
Todo list can be synced from [Taiga](https://www.taiga.io) or [Todoist](https://todoist.com) and weather from [Openweathermap](https://www.openweathermap.org)

<img src="Wave.png" width="500" alt="Wave image">

## Usage

### Getting Openweathermap API key
From their [website](https://openweathermap.org/appid),

The API key is all you need to call any of our weather APIs.
Once you [sign up](https://openweathermap.org/home/sign_up) using your email,
the API key (APPID) will be sent to you in a confirmation email.
Your API keys can always be found on your [account page](https://home.openweathermap.org/api_keys),
where you can also generate additional API keys if needed. 
Check our [documentation page](https://openweathermap.org/api)
to find all technical information for each product. 
Documentation is an essential guide with actual examples and 
comprehensive description of API calls, responses and parameters.

### Todo list from Taiga
First create an account on [Taiga](https://www.taiga.io) and create a new project.
Add your login details and project name to the config file.

### Todo list from Todoist
First create an account on [Todoist](https://www.todoist.com).
Go to `Settings > Integrations` and generate an API token.
Copy this token to the config file.

### Uploading bitmaps
Use [ESP32 Sketch data upload](https://randomnerdtutorials.com/install-esp32-filesystem-uploader-arduino-ide/)
to upload images from data folder.

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
