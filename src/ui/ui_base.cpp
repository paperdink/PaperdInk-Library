#include "ui_base.h"

void PaperdinkUIClass::display_grid(GxEPD2_GFX& display)
{
	/* Vertical line at half width */
	display.drawLine(display.width() * 0.5, 0, display.width() * 0.5, display.height(), GxEPD_BLACK);
	/* Vertical line at 1/4 width */
	display.drawLine(display.width() * 0.25, 0, display.width() * 0.25, display.height(), GxEPD_BLACK);
	/* Vertical line at 3/4 width */
	display.drawLine(display.width() * 0.75, 0, display.width() * 0.75, display.height(), GxEPD_BLACK);

	/* Horizontal line at half height */
	display.drawLine(0, display.height() * 0.5, display.width(), display.height() * 0.5, GxEPD_BLACK);
	/* Horizontal line at 1/4 width */
	display.drawLine(0, display.height() * 0.25, display.width(), display.height() * 0.25, GxEPD_BLACK);
	/* Horizontal line at 3/4 width */
	display.drawLine(0, display.height() * 0.75, display.width(), display.height() * 0.75, GxEPD_BLACK);
}


void PaperdinkUIClass::display_text_center(GxEPD2_GFX& display, int16_t x, int16_t y, const char *string, int16_t w, int16_t h, const GFXfont *font, uint8_t color, uint8_t size)
{
	int16_t xt, yt;
	uint16_t wt, ht, prev_height = y, prev_width = x;

	display.setFont(font);
	display.setTextColor(color);
	display.setTextSize(size);

	display.getTextBounds(string, 0, 0, &xt, &yt, &wt, &ht);
	if (w == 0 || h == 0)
		/* No centering */
		display.setCursor(prev_width, prev_height + ht);
	else
		display.setCursor(prev_width + ((w - wt) / 2), prev_height + ((h - ht) / 2));

	display.print(string);
}

#define seekSet seek

static const uint16_t input_buffer_pixels = 20;                 // may affect performance
static const uint16_t max_palette_pixels = 256;                 // for depth <= 8
static uint8_t input_buffer[3 * input_buffer_pixels];           // up to depth 24
static uint8_t mono_palette_buffer[max_palette_pixels / 8];     // palette buffer for depth <= 8 b/w
static uint8_t color_palette_buffer[max_palette_pixels / 8];    // palette buffer for depth <= 8 c/w
uint16_t rgb_palette_buffer[max_palette_pixels];                // palette buffer for depth <= 8 for buffered graphics, needed for 7-color display

uint16_t PaperdinkUIClass::read16(File& f)
{
	// BMP data is stored little-endian, same as Arduino.
	uint16_t result;

	((uint8_t *)&result)[0] = f.read();     // LSB
	((uint8_t *)&result)[1] = f.read();     // MSB
	return result;
}

uint16_t PaperdinkUIClass::read16(WiFiClient& client)
{
	// BMP data is stored little-endian, same as Arduino.
	uint16_t result;
	((uint8_t *)&result)[0] = client.read(); // LSB
	((uint8_t *)&result)[1] = client.read(); // MSB
	return result;
}

uint32_t PaperdinkUIClass::read32(File& f)
{
	// BMP data is stored little-endian, same as Arduino.
	uint32_t result;

	((uint8_t *)&result)[0] = f.read(); // LSB
	((uint8_t *)&result)[1] = f.read();
	((uint8_t *)&result)[2] = f.read();
	((uint8_t *)&result)[3] = f.read(); // MSB
	return result;
}

uint32_t PaperdinkUIClass::read32(WiFiClient& client)
{
	// BMP data is stored little-endian, same as Arduino.
	uint32_t result;
	((uint8_t *)&result)[0] = client.read(); // LSB
	((uint8_t *)&result)[1] = client.read();
	((uint8_t *)&result)[2] = client.read();
	((uint8_t *)&result)[3] = client.read(); // MSB
	return result;
}

uint32_t PaperdinkUIClass::skip(WiFiClient& client, int32_t bytes)
{
	int32_t remain = bytes;
	uint32_t start = millis();
	while ((client.connected() || client.available()) && (remain > 0))
	{
		if (client.available())
		{
			int16_t v = client.read();
			remain--;
		}
		else delay(1);
		if (millis() - start > 2000) break; // don't hang forever
	}
	return bytes - remain;
}

