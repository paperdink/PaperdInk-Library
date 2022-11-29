
#ifndef _WEATHER_PARSER_H_
#define _WEATHER_PARSER_H_

#include <JsonListener.h>

class WeatherJsonListener : public JsonListener {

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

#endif /* _WEATHER_PARSER_H_ */

