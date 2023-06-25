/*
 *  E-Ink Joke Display
 *  This is an Arduino sketch for an internet-connected E-ink screen powered by an ESP32 that displays a joke of the day
 *  using a joke API. It also displays a rough estimate of the battery charge remaining on the screen.
 *
 *  Created by Alex Meub, modified from: https://github.com/paperdink/PaperdInk-Library/tree/main/examples/Quotes
 *  MIT License (MIT)
 */

#include <WiFiClientSecure.h>
#include <HTTPClient.h>

/* CONFIGURATION
 * Uncomment only one of the below #define statements
 * based on the paperd.ink device you have
 */
#define PAPERDINK_DEVICE Paperdink_Classic
//#define PAPERDINK_DEVICE Paperdink_Merlot

#define SSID     "*****"      // Wifi Network SSID (name of wifi network)
#define PASSWORD "*****"      // Wifi Network password

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

#define MAX_JOKE_LENGTH 256             // Maximum characters in a joke
#define MAX_PUNCHLINE_LENGTH 128        // Maximum characters in a punchline

#define FIRST_LINE_MARGIN 18            // Margin to leave on the left for the first line (in pixels)
#define JOKE_MARGIN_LEFT 18             // Margin to leave on the left of the joke 2nd line onwards (in pixels)
#define JOKE_MARGIN_RIGHT 18            // Margin to leave on the right of the joke (in pixels)

#define EXPECTED_LINE_CHARS 40          // Maximum expected characters in a line
#define LINE_HEIGHT_FACTOR 30           // Dividing string length with this to get number of lines

#include <Paperdink.h>

int8_t fetch_joke();
void display_joke(GxEPD2_GFX& display);

/* Create paperdink object */
PAPERDINK_DEVICE Paperdink;

void setup()
{
	DEBUG.begin(115200);

	/* Initialize paperd.ink device */
	Paperdink.begin();

	/* Enable power to the display */
	Paperdink.enable_display();

	/* Clear the background */
	Paperdink.epd.fillScreen(GxEPD_WHITE);

	/* Connect to wifi network */
	if (Paperdink.connect_wifi(SSID, PASSWORD) < 0) {
		DEBUG.println("Unable to connect to WiFi");
		Paperdink.epd.drawBitmap(370, 4, wifi_off_sml, wifi_off_sml_width, wifi_off_sml_height, GxEPD_BLACK);
	}

	/* Fetch date data */
	if (Paperdink_Date.fetch_data(TIME_ZONE) < 0)
		DEBUG.println("Unable to fetch Date Time data");

	/* Fetch and display joke */
	fetch_joke();
	display_joke(Paperdink.epd);

	/* Send data to display for the update */
	Paperdink.epd.display();

	DEBUG.println("Turning off everything");

	/* Sleep till update time.
	 * Align updates to 12am so that date change aligns
	 * with actual day change.
	 */
	uint64_t sleep_time = (86400 / (UPDATES_PER_DAY)) - (((Paperdink_Date.mil_hour * 3600) + (Paperdink_Date.min * 60) + (Paperdink_Date.sec)) % (86400 / UPDATES_PER_DAY));

	/* Update after sleep_time microsecond or when button 1 is pressed. */
	// Paperdink.deep_sleep_timer_wakeup(sleep_time*S_TO_uS_FACTOR); // Consumes lower current
	Paperdink.deep_sleep_timer_button_wakeup(sleep_time * S_TO_uS_FACTOR, BUTTON_1_PIN); // Consumes higher current
}

void loop()
{
}

char joke_string[MAX_JOKE_LENGTH];
char punchline_string[MAX_PUNCHLINE_LENGTH];
char date_string[15];

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define MAX_JOKE_LENGTH_STR TOSTRING(MAX_JOKE_LENGTH)

const char *url = "http://official-joke-api.appspot.com/jokes/general/random/";