uint32_t PaperdinkUIClass::read8n(WiFiClient& client, uint8_t* buffer, int32_t bytes)
{
	int32_t remain = bytes;
	uint32_t start = millis();
	while ((client.connected() || client.available()) && (remain > 0))
	{
		if (client.available())
		{
			int16_t v = client.read();
			*buffer++ = uint8_t(v);
			remain--;
		}
		else delay(1);
		if (millis() - start > 2000) break; // don't hang forever
	}
	return bytes - remain;
}

void PaperdinkUIClass::display_bitmap_fs(GxEPD2_GFX& display, fs::FS &fs, const char *filename, int16_t x, int16_t y, bool with_color)
{
	File file;
	bool valid = false;     // valid format to be handled
	bool flip = true;       // bitmap is stored bottom-to-top
	uint32_t startTime = millis();

	if ((x >= display.width()) || (y >= display.height())) return;

	DEBUG.println();
	DEBUG.print("Loading image '");
	DEBUG.print(filename);
	DEBUG.println('\'');

	file = fs.open(String("/") + filename, FILE_READ);
	if (!file) {
		DEBUG.print("File not found");
		return;
	}

	// Parse BMP header
	if (read16(file) == 0x4D42) { // BMP signature
		uint32_t fileSize = read32(file);
		uint32_t creatorBytes = read32(file);
		uint32_t imageOffset = read32(file); // Start of image data
		uint32_t headerSize = read32(file);
		uint32_t width = read32(file);
		uint32_t height = read32(file);
		uint16_t planes = read16(file);
		uint16_t depth = read16(file);                                  // bits per pixel
		uint32_t format = read32(file);
		if ((planes == 1) && ((format == 0) || (format == 3))) {        // uncompressed is handled, 565 also
			DEBUG.print("File size: "); DEBUG.println(fileSize);
			DEBUG.print("Image Offset: "); DEBUG.println(imageOffset);
			DEBUG.print("Header size: "); DEBUG.println(headerSize);
			DEBUG.print("Bit Depth: "); DEBUG.println(depth);
			DEBUG.print("Image size: ");
			DEBUG.print(width);
			DEBUG.print('x');
			DEBUG.println(height);
			// BMP rows are padded (if needed) to 4-byte boundary
			uint32_t rowSize = (width * depth / 8 + 3) & ~3;
			if (depth < 8) rowSize = ((width * depth + 8 - depth) / 8 + 3) & ~3;
			if (height < 0) {
				height = -height;
				flip = false;
			}
			uint16_t w = width;
			uint16_t h = height;
			if ((x + w - 1) >= display.width()) w = display.width() - x;
			if ((y + h - 1) >= display.height()) h = display.height() - y;
			valid = true;
			uint8_t bitmask = 0xFF;
			uint8_t bitshift = 8 - depth;
			uint16_t red, green, blue;
			bool whitish, colored;
			if (depth == 1) with_color = false;
			if (depth <= 8) {
				if (depth < 8) bitmask >>= depth;
				file.seekSet(54); //palette is always @ 54
				for (uint16_t pn = 0; pn < (1 << depth); pn++) {
					blue = file.read();
					green = file.read();
					red = file.read();
					file.read();
					whitish = with_color ? ((red > 0x80) && (green > 0x80) && (blue > 0x80)) : ((red + green + blue) > 3 * 0x80);   // whitish
					colored = (red > 0xF0) || ((green > 0xF0) && (blue > 0xF0));                                                    // reddish or yellowish?
					if (0 == pn % 8) mono_palette_buffer[pn / 8] = 0;
					mono_palette_buffer[pn / 8] |= whitish << pn % 8;
					if (0 == pn % 8) color_palette_buffer[pn / 8] = 0;
					color_palette_buffer[pn / 8] |= colored << pn % 8;
				}
			}
			//display.fillScreen(GxEPD_WHITE);
			uint32_t rowPosition = flip ? imageOffset + (height - h) * rowSize : imageOffset;
			for (uint16_t row = 0; row < h; row++, rowPosition += rowSize) { // for each line
				uint32_t in_remain = rowSize;
				uint32_t in_idx = 0;
				uint32_t in_bytes = 0;
				uint8_t in_byte = 0;    // for depth <= 8
				uint8_t in_bits = 0;    // for depth <= 8
				uint16_t color = GxEPD_BLACK;
				file.seekSet(rowPosition);
				for (uint16_t col = 0; col < w; col++) {        // for each pixel
																// Time to read more pixel data?
					if (in_idx >= in_bytes) {               // ok, exact match for 24bit also (size IS multiple of 3)
						in_bytes = file.read(input_buffer, in_remain > sizeof(input_buffer) ? sizeof(input_buffer) : in_remain);
						in_remain -= in_bytes;
						in_idx = 0;
					}
					switch (depth) {
						case 24:
							blue = input_buffer[in_idx++];
							green = input_buffer[in_idx++];
							red = input_buffer[in_idx++];
							whitish = with_color ? ((red > 0x80) && (green > 0x80) && (blue > 0x80)) : ((red + green + blue) > 3 * 0x80);   // whitish
							colored = (red > 0xF0) || ((green > 0xF0) && (blue > 0xF0));                                                    // reddish or yellowish?
							break;
						case 16:
							{
								uint8_t lsb = input_buffer[in_idx++];
								uint8_t msb = input_buffer[in_idx++];
								if (format == 0) { // 555
									blue = (lsb & 0x1F) << 3;
									green = ((msb & 0x03) << 6) | ((lsb & 0xE0) >> 2);
									red = (msb & 0x7C) << 1;
								} else {// 565
									blue = (lsb & 0x1F) << 3;
									green = ((msb & 0x07) << 5) | ((lsb & 0xE0) >> 3);
									red = (msb & 0xF8);
								}
								whitish = with_color ? ((red > 0x80) && (green > 0x80) && (blue > 0x80)) : ((red + green + blue) > 3 * 0x80);   // whitish
								colored = (red > 0xF0) || ((green > 0xF0) && (blue > 0xF0));                                                    // reddish or yellowish?
							}
							break;
						case 1:
						case 4:
						case 8:
							{
								if (0 == in_bits) {
									in_byte = input_buffer[in_idx++];
									in_bits = 8;
								}
								uint16_t pn = (in_byte >> bitshift) & bitmask;
								whitish = mono_palette_buffer[pn / 8] & (0x1 << pn % 8);
								colored = color_palette_buffer[pn / 8] & (0x1 << pn % 8);
								in_byte <<= depth;
								in_bits -= depth;
							}
							break;
					}
					if (whitish)
						color = GxEPD_WHITE;
					else if (colored && with_color)
						color = GxEPD_RED;
					else
						color = GxEPD_BLACK;
					uint16_t yrow = y + (flip ? h - row - 1 : row);
					display.drawPixel(x + col, yrow, color);
				}       // end pixel
			}               // end line
			DEBUG.print("loaded in "); DEBUG.print(millis() - startTime); DEBUG.println(" ms");
		}
	}
	file.close();
	if (!valid)
		DEBUG.println("bitmap format not handled.");
}

