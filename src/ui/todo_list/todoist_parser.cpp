
#include "todo_list.h"
#include "todoist_parser.h"

enum TODOIST_STATE {NOT_FOUND, FOUND_TASK};
static TODOIST_STATE key_state;

void TodoTodoistJsonListener::whitespace(char c)
{
}

void TodoTodoistJsonListener::startDocument()
{
	Paperdink_TodoListTodoist.task_count = 0;
}

void TodoTodoistJsonListener::key(String key)
{
	if(key.equals("content") && Paperdink_TodoListTodoist.task_count < TODO_LIST_MAX_TASKS){
		key_state = FOUND_TASK;
	}
}

void TodoTodoistJsonListener::value(String value)
{
	if(key_state == FOUND_TASK){
        // Limit to TODO_LIST_MAX_TODO_STR_LENGTH to display properly on screen
        //DEBUG.printf("TASK: %s\r\n", value.c_str());
		strncpy(Paperdink_TodoListTodoist.tasks[Paperdink_TodoListTodoist.task_count], (char*)value.c_str(), TODO_LIST_MAX_TODO_STR_LENGTH);
		Paperdink_TodoListTodoist.tasks[Paperdink_TodoListTodoist.task_count][TODO_LIST_MAX_TODO_STR_LENGTH+1] = '\0';
		Paperdink_TodoListTodoist.task_count++;
		key_state = NOT_FOUND;
	}
}

void TodoTodoistJsonListener::endArray()
{
}

void TodoTodoistJsonListener::endObject()
{
}

void TodoTodoistJsonListener::endDocument()
{
}

void TodoTodoistJsonListener::startArray()
{
}

void TodoTodoistJsonListener::startObject()
{
}

