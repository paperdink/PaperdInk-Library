
#ifndef _TODOIST_PARSER_H_
#define _TODOIST_PARSER_H_

#include <JsonListener.h>

class TodoTodoistJsonListener : public JsonListener {

    public:
        virtual void whitespace(char c);

        virtual void startDocument();

        virtual void key(String key);

        virtual void value(String value);

        virtual void endArray();

        virtual void endObject();

        virtual void endDocument();

        virtual void startArray();

        virtual void startObject();
};

#endif /* _TAIGA_PARSER_H_ */
