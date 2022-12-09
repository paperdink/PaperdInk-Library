
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

// Todoist
String todoist_url = "https://api.todoist.com/rest/v2/tasks";
TodoTodoistJsonListener todoist_todo_listener;

int8_t PaperdinkUITodoListTaigaClass::fetch_data(const char *project_name, const char *project_username, const char *project_password)
{
    int httpCode, ret = 0;
    String payload;
    HTTPClient https;
    WiFiClientSecure *client = new WiFiClientSecure;

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

    if(ret != 0){
        return ret;
    }

    /* Get the slug */
    taiga_slug_url.replace("{0}", project_username);
    taiga_slug_url.replace("{1}", project_name);

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

    if(ret != 0){
        return ret;
    }

    /* Get the tasks list */
    taiga_tasks_url += project_id;
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

    return ret;
}

int8_t PaperdinkUITodoListTodoistClass::fetch_data(const char *token)
{
    int httpCode, ret = 0;
    String payload;
    HTTPClient https;
    WiFiClientSecure *client = new WiFiClientSecure;

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

    return ret;
}

//void PaperdinkUITodoListClass::display_lrg_box(GxEPD2_GFX& display, int16_t x, int16_t y, uint16_t max_tasks, uint16_t max_str_len)
//{
//    int16_t xt, yt;
//    uint16_t wt, ht, prev_height = 0, prev_width = 0;
//
//    display.setFont(font_bold);
//    display.setTextColor(GxEPD_BLACK);
//    display.setTextSize(1);
//
//    display.getTextBounds(F("To-Do List"), 0, 0, &xt, &yt, &wt, &ht);
//    display.setCursor(x+5, y+prev_height+ht);
//    prev_height += ht;
//    display.println(F("To-Do List"));
//
//    display.setFont(font);
//    display.setTextColor(GxEPD_BLACK);
//    display.setTextSize(1);
//    display.getTextBounds(F("item"), 0, 0, &xt, &yt, &wt, &ht);
//
//    // Display tasks
//    for(int i = 0; i < task_count; i++){
//        display.setCursor(x+5, y+prev_height+(ht+7)*i+20);
//        DEBUG.printf("Task: %s\r\n", tasks[i]);
//        display.print((char*)tasks[i]);
//    }
//}

void PaperdinkUITodoListClass::display_list_style1(GxEPD2_GFX& display, int16_t x, int16_t y, int16_t rows, int16_t columns, uint16_t max_line_width)
{
    int16_t xt, yt;
    uint16_t wt, ht, char_ht, prev_height = y, prev_width = x;
    uint16_t i = 0, j = 0, start_height = 0;

    display.setFont(font_bold);
    display.setTextColor(GxEPD_WHITE);
    display.setTextSize(1);

    display.getTextBounds(F("To-Do List"), 0, 0, &xt, &yt, &wt, &ht);
    display.setCursor(prev_width+5, prev_height+ht);
    prev_height += ht;
    prev_width += 5;
    display.println(F("To-Do List"));

    display.setFont(font);
    display.setTextColor(GxEPD_WHITE);
    display.setTextSize(1);

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