int8_t fetch_joke()
{
	int httpCode, ret = 0;
	String payload;

	WiFiClient *client = new WiFiClient;

	{
		HTTPClient http;
		http.begin(*client, url);
		http.addHeader("Content-Type", "application/json", 0, 0);

		httpCode = http.GET();

		if (httpCode == HTTP_CODE_OK) {
			// HTTP header has been send and Server response header has been handled
			DEBUG.printf("[HTTP] GET SUCCESS\r\n");
			//String payload = https.getString();
			//Serial.println(payload);

			// Use ArduinoJSON to deserialize json response
			DynamicJsonDocument json(50 * 1024);
			DeserializationError error = deserializeJson(json, http.getStream());
			// Test if parsing succeeds.
			if (error) {
				Serial.print(F("deserializeJson() failed: "));
				Serial.println(error.c_str());
				ret = -1;
			} else {
				// convert it to a JsonObject
				JsonArray array = json.as < JsonArray > ();
				JsonObject root = array[0];

				strncpy(joke_string, root["setup"], MAX_JOKE_LENGTH);
				Serial.printf("Quote: %s\r\n", joke_string);

				strncpy(punchline_string, root["punchline"], MAX_PUNCHLINE_LENGTH);
				Serial.printf("Author: %s\r\n", punchline_string);
			}
		} else {
			DEBUG.printf("[HTTP] Failed, error: %d::%s\r\n", httpCode, http.errorToString(httpCode).c_str());
			ret = -1;
		}

		http.end();
	}

	DEBUG.printf("[HTTP] COMPLETED \r\n");

	delete client;
	return ret;
}

int32_t get_chars_words(GxEPD2_GFX& display, char *string, char *first_line, uint8_t margin)
{
	int16_t x = 0, y = 0;
	uint16_t w = 0, h = 0;
	int32_t length_count = 0;
	int32_t space_index = 0;
	char *temp = first_line;

	while (*string != '\0' && w <= display.width() - margin) {
		length_count++;
		*temp = *string;

		if (*string == ' ')
			space_index = length_count;

		*(temp + 1) = '\0';
		display.getTextBounds(first_line, 0, 0, &x, &y, &w, &h);
		temp++;
		string++;
	}

	if (w <= display.width() - margin)
		space_index = length_count;

	first_line[space_index] = '\0';

	return space_index;
}

void display_joke(GxEPD2_GFX& display)
{
	int16_t x, y;
	uint16_t w, h;
	char line[EXPECTED_LINE_CHARS];
	uint32_t first_line_chars, num_lines = 0;
	int32_t str_len, completed = 0;
	int battery_voltage = 0;
	const int MAX_ANALOG_VAL = 4095;
	const float MAX_BATTERY_VOLTAGE = 4.2; // Max LiPoly voltage of a 3.7 battery is 4.2
	str_len = strlen(joke_string);

	display.setFont(&Roboto_Regular12pt7b);
	display.setTextColor(GxEPD_BLACK);
	display.setTextSize(1);

	// Get height of the font
	display.getTextBounds(F("A"), 0, 0, &x, &y, &w, &h);
	num_lines = (str_len / LINE_HEIGHT_FACTOR) + 1;

	// Print Setup
	display.setCursor(JOKE_MARGIN_LEFT, (display.height() - (num_lines * h)) / 3);
	completed = get_chars_words(display, joke_string, line, (JOKE_MARGIN_LEFT + JOKE_MARGIN_RIGHT));
	display.println(line);

	while (completed < str_len) {
		completed += get_chars_words(display, &joke_string[completed], line, (JOKE_MARGIN_LEFT + JOKE_MARGIN_RIGHT));
		DEBUG.printf("Completed: %d/%d\r\n", completed, str_len);
		display.getTextBounds(line, 0, 0, &x, &y, &w, &h);
		display.setCursor(JOKE_MARGIN_LEFT + display.getCursorX(), display.getCursorY());
		display.println(line);
	}

	// Calculate battery level
	pinMode(BATT_VOLT, INPUT); // It is necessary to declare the input pin
	pinMode(BATT_EN, OUTPUT);
	digitalWrite(BATT_EN, LOW);
	delay(10);
	analogReadResolution(12);
	double rawValue = analogReadMilliVolts(BATT_VOLT);
	digitalWrite(BATT_EN, HIGH);
	// calculate battery voltage according to divider resistors
	// Vbat = (R1+R2)/R2 = (470000 + 1600000) / 1600000 = 1.29375
	float voltageLevel = ((double(rawValue) * 1.29375) / 1000);
	float batteryFraction = voltageLevel / MAX_BATTERY_VOLTAGE;
	float batteryLevel = batteryFraction * 100;
	int batt = int(batteryLevel);
	Serial.println((String)"Raw:" + rawValue + " Voltage:" + voltageLevel + "V Percent: " + batteryLevel + "%");

	// Print Punchline, set relative height
	display.setFont(&Gobold_Thin9pt7b);
	display.getTextBounds(punchline_string, 0, 0, &x, &y, &w, &h);
	display.setCursor((display.width() - w) / 2, display.getCursorY() + 40);
	display.println(punchline_string);

	// Print Battery Level in Lower Right
	display.setFont(&Code_New_Roman9pt7b);
	display.setCursor(display.width() - 40, display.height() - 12);
	display.println((String)(batt) + "%");
}