void PaperdinkUIClass::display_bitmap_https(GxEPD2_GFX& display, const char* host, const int httpsPort, const char* path, const char* filename, int16_t x, int16_t y, bool with_color, const char* certificate)
{
	// Use WiFiClientSecure class to create TLS connection
#if USE_BearSSL
	BearSSL::WiFiClientSecure client;
#else
	//WiFiClientSecure client;
	WiFiClient client;
#endif

	bool connection_ok = false;
	bool valid = false; // valid format to be handled
	bool flip = true; // bitmap is stored bottom-to-top
	bool has_multicolors = display.epd2.panel == GxEPD2::ACeP565;
	uint32_t startTime = millis();
	if ((x >= display.width()) || (y >= display.height())) return;
	Serial.printf("connecting to %s:%d\r\n",host, httpsPort);

	//if (certificate) client.setCACert(certificate);

	if (!client.connect(host, httpsPort))
	{
		Serial.println("connection failed");
		return;
	}
	Serial.print("requesting URL: ");
	Serial.println(String("https://") + host + path + filename);
	client.print(String("GET ") + path + filename + " HTTP/1.0\r\n" +
			"Host: " + host + "\r\n" +
			"User-Agent: Paperd.Ink\r\n" +
			"Connection: close\r\n\r\n");
	Serial.println("request sent");
	while (client.connected())
	{
		String line = client.readStringUntil('\n');
		if (!connection_ok)
		{
			connection_ok = line.startsWith("HTTP/1.0 200 OK");
			if (connection_ok) Serial.println(line);
			//if (!connection_ok) Serial.println(line);
		}
		if (!connection_ok) Serial.println(line);
		//Serial.println(line);
		if (line == "\r")
		{
			Serial.println("headers received");
			break;
		}
	}
	if (!connection_ok){
		Serial.println("connection not ok");
		return;
	}
	// Parse BMP header
	uint16_t signature = 0;
	for (int16_t i = 0; i < 50; i++)
	{
		if (!client.available()) delay(100);
		else signature = read16(client);
		if (signature == 0x4D42)
		{
		//Serial.print("signature wait loops: "); Serial.println(i);
		break;
		}
	}

	if (signature == 0x4D42) // BMP signature
	{
		uint32_t fileSize = read32(client);
		uint32_t creatorBytes = read32(client);
		uint32_t imageOffset = read32(client); // Start of image data
		uint32_t headerSize = read32(client);
		uint32_t width  = read32(client);
		uint32_t height = read32(client);
		uint16_t planes = read16(client);
		uint16_t depth = read16(client); // bits per pixel
		uint32_t format = read32(client);
		uint32_t bytes_read = 7 * 4 + 3 * 2; // read so far
		if ((planes == 1) && ((format == 0) || (format == 3))) // uncompressed is handled, 565 also
		{
			Serial.print("File size: "); Serial.println(fileSize);
			Serial.print("Image Offset: "); Serial.println(imageOffset);
			Serial.print("Header size: "); Serial.println(headerSize);
			Serial.print("Bit Depth: "); Serial.println(depth);
			Serial.print("Image size: ");
			Serial.print(width);
			Serial.print('x');
			Serial.println(height);
			// BMP rows are padded (if needed) to 4-byte boundary
			uint32_t rowSize = (width * depth / 8 + 3) & ~3;
			if (depth < 8) rowSize = ((width * depth + 8 - depth) / 8 + 3) & ~3;
			if (height < 0)
			{
				height = -height;
				flip = false;
			}
			uint16_t w = width;
			uint16_t h = height;
			if ((x + w - 1) >= display.width())  w = display.width()  - x;
			if ((y + h - 1) >= display.height()) h = display.height() - y;
			//if (w <= max_row_width) // handle with direct drawing
			{
				valid = true;
				uint8_t bitmask = 0xFF;
				uint8_t bitshift = 8 - depth;
				uint16_t red, green, blue;
				bool whitish, colored;
				if (depth == 1) with_color = false;
				if (depth <= 8)
				{
					if (depth < 8) bitmask >>= depth;
					//bytes_read += skip(client, 54 - bytes_read); //palette is always @ 54
					bytes_read += skip(client, imageOffset - (4 << depth) - bytes_read); // 54 for regular, diff for colorsimportant
					for (uint16_t pn = 0; pn < (1 << depth); pn++)
					{
						blue  = client.read();
						green = client.read();
						red   = client.read();
						client.read();
						bytes_read += 4;
						whitish = with_color ? ((red > 0x80) && (green > 0x80) && (blue > 0x80)) : ((red + green + blue) > 3 * 0x80); // whitish
						colored = (red > 0xF0) || ((green > 0xF0) && (blue > 0xF0)); // reddish or yellowish?
						if (0 == pn % 8) mono_palette_buffer[pn / 8] = 0;
						mono_palette_buffer[pn / 8] |= whitish << pn % 8;
						if (0 == pn % 8) color_palette_buffer[pn / 8] = 0;
						color_palette_buffer[pn / 8] |= colored << pn % 8;
						//Serial.print("0x00"); Serial.print(red, HEX); Serial.print(green, HEX); Serial.print(blue, HEX);
						//Serial.print(" : "); Serial.print(whitish); Serial.print(", "); Serial.println(colored);
						rgb_palette_buffer[pn] = ((red & 0xF8) << 8) | ((green & 0xFC) << 3) | ((blue & 0xF8) >> 3);
					}
				}
				uint32_t rowPosition = flip ? imageOffset + (height - h) * rowSize : imageOffset;
				//Serial.print("skip "); Serial.println(rowPosition - bytes_read);
				bytes_read += skip(client, rowPosition - bytes_read);
				for (uint16_t row = 0; row < h; row++, rowPosition += rowSize) // for each line
				{
					if (!connection_ok || !(client.connected() || client.available())) break;
					delay(1); // yield() to avoid WDT
					uint32_t in_remain = rowSize;
					uint32_t in_idx = 0;
					uint32_t in_bytes = 0;
					uint8_t in_byte = 0; // for depth <= 8
					uint8_t in_bits = 0; // for depth <= 8
					uint16_t color = GxEPD_WHITE;
					for (uint16_t col = 0; col < w; col++) // for each pixel
					{
						yield();
						if (!connection_ok || !(client.connected() || client.available())) break;
						// Time to read more pixel data?
						if (in_idx >= in_bytes) // ok, exact match for 24bit also (size IS multiple of 3)
						{
							uint32_t get = in_remain > sizeof(input_buffer) ? sizeof(input_buffer) : in_remain;
							uint32_t got = read8n(client, input_buffer, get);
							while ((got < get) && connection_ok)
							{
								//Serial.print("got "); Serial.print(got); Serial.print(" < "); Serial.print(get); Serial.print(" @ "); Serial.println(bytes_read);
								uint32_t gotmore = read8n(client, input_buffer + got, get - got);
								got += gotmore;
								connection_ok = gotmore > 0;
							}
							in_bytes = got;
							in_remain -= got;
							bytes_read += got;
						}
						if (!connection_ok)
						{
							Serial.print("Error: got no more after "); Serial.print(bytes_read); Serial.println(" bytes read!");
							break;
						}
						switch (depth)
						{
							case 24:
								blue = input_buffer[in_idx++];
								green = input_buffer[in_idx++];
								red = input_buffer[in_idx++];
								whitish = with_color ? ((red > 0x80) && (green > 0x80) && (blue > 0x80)) : ((red + green + blue) > 3 * 0x80); // whitish
								colored = (red > 0xF0) || ((green > 0xF0) && (blue > 0xF0)); // reddish or yellowish?
								color = ((red & 0xF8) << 8) | ((green & 0xFC) << 3) | ((blue & 0xF8) >> 3);
								break;
							case 16:
								{
									uint8_t lsb = input_buffer[in_idx++];
									uint8_t msb = input_buffer[in_idx++];
									if (format == 0) // 555
									{
										blue  = (lsb & 0x1F) << 3;
										green = ((msb & 0x03) << 6) | ((lsb & 0xE0) >> 2);
										red   = (msb & 0x7C) << 1;
										color = ((red & 0xF8) << 8) | ((green & 0xFC) << 3) | ((blue & 0xF8) >> 3);
									}
									else // 565
									{
										blue  = (lsb & 0x1F) << 3;
										green = ((msb & 0x07) << 5) | ((lsb & 0xE0) >> 3);
										red   = (msb & 0xF8);
										color = (msb << 8) | lsb;
									}
									whitish = with_color ? ((red > 0x80) && (green > 0x80) && (blue > 0x80)) : ((red + green + blue) > 3 * 0x80); // whitish
									colored = (red > 0xF0) || ((green > 0xF0) && (blue > 0xF0)); // reddish or yellowish?
								}
								break;
							case 1:
							case 4:
							case 8:
								{
									if (0 == in_bits)
									{
										in_byte = input_buffer[in_idx++];
										in_bits = 8;
									}
									uint16_t pn = (in_byte >> bitshift) & bitmask;
									whitish = mono_palette_buffer[pn / 8] & (0x1 << pn % 8);
									colored = color_palette_buffer[pn / 8] & (0x1 << pn % 8);
									in_byte <<= depth;
									in_bits -= depth;
									color = rgb_palette_buffer[pn];
								}
								break;
						}
						if (with_color && has_multicolors)
						{
							// keep color
						}
						else if (whitish)
						{
							color = GxEPD_WHITE;
						}
						else if (colored && with_color)
						{
							color = GxEPD_COLORED;
						}
						else
						{
							color = GxEPD_BLACK;
						}
						uint16_t yrow = y + (flip ? h - row - 1 : row);
						display.drawPixel(x + col, yrow, color);
					} // end pixel
				} // end line
			}
			Serial.print("bytes read "); Serial.println(bytes_read);
		}
	}
	Serial.print("loaded in "); Serial.print(millis() - startTime); Serial.println(" ms");
	client.stop();
	if (!valid)
	{
		Serial.println("bitmap format not handled.");
	}
}

PaperdinkUIClass Paperdink_UI;
