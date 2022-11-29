
#ifndef _PAPERDINK_TODO_LIST_H_
#define _PAPERDINK_TODO_LIST_H_

#include "../../common.h"

#define DEBUG Serial

#define TODO_LIST_MAX_TASKS 10
#define TODO_LIST_MAX_TODO_STR_LENGTH 15

class PaperdinkUITodoListClass : public PaperdinkUIBaseClass
{
    public:
        char tasks[TODO_LIST_MAX_TASKS][TODO_LIST_MAX_TODO_STR_LENGTH+1];
        uint8_t task_count;

        void display_lrg_box(GxEPD2_GFX& display, int16_t x, int16_t y, uint16_t max_tasks = TODO_LIST_MAX_TASKS, uint16_t max_str_len = TODO_LIST_MAX_TODO_STR_LENGTH);
};

class PaperdinkUITodoListTaigaClass : public PaperdinkUITodoListClass
{
    public:
        String project_id; // Project ID in case of taiga
        String token; // API token in case of taiga

        int8_t fetch_data(const char *project_name, const char *project_username, const char *project_password);
};

extern PaperdinkUITodoListTaigaClass Paperdink_TodoListTaiga;

#endif /* _PAPERDINK_TODO_LIST_H_ */
