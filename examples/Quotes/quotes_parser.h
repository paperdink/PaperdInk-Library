#ifndef CUSTOM_PARSER_H
#define CUSTOM_PARSER_H

#include <Paperdink.h>

class quoteListener : public JsonListener {
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

#endif /* CUSTOM_PARSER_H */
