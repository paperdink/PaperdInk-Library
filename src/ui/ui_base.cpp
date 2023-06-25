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

uint16_t PaperdinkUIClass::read16(File& f)
{
	// BMP data is stored little-endian, same as Arduino.
	uint16_t result;

	((uint8_t *)&result)[0] = f.read();     // LSB
	((uint8_t *)&result)[1] = f.read();     // MSB
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

PaperdinkUIClass Paperdink_UI;
