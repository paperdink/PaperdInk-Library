# Cryptocurrency Tracker
This example displays 6 crypto tickers with their price and percentage change.
It also shows the date and time it was updated.

<img src="Crypto_Tracker.png" width="500" alt="Crypto tracker image">

## Usage

### Fetch crypto ticker
Visit [coingecko](www.coingecko.com) and search for the ticker you like.
Open the ticker page and note the **API id** field. This string is the ticker string
to be copied in the configuration file.

### Update `config.h`

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
#define UPDATES_PER_DAY 4

#define CRYPTO_TICKER_1 "Bitcoin"
#define CRYPTO_TICKER_2 "Ethereum"
#define CRYPTO_TICKER_3 "Dogecoin"
#define CRYPTO_TICKER_4 "Monero"
#define CRYPTO_TICKER_5 "Solana"
#define CRYPTO_TICKER_6 "Helium"
```
