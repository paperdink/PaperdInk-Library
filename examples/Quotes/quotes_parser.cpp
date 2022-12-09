
#include "config.h"
#include "quotes_parser.h"

enum STATE {NOT_FOUND, FOUND_CONTENT, FOUND_AUTHOR};
static STATE key_state;

extern char quote_string[MAX_QUOTE_LENGTH];
extern char author_string[MAX_AUTHOR_LENGTH];

void quoteListener::whitespace(char c)
{
}

void quoteListener::startDocument()
{
    key_state = NOT_FOUND;
}

void quoteListener::key(String key)
{
    if(key.equals("content")){
        key_state = FOUND_CONTENT;
    } else if(key.equals("author")){
        key_state = FOUND_AUTHOR;
    }
}

void quoteListener::value(String value)
{
    if(key_state == FOUND_CONTENT){
        strncpy(quote_string, (char*)value.c_str(), MAX_QUOTE_LENGTH);
        DEBUG.printf("Quote: %s\r\n", quote_string);
        key_state = NOT_FOUND;
    } else if(key_state == FOUND_AUTHOR){
        strncpy(author_string, (char*)value.c_str(), MAX_AUTHOR_LENGTH);
        DEBUG.printf("Author: %s\r\n", author_string);
        key_state = NOT_FOUND;
    }
}

void quoteListener::endArray()
{
}

void quoteListener::endObject()
{
}

void quoteListener::endDocument()
{
}

void quoteListener::startArray()
{
}

void quoteListener::startObject()
{
}

