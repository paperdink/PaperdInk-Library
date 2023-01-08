
#include <WiFiClientSecure.h>
#include <HTTPClient.h>

#include "todo_list.h"
#include "taiga_parser.h"
#include "todoist_parser.h"

// Taiga
String taiga_auth_url = "https://api.taiga.io/api/v1/auth";
String taiga_slug_url = "https://api.taiga.io/api/v1/projects/by_slug?slug={0}-{1}";
String taiga_tasks_url = "https://api.taiga.io/api/v1/userstories?include_tasks=1&status__is_archived=false&project=";
String taiga_post_data = "{ \"type\": \"normal\", \"username\": \"{0}\", \"password\": \"{1}\" }";
TodoTaigaJsonListener taiga_todo_listener;

// Valid till 14 Apr 2023
static const char* taiga_cert = "-----BEGIN CERTIFICATE-----\n" \
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

// Todoist
String todoist_url = "https://api.todoist.com/rest/v2/tasks";
TodoTodoistJsonListener todoist_todo_listener;

// Valid till 9 July 2023
static const char* todoist_cert = "-----BEGIN CERTIFICATE-----\n" \
"MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF\n" \
"ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6\n" \
"b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL\n" \
"MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv\n" \
"b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj\n" \
"ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM\n" \
"9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw\n" \
"IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6\n" \
"VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L\n" \
"93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm\n" \
"jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC\n" \
"AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA\n" \
"A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI\n" \
"U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs\n" \
"N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv\n" \
"o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU\n" \
"5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy\n" \
"rqXRfboQnoZsG4q5WTP468SQvvG5\n" \
"-----END CERTIFICATE-----\n";

int8_t PaperdinkUITodoListTaigaClass::fetch_data(const char *project_name, const char *project_username, const char *project_password)
{
    int httpCode, ret = 0;
    String payload;
    
    WiFiClientSecure *client = new WiFiClientSecure;
    client->setCACert(taiga_cert);

    {
        HTTPClient https;
        /* Authenticate with Taiga server */
        https.begin(*client, taiga_auth_url.c_str());
        https.addHeader("Content-Type", "application/json", 0, 0);

        taiga_post_data.replace("{0}", project_username);
        taiga_post_data.replace("{1}", project_password);
        httpCode = https.POST(taiga_post_data);

        if(httpCode == HTTP_CODE_OK){
            DEBUG.printf("[HTTP] AUTH SUCCESS\r\n");
            //String payload = https.getString();
            //Serial.println(payload);
            ArudinoStreamParser parser;
            parser.setListener(&taiga_todo_listener);
            https.writeToStream(&parser);
        } else {
            DEBUG.printf("[HTTP] AUTH... failed, error: %s\r\n", https.errorToString(httpCode).c_str());
            ret = -1;
        }

        https.end();
    }

    if(ret != 0){
        delete client;
        return ret;
    }

    /* Get the slug */
    taiga_slug_url.replace("{0}", project_username);
    taiga_slug_url.replace("{1}", project_name);

    {
        HTTPClient https;
        https.begin(*client, taiga_slug_url.c_str());
        https.addHeader("Authorization", token, 0, 0);
        httpCode = https.GET();

        if(httpCode == HTTP_CODE_OK){
            DEBUG.printf("[HTTP] SLUG SUCCESS\r\n");
            //payload = https.getString();
            //DEBUG.println(payload);
            ArudinoStreamParser parser;
            parser.setListener(&taiga_todo_listener);
            https.writeToStream(&parser);
        } else {
            DEBUG.printf("[HTTP] SLUG... failed, error: %s\r\n", https.errorToString(httpCode).c_str());
            ret = -1;
        }
        https.end();
    }
    
    if(ret != 0){
        delete client;
        return ret;
    }

    /* Get the tasks list */
    taiga_tasks_url += project_id;
    
    {
        HTTPClient https;
        https.begin(*client, taiga_tasks_url.c_str());
        https.addHeader("Authorization", token, 0, 0);
        httpCode = https.GET();

        if(httpCode == HTTP_CODE_OK){
            DEBUG.printf("[HTTP] TASKS SUCCESS\r\n");
            //payload = https.getString();
            //DEBUG.println(payload);
            ArudinoStreamParser parser;
            parser.setListener(&taiga_todo_listener);
            https.writeToStream(&parser);
        } else {
            DEBUG.printf("[HTTP] TASKS... failed, error: %s\r\n", https.errorToString(httpCode).c_str());
        }

        https.end();
    }

    delete client;
    return ret;
}

int8_t PaperdinkUITodoListTodoistClass::fetch_data(const char *token)
{
    int httpCode, ret = 0;
    String payload;

    WiFiClientSecure *client = new WiFiClientSecure;
    client->setCACert(todoist_cert);

    {
        HTTPClient https;
        /* Authenticate with Todoist server */
        https.begin(*client, todoist_url.c_str());
        https.addHeader("Content-Type", "application/json", 0, 0);
        https.addHeader("Authorization", token, 0, 0);

        httpCode = https.GET();

        if(httpCode == HTTP_CODE_OK){
            DEBUG.printf("[HTTP] AUTH SUCCESS\r\n");
            //String payload = https.getString();
            //Serial.println(payload);
            ArudinoStreamParser parser;
            parser.setListener(&todoist_todo_listener);
            https.writeToStream(&parser);
        } else {
            DEBUG.printf("[HTTP] AUTH... failed, error: %d::%s\r\n", httpCode, https.errorToString(httpCode).c_str());
            ret = -1;
        }

        https.end();
    }
    
    delete client;
    return ret;
}

void PaperdinkUITodoListClass::display_list_style1(GxEPD2_GFX& display, int16_t x, int16_t y, int16_t rows, int16_t columns, uint16_t max_line_width)
{
    int16_t xt, yt;
    uint16_t wt, ht, char_ht, prev_height = y, prev_width = x;
    uint16_t i = 0, j = 0, start_height = 0;

    display.setFont(font_bold);
    display.setTextColor(primary_color);
    display.setTextSize(1);

    display.getTextBounds(F("To-Do List"), 0, 0, &xt, &yt, &wt, &ht);
    display.setCursor(prev_width+5, prev_height+ht);
    prev_height += ht;
    prev_width += 5;
    display.println(F("To-Do List"));

    display.setFont(font);
    display.setTextColor(primary_color);
    display.getTextBounds("W", 0, 0, &xt, &yt, &wt, &char_ht);

    // Display tasks
    start_height = prev_height + ht + 5;
    for(j = 0; j < columns; j++){
        prev_height = start_height;
        for(i = 0; i+(j*rows) < task_count && i < rows; i++){
            String task = tasks[i+(j*rows)];
            uint16_t cnt = task.length();
            do{
                task = task.substring(0, cnt);
                display.getTextBounds(task.c_str(), 0, 0, &xt, &yt, &wt, &ht);
            }while(wt > max_line_width && cnt-- > 0);

            DEBUG.printf("Task: %s\r\n", task.c_str());
            display.setCursor(prev_width, prev_height);
            display.printf("%s", task.c_str());
            prev_height += char_ht + 7;
        }
        prev_width += max_line_width + 10;
    }
}

PaperdinkUITodoListTaigaClass Paperdink_TodoListTaiga;
PaperdinkUITodoListTodoistClass Paperdink_TodoListTodoist;

