
#include <WiFiClientSecure.h>
#include "base.h"

int8_t Paperdink_Base_Class::begin()
{
	pinMode(EPD_EN, OUTPUT);
	pinMode(EPD_RST, OUTPUT);
	pinMode(SD_EN, OUTPUT);
	pinMode(BATT_EN, OUTPUT);
	pinMode(PCF_INT, INPUT); // Required to lower power consumption

	return 0;
}

int8_t Paperdink_Base_Class::enable_display()
{
    // Power up EPD
	digitalWrite(EPD_EN, LOW);
	digitalWrite(EPD_RST, LOW);
	delay(50);
	digitalWrite(EPD_RST, HIGH);
	delay(50);

	return 0;
}

int8_t Paperdink_Base_Class::disable_display()
{
	digitalWrite(EPD_EN, HIGH);
	digitalWrite(EPD_RST, LOW);
	delay(50);
	digitalWrite(EPD_RST, HIGH);

	return 0;
}

int8_t Paperdink_Base_Class::disable_everything()
{
	disable_display();
    // TODO: Add disable functions for SD and Batt
	digitalWrite(SD_EN, HIGH);
	digitalWrite(BATT_EN, HIGH);

	return 0;
}

int8_t Paperdink_Base_Class::deep_sleep_timer_wakeup(uint64_t sleep_time_us)
{
    // Turn off everything
	disable_everything();

	esp_sleep_enable_timer_wakeup(sleep_time_us);
	Serial.printf("Timer wakeup after %lld microseconds...", sleep_time_us);
    // Go to sleep
	esp_deep_sleep_start();

	return 0;
}

int8_t Paperdink_Base_Class::deep_sleep_button_wakeup(uint8_t gpio_num)
{
    // Turn off everything
	disable_everything();

	esp_sleep_enable_ext0_wakeup((gpio_num_t)gpio_num, 0); //1 = High, 0 = Low
	Serial.printf("Button wakeup on pin %d", gpio_num);
    // Go to sleep
	esp_deep_sleep_start();

	return 0;
}

int8_t Paperdink_Base_Class::deep_sleep_timer_button_wakeup(uint64_t sleep_time_us, uint8_t gpio_num)
{
    // Turn off everything
	disable_everything();

	esp_sleep_enable_timer_wakeup(sleep_time_us);
	esp_sleep_enable_ext0_wakeup((gpio_num_t)gpio_num, 0); //1 = High, 0 = Low
	Serial.printf("Timer wakeup after %lld us or button on pin %d", sleep_time_us, gpio_num);
    // Go to sleep
	esp_deep_sleep_start();

	return 0;
}

int8_t Paperdink_Base_Class::connect_wifi(const char *ssid, const char *password, uint8_t attempts)
{

	WiFi.begin(ssid, password);

	while(WiFi.status() != WL_CONNECTED ){
		delay(500);
		Serial.print(".");
		if(!attempts--){
			return -1;
		}
	}
	Serial.println("Connected");

	return 0;
}

int8_t Paperdink_Base_Class::connect_wifi(const char *ssid, const char *password)
{
	return connect_wifi(ssid, password, 40);
}
