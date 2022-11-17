
#ifndef _PAPERDINK_COMMON_H_
#define _PAPERDINK_COMMON_H_

#include <JsonStreamingParser.h>
#include <JsonListener.h>
#include <Print.h>
#include <Stream.h>
#include <GxEPD2_GFX.h>

#include "fonts.h"
#include "icons.h"

/* Convert the Json data into an Arduino Stream */
class ArudinoStreamParser : public Stream, public JsonStreamingParser  {
/* https://github.com/mrfaptastic/json-streaming-parser2 */
public:
size_t write(const uint8_t *buffer, size_t size) override;
size_t write(uint8_t data) override;

int available() override;
int read() override;
int peek() override;
void flush() override;
};

#endif /* _PAPERDINK_COMMON_H_ */
