#include "todo_list.h"
#include "taiga_parser.h"

enum TAIGA_STATE { NOT_FOUND, FOUND_TASK, FOUND_ID, FOUND_TOKEN, W_ID, FOUND_TIME, FOUND_WEATHER, FOUND_MAIN };
static TAIGA_STATE key_state;

static bool id_found = false;

void TodoTaigaJsonListener::whitespace(char c)
{
}

void TodoTaigaJsonListener::startDocument()
{
	Paperdink_TodoListTaiga.task_count = 0;
}

void TodoTaigaJsonListener::key(String key)
{
	if (key.equals("auth_token"))
		key_state = FOUND_TOKEN;

	if (key_state == W_ID && key.equals("id")) {
		DEBUG.println("Got id");
		key_state = FOUND_ID;
	}

	if (Paperdink_TodoListTaiga.task_count < TODO_LIST_MAX_TASKS) {
		if (id_found && key.equals("subject"))
			key_state = FOUND_TASK;
	}
}

void TodoTaigaJsonListener::value(String value)
{
	if (key_state == FOUND_TOKEN) {
		Paperdink_TodoListTaiga.token = "Bearer ";
		Paperdink_TodoListTaiga.token += value;
		key_state = W_ID;
	}

	if (!id_found && key_state == FOUND_ID) {
		id_found = true;
		DEBUG.println(value);
		Paperdink_TodoListTaiga.project_id = value;
		key_state = NOT_FOUND;
	}

	if (id_found && key_state == FOUND_TASK) {
		// TODO: Figure out a way to directly print to display
		// Limit to TODO_LIST_MAX_TODO_STR_LENGTH to display properly on screen
		// DEBUG.printf("TASK: %s\r\n", value.c_str());
		strncpy(Paperdink_TodoListTaiga.tasks[Paperdink_TodoListTaiga.task_count], (char *)value.c_str(), TODO_LIST_MAX_TODO_STR_LENGTH);
		Paperdink_TodoListTaiga.tasks[Paperdink_TodoListTaiga.task_count][TODO_LIST_MAX_TODO_STR_LENGTH + 1] = '\0';
		Paperdink_TodoListTaiga.task_count++;
		key_state = NOT_FOUND;
	}
}

void TodoTaigaJsonListener::endArray()
{
}

void TodoTaigaJsonListener::endObject()
{
}

void TodoTaigaJsonListener::endDocument()
{
}

void TodoTaigaJsonListener::startArray()
{
}

void TodoTaigaJsonListener::startObject()
{
}
