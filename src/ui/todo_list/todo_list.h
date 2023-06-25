#ifndef _PAPERDINK_TODO_LIST_H_
#define _PAPERDINK_TODO_LIST_H_

#include "../../paperdink_common.h"

#define DEBUG Serial

#define TODO_LIST_MAX_TASKS 30
#define TODO_LIST_MAX_TODO_STR_LENGTH 30

class PaperdinkUITodoListClass : public PaperdinkUIBaseClass
{
public:
char tasks[TODO_LIST_MAX_TASKS][TODO_LIST_MAX_TODO_STR_LENGTH + 1];
uint8_t task_count;
String token;

void display_list_style1(GxEPD2_GFX& display, int16_t x, int16_t y, int16_t rows = TODO_LIST_MAX_TASKS, int16_t columns = 1, uint16_t max_line_width = 400);
};

class PaperdinkUITodoListTaigaClass : public PaperdinkUITodoListClass
{
public:
String project_id;         // Project ID in case of taiga

int8_t fetch_data(const char *project_name, const char *project_username, const char *project_password);
};

class PaperdinkUITodoListTodoistClass : public PaperdinkUITodoListClass
{
public:

int8_t fetch_data(const char *token);
};

extern PaperdinkUITodoListTaigaClass Paperdink_TodoListTaiga;
extern PaperdinkUITodoListTodoistClass Paperdink_TodoListTodoist;

#endif /* _PAPERDINK_TODO_LIST_H_ */
