
#include <WiFiClientSecure.h>
#include <HTTPClient.h>

#include "weather.h"
#include "weather_parser.h"

WeatherJsonListener weather_listener;
static String owm_url = "https://api.openweathermap.org/data/2.5/forecast?q={0},{1}&appid={2}&mode=json&cnt=1";

// Valid till 07 Jul 2023
static const char* cert = "-----BEGIN CERTIFICATE-----\n" \
"MIIF3jCCA8agAwIBAgIQAf1tMPyjylGoG7xkDjUDLTANBgkqhkiG9w0BAQwFADCB\n" \
"iDELMAkGA1UEBhMCVVMxEzARBgNVBAgTCk5ldyBKZXJzZXkxFDASBgNVBAcTC0pl\n" \
"cnNleSBDaXR5MR4wHAYDVQQKExVUaGUgVVNFUlRSVVNUIE5ldHdvcmsxLjAsBgNV\n" \
"BAMTJVVTRVJUcnVzdCBSU0EgQ2VydGlmaWNhdGlvbiBBdXRob3JpdHkwHhcNMTAw\n" \
"MjAxMDAwMDAwWhcNMzgwMTE4MjM1OTU5WjCBiDELMAkGA1UEBhMCVVMxEzARBgNV\n" \
"BAgTCk5ldyBKZXJzZXkxFDASBgNVBAcTC0plcnNleSBDaXR5MR4wHAYDVQQKExVU\n" \
"aGUgVVNFUlRSVVNUIE5ldHdvcmsxLjAsBgNVBAMTJVVTRVJUcnVzdCBSU0EgQ2Vy\n" \
"dGlmaWNhdGlvbiBBdXRob3JpdHkwggIiMA0GCSqGSIb3DQEBAQUAA4ICDwAwggIK\n" \
"AoICAQCAEmUXNg7D2wiz0KxXDXbtzSfTTK1Qg2HiqiBNCS1kCdzOiZ/MPans9s/B\n" \
"3PHTsdZ7NygRK0faOca8Ohm0X6a9fZ2jY0K2dvKpOyuR+OJv0OwWIJAJPuLodMkY\n" \
"tJHUYmTbf6MG8YgYapAiPLz+E/CHFHv25B+O1ORRxhFnRghRy4YUVD+8M/5+bJz/\n" \
"Fp0YvVGONaanZshyZ9shZrHUm3gDwFA66Mzw3LyeTP6vBZY1H1dat//O+T23LLb2\n" \
"VN3I5xI6Ta5MirdcmrS3ID3KfyI0rn47aGYBROcBTkZTmzNg95S+UzeQc0PzMsNT\n" \
"79uq/nROacdrjGCT3sTHDN/hMq7MkztReJVni+49Vv4M0GkPGw/zJSZrM233bkf6\n" \
"c0Plfg6lZrEpfDKEY1WJxA3Bk1QwGROs0303p+tdOmw1XNtB1xLaqUkL39iAigmT\n" \
"Yo61Zs8liM2EuLE/pDkP2QKe6xJMlXzzawWpXhaDzLhn4ugTncxbgtNMs+1b/97l\n" \
"c6wjOy0AvzVVdAlJ2ElYGn+SNuZRkg7zJn0cTRe8yexDJtC/QV9AqURE9JnnV4ee\n" \
"UB9XVKg+/XRjL7FQZQnmWEIuQxpMtPAlR1n6BB6T1CZGSlCBst6+eLf8ZxXhyVeE\n" \
"Hg9j1uliutZfVS7qXMYoCAQlObgOK6nyTJccBz8NUvXt7y+CDwIDAQABo0IwQDAd\n" \
"BgNVHQ4EFgQUU3m/WqorSs9UgOHYm8Cd8rIDZsswDgYDVR0PAQH/BAQDAgEGMA8G\n" \
"A1UdEwEB/wQFMAMBAf8wDQYJKoZIhvcNAQEMBQADggIBAFzUfA3P9wF9QZllDHPF\n" \
"Up/L+M+ZBn8b2kMVn54CVVeWFPFSPCeHlCjtHzoBN6J2/FNQwISbxmtOuowhT6KO\n" \
"VWKR82kV2LyI48SqC/3vqOlLVSoGIG1VeCkZ7l8wXEskEVX/JJpuXior7gtNn3/3\n" \
"ATiUFJVDBwn7YKnuHKsSjKCaXqeYalltiz8I+8jRRa8YFWSQEg9zKC7F4iRO/Fjs\n" \
"8PRF/iKz6y+O0tlFYQXBl2+odnKPi4w2r78NBc5xjeambx9spnFixdjQg3IM8WcR\n" \
"iQycE0xyNN+81XHfqnHd4blsjDwSXWXavVcStkNr/+XeTWYRUc+ZruwXtuhxkYze\n" \
"Sf7dNXGiFSeUHM9h4ya7b6NnJSFd5t0dCy5oGzuCr+yDZ4XUmFF0sbmZgIn/f3gZ\n" \
"XHlKYC6SQK5MNyosycdiyA5d9zZbyuAlJQG03RoHnHcAP9Dc1ew91Pq7P8yF1m9/\n" \
"qS3fuQL39ZeatTXaw2ewh0qpKJ4jjv9cJ2vhsE/zB+4ALtRZh8tSQZXq9EfX7mRB\n" \
"VXyNWQKV3WKdwrnuWih0hKWbt5DHDAff9Yk2dDLWKMGwsAvgnEzDHNb842m1R0aB\n" \
"L6KCq9NjRHDEjf8tM7qtj3u1cIiuPhnPQCjY/MiQu12ZIvVS5ljFH4gxQ+6IHdfG\n" \
"jjxDah2nGN59PRbxYvnKkKj9\n" \
"-----END CERTIFICATE-----\n";

