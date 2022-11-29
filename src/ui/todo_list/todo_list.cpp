
#include <WiFiClientSecure.h>
#include <HTTPClient.h>

#include "todo_list.h"
#include "taiga_parser.h"

String auth_url = "https://api.taiga.io/api/v1/auth";
String slug_url = "https://api.taiga.io/api/v1/projects/by_slug?slug={0}-{1}";
String tasks_url = "https://api.taiga.io/api/v1/userstories?include_tasks=1&status__is_archived=false&project=";
String post_data = "{ \"type\": \"normal\", \"username\": \"{0}\", \"password\": \"{1}\" }";

TodoTaigaJsonListener todo_listener;

int8_t PaperdinkUITodoListTaigaClass::fetch_data(const char *project_name, const char *project_username, const char *project_password)
{
    int httpCode, ret = 0;
    String payload;
    HTTPClient https;
    WiFiClientSecure *client = new WiFiClientSecure;

    /* Authenticate with Taiga server */
    https.begin(*client, auth_url.c_str());
    https.addHeader("Content-Type", "application/json", 0, 0);

    post_data.replace("{0}", project_username);
    post_data.replace("{1}", project_password);
    httpCode = https.POST(post_data);

    if(httpCode == HTTP_CODE_OK){
        DEBUG.printf("[HTTP] AUTH SUCCESS\r\n");
        //String payload = https.getString();
        //Serial.println(payload);
        ArudinoStreamParser parser;
        parser.setListener(&todo_listener);
        https.writeToStream(&parser);
    } else {
        DEBUG.printf("[HTTP] AUTH... failed, error: %s\r\n", https.errorToString(httpCode).c_str());
        ret = -1;
    }

    https.end();

    if(ret != 0){
        return ret;
    }

    /* Get the slug */
    slug_url.replace("{0}", project_username);
    slug_url.replace("{1}", project_name);

    https.begin(*client, slug_url.c_str());
    https.addHeader("Authorization", token, 0, 0);
    httpCode = https.GET();

    if(httpCode == HTTP_CODE_OK){
        DEBUG.printf("[HTTP] SLUG SUCCESS\r\n");
        //payload = https.getString();
        //DEBUG.println(payload);
        ArudinoStreamParser parser;
        parser.setListener(&todo_listener);
        https.writeToStream(&parser);
    } else {
        DEBUG.printf("[HTTP] SLUG... failed, error: %s\r\n", https.errorToString(httpCode).c_str());
        ret = -1;
    }
    https.end();

    if(ret != 0){
        return ret;
    }

    /* Get the tasks list */
    tasks_url += project_id;
    https.begin(*client, tasks_url.c_str());
    https.addHeader("Authorization", token, 0, 0);
    httpCode = https.GET();

    if(httpCode == HTTP_CODE_OK){
        DEBUG.printf("[HTTP] TASKS SUCCESS\r\n");
        //payload = https.getString();
        //DEBUG.println(payload);
        ArudinoStreamParser parser;
        parser.setListener(&todo_listener);
        https.writeToStream(&parser);
    } else {
        DEBUG.printf("[HTTP] TASKS... failed, error: %s\r\n", https.errorToString(httpCode).c_str());
    }

    https.end();

    return ret;
}

void PaperdinkUITodoListClass::display_lrg_box(GxEPD2_GFX& display, int16_t x, int16_t y, uint16_t max_tasks, uint16_t max_str_len)
{
    int16_t xt, yt;
    uint16_t wt, ht, prev_height = 0, prev_width = 0;

    display.setFont(font_bold);
    display.setTextColor(GxEPD_BLACK);
    display.setTextSize(1);

    display.getTextBounds(F("To-Do List"), 0, 0, &xt, &yt, &wt, &ht);
    display.setCursor(x+5, y+prev_height+ht);
    prev_height += ht;
    display.println(F("To-Do List"));

    display.setFont(font);
    display.setTextColor(GxEPD_BLACK);
    display.setTextSize(1);
    display.getTextBounds(F("item"), 0, 0, &xt, &yt, &wt, &ht);

    // Display tasks
    for(int i = 0; i < task_count; i++){
        display.setCursor(x+5, y+prev_height+(ht+7)*i+20);
        DEBUG.printf("Task: %s\r\n", tasks[i]);
        display.print((char*)tasks[i]);
    }
}

PaperdinkUITodoListTaigaClass Paperdink_TodoListTaiga;

