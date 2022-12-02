
#include <WiFiClientSecure.h>
#include <HTTPClient.h>

#include "weather.h"
#include "weather_parser.h"

// RGREMOVE:
#include <StreamUtils.h>

WeatherJsonListener weather_listener;
static String owm_url = "https://api.openweathermap.org/data/2.5/forecast?q={0},{1}&appid={2}&mode=json&cnt=1";

int8_t PaperdinkUIWeatherClass::fetch_data(const char* city, const char* country, const char* owm_api_id, const char* units)
{
    int httpCode, ret = 0;
    HTTPClient https;
    WiFiClientSecure *client = new WiFiClientSecure;

    snprintf(Paperdink_Weather.city, 10, "%s", city);
    snprintf(Paperdink_Weather.country, 10, "%s", country);
    
    owm_url.replace("{0}", city);
    owm_url.replace("{1}", country);
    owm_url.replace("{2}", owm_api_id);

    if(units[0] == 'M'){
        owm_url += "&units=metric";
    } else {
        owm_url += "&units=imperial";
    }

    DEBUG.printf("OWM URL: %s\r\n", owm_url.c_str());

    https.begin(*client, owm_url.c_str());
    https.addHeader("Content-Type", "application/json", 0, 0);

    httpCode = https.GET();

    if(httpCode == HTTP_CODE_OK){ // Check for the return code
                                  // Parser updates weather_string directly
        DEBUG.printf("[HTTP] GET SUCCESS\r\n");
        DynamicJsonDocument json(35 * 1024);

        // Deserialize the JSON document
        // ReadLoggingStream can be used to debug the stream
        //ReadLoggingStream loggingStream(https.getStream(), Serial);
        //DeserializationError error = deserializeJson(json, loggingStream);
        DeserializationError error = deserializeJson(json, https.getStream());
        // Test if parsing succeeds.
        if (error) {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.c_str());
            ret = -1;
        } else {
            // convert it to a JsonObject
            JsonObject root = json.as<JsonObject>();

            snprintf(Paperdink_Weather.weather_string, 10, "%s", root["list"][0]["weather"][0]["main"].as<const char*>());
            Paperdink_Weather.day_max_temp = root["list"][0]["main"]["temp_max"].as<float>();
            Paperdink_Weather.day_min_temp = root["list"][0]["main"]["temp_min"].as<float>();
            Paperdink_Weather.rain = root["list"][0]["rain"]["1h"].as<float>();
            Paperdink_Weather.snow = root["list"][0]["snow"]["1h"].as<float>(); 
            if(Paperdink_Weather.rain == 0){
                Paperdink_Weather.rain = root["list"][0]["rain"]["3h"].as<float>();
            }
            if(Paperdink_Weather.snow == 0){
                Paperdink_Weather.snow = root["list"][0]["snow"]["3h"].as<float>();
            }
        }

        //ArudinoStreamParser parser;
        //parser.setListener(&weather_listener);
        //https.writeToStream(&parser);
    } else {
        DEBUG.printf("[HTTP] Failed, error: %s\r\n", https.errorToString(httpCode).c_str());
        ret = -1;
    }

    https.end();
    DEBUG.printf("[HTTP] COMPLETED \r\n");

    return ret;
}

void PaperdinkUIWeatherClass::display_weather_text_center(GxEPD2_GFX& display, uint16_t x, uint16_t y, uint16_t w)
{
    int16_t xt, yt;
    uint16_t wt, ht;

    display.setFont(font);
    display.setTextColor(GxEPD_BLACK);
    display.getTextBounds(Paperdink_Weather.weather_string, 0, 0, &xt, &yt, &wt, &ht);
    display.setCursor(x+((w-wt)/2), y+ht);
    display.print(weather_string);
}

void PaperdinkUIWeatherClass::display_weather_style1(GxEPD2_GFX& display, uint16_t x, uint16_t y)
{
    int16_t xt, yt;
    uint16_t wt, ht, prev_width = x, prev_height = y;

    display.setFont(font);
    display.setTextColor(GxEPD_BLACK);

    // Show min and max temperature
    display.drawBitmap(prev_width, prev_height, thermometer_sml, thermometer_sml_width, thermometer_sml_height, GxEPD_BLACK); 
    prev_width += thermometer_sml_width;

    display.getTextBounds("77", 0, 0, &xt, &yt, &wt, &ht);
    display.setCursor(prev_width+10, prev_height+ht+2);
    display.printf("%d / %d", (int16_t)Paperdink_Weather.day_max_temp, (int16_t)Paperdink_Weather.day_min_temp);
    prev_height += thermometer_sml_height;

    prev_width = x; // Reset width for next line

    // Show rain
    display.drawBitmap(prev_width, prev_height+10, umbrella_sml, umbrella_sml_width, umbrella_sml_height, GxEPD_BLACK); 
    prev_width += umbrella_sml_width;

    display.setCursor(prev_width+10, prev_height+10+ht+2);
    display.printf("%d", (int16_t)Paperdink_Weather.rain);
    prev_height += umbrella_sml_height+10;

    prev_width = x; // Reset width for next line

    // Show snow
    display.drawBitmap(prev_width, prev_height+10, snowflake_sml, snowflake_sml_width, snowflake_sml_height, GxEPD_BLACK); 
    prev_width += snowflake_sml_width;

    display.setCursor(prev_width+10, prev_height+10+ht+2);
    display.printf("%d", (int16_t)Paperdink_Weather.snow);

}

void PaperdinkUIWeatherClass::display_weather_style2_center(GxEPD2_GFX& display, uint16_t x, uint16_t y, uint16_t w)
{
    const uint8_t* bitmap;

    if (!strcmp(weather_string, "Drizzle")){
        bitmap = drizzle_xlrg;
    } else if (!strcmp(Paperdink_Weather.weather_string, "Thunderstorm") || !strcmp(Paperdink_Weather.weather_string, "Lightning")) {
        bitmap = lightning_xlrg;
    } else if (!strcmp(Paperdink_Weather.weather_string, "Rain")) {
        bitmap = rain_xlrg;
    } else if (!strcmp(Paperdink_Weather.weather_string, "Snow")) {
        bitmap = snow_xlrg;
    } else if (!strcmp(Paperdink_Weather.weather_string, "Clear")) {
        bitmap = sun_xlrg;
    } else if (!strcmp(Paperdink_Weather.weather_string, "Clouds")) {
        bitmap = cloud_xlrg;
    } else {
        bitmap = alert_xlrg;
        DEBUG.printf("Invalid weather: %s\r\n", Paperdink_Weather.weather_string);
    }

    display.drawBitmap(x + (w - alert_xlrg_width)/2, y, bitmap, alert_xlrg_width, alert_xlrg_height, GxEPD_BLACK);
}

PaperdinkUIWeatherClass Paperdink_Weather;