int8_t PaperdinkUIWeatherClass::fetch_data(const char* city, const char* country, const char* owm_api_id, const char* units)
{
    int httpCode, ret = 0;
    
    WiFiClientSecure *client = new WiFiClientSecure;
    client->setCACert(cert);

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

    {
        HTTPClient https;
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
    }
    
    DEBUG.printf("[HTTP] COMPLETED \r\n");

    delete client;
    return ret;
}

void PaperdinkUIWeatherClass::display_weather_text_center(GxEPD2_GFX& display, uint16_t x, uint16_t y, uint16_t w)
{
    int16_t xt, yt;
    uint16_t wt, ht;

    display.setFont(font);
    display.setTextColor(primary_color);
    display.getTextBounds(Paperdink_Weather.weather_string, 0, 0, &xt, &yt, &wt, &ht);
    display.setCursor(x+((w-wt)/2), y+ht);
    display.print(weather_string);
}

void PaperdinkUIWeatherClass::display_weather_style1(GxEPD2_GFX& display, uint16_t x, uint16_t y)
{
    int16_t xt, yt;
    uint16_t wt, ht, prev_width = x, prev_height = y;

    display.setFont(font);
    display.setTextColor(primary_color);

    // Show min and max temperature
    display.drawBitmap(prev_width, prev_height, thermometer_sml, thermometer_sml_width, thermometer_sml_height, primary_color); 
    prev_width += thermometer_sml_width;

    display.getTextBounds("77", 0, 0, &xt, &yt, &wt, &ht);
    display.setCursor(prev_width+10, prev_height+ht+2);
    display.printf("%d / %d", (int16_t)Paperdink_Weather.day_max_temp, (int16_t)Paperdink_Weather.day_min_temp);
    prev_height += thermometer_sml_height;

    prev_width = x; // Reset width for next line

    // TODO: OWM API provides rain/snow in mm only. Convert to in
    // Show rain
    display.drawBitmap(prev_width, prev_height+10, umbrella_sml, umbrella_sml_width, umbrella_sml_height, primary_color); 
    prev_width += umbrella_sml_width;

    display.setCursor(prev_width+10, prev_height+10+ht+2);
    display.printf("%.2f mm", Paperdink_Weather.rain);
    prev_height += umbrella_sml_height+10;

    prev_width = x; // Reset width for next line

    // Show snow
    display.drawBitmap(prev_width, prev_height+10, snowflake_sml, snowflake_sml_width, snowflake_sml_height, primary_color); 
    prev_width += snowflake_sml_width;

    display.setCursor(prev_width+10, prev_height+10+ht+2);
    display.printf("%.2f mm", Paperdink_Weather.snow);

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

    display.drawBitmap(x + (w - alert_xlrg_width)/2, y, bitmap, alert_xlrg_width, alert_xlrg_height, primary_color);
}

PaperdinkUIWeatherClass Paperdink_Weather;

