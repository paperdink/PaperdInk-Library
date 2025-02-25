// Display Library for SPI e-paper panels from Dalian Good Display and boards from Waveshare.
// Requires HW SPI and Adafruit_GFX. Caution: these e-papers require 3.3V supply AND data lines!
//
// based on Demo Example from Good Display: http://www.e-paper-display.com/download_list/downloadcategoryid=34&isMode=false.html
// Controller: IL0398 : http://www.e-paper-display.com/download_detail/downloadsId=537.html
//
// Author: Jean-Marc Zingg
//
// Version: see library.properties
//
// Library: https://github.com/ZinggJM/GxEPD2

#include "GxEPD2_420.h"

GxEPD2_420::GxEPD2_420(int16_t cs, int16_t dc, int16_t rst, int16_t busy) :
  GxEPD2_EPD(cs, dc, rst, busy, LOW, 10000000, WIDTH, HEIGHT, panel, hasColor, hasPartialUpdate, hasFastPartialUpdate)
{
  _refresh_mode = full_refresh;
}

void GxEPD2_420::clearScreen(uint8_t value)
{
  writeScreenBuffer(value);
  refresh(true);
  if (_refresh_mode != grey_refresh) writeScreenBuffer(value);
}

void GxEPD2_420::writeScreenBuffer(uint8_t value)
{
  _initial_write = false; // initial full screen buffer clean done
  if (_refresh_mode == full_refresh) _Init_Part();
  if (_initial_refresh || (_refresh_mode == grey_refresh))
  {
    _writeCommand(0x10); // init old data
    _startTransfer();
    for (uint32_t i = 0; i < uint32_t(WIDTH) * uint32_t(HEIGHT) / 8; i++)
    {
      _transfer(value);
    }
    _endTransfer();
  }
  _writeCommand(0x13);
  _startTransfer();
  for (uint32_t i = 0; i < uint32_t(WIDTH) * uint32_t(HEIGHT) / 8; i++)
  {
    _transfer(value);
  }
  _endTransfer();
}

void GxEPD2_420::writeImage(const uint8_t bitmap[], int16_t x, int16_t y, int16_t w, int16_t h, bool invert, bool mirror_y, bool pgm)
{
	writeImage_4G(bitmap, GxEPD_BPP, x, y, w, h,  invert, mirror_y, pgm);

//  if (_initial_write) writeScreenBuffer(); // initial full screen buffer clean
//  delay(1); // yield() to avoid WDT on ESP8266 and ESP32
//  int16_t wb = (w + 7) / 8; // width bytes, bitmaps are padded
//  x -= x % 8; // byte boundary
//  w = wb * 8; // byte boundary
//  int16_t x1 = x < 0 ? 0 : x; // limit
//  int16_t y1 = y < 0 ? 0 : y; // limit
//  int16_t w1 = x + w < int16_t(WIDTH) ? w : int16_t(WIDTH) - x; // limit
//  int16_t h1 = y + h < int16_t(HEIGHT) ? h : int16_t(HEIGHT) - y; // limit
//  int16_t dx = x1 - x;
//  int16_t dy = y1 - y;
//  w1 -= dx;
//  h1 -= dy;
//  if ((w1 <= 0) || (h1 <= 0)) return;
//  if (_refresh_mode == grey_refresh) _Force_Init_Full();
//  else if (_refresh_mode == full_refresh) _Init_Part();
//  //uint32_t start = micros();
//  _writeCommand(0x91); // partial in
//  _setPartialRamArea(x1, y1, w1, h1);
//  _writeCommand(0x13);
//  _startTransfer();
//  for (int16_t i = 0; i < h1; i++)
//  {
//    for (int16_t j = 0; j < w1 / 8; j++)
//    {
//      uint8_t data;
//      // use wb, h of bitmap for index!
//      int16_t idx = mirror_y ? j + dx / 8 + ((h - 1 - (i + dy))) * wb : j + dx / 8 + (i + dy) * wb;
//      if (pgm)
//      {
//#if defined(__AVR) || defined(ESP8266) || defined(ESP32)
//        data = pgm_read_byte(&bitmap[idx]);
//#else
//        data = bitmap[idx];
//#endif
//      }
//      else
//      {
//        data = bitmap[idx];
//      }
//      if (invert) data = ~data;
//      _transfer(data);
//    }
//  }
//  _endTransfer();
//  _writeCommand(0x92); // partial out
//  //Serial.print("GxEPD2_420::writeImage took "); Serial.println(micros() - start);
//  delay(1); // yield() to avoid WDT on ESP8266 and ESP32
}

void GxEPD2_420::writeImage_4G(const uint8_t bitmap[], uint8_t bpp, int16_t x, int16_t y, int16_t w, int16_t h, bool invert, bool mirror_y, bool pgm)
{
  uint16_t ppb = (bpp == 2 ? 4 : (bpp == 4 ? 2 : (bpp == 8 ? 1 : 0)));
  uint8_t mask = (bpp == 2 ? 0xC0 : (bpp == 4 ? 0xF0 : 0xFF));
  uint8_t grey1 = (bpp == 2 ? 0x80 : 0xA0); // demo limit for 4bpp
  if (ppb == 0) return;
  if (_initial_write) writeScreenBuffer(); // initial full screen buffer clean
  delay(1); // yield() to avoid WDT on ESP8266 and ESP32
  int16_t wbc = (w + 7) / 8; // width bytes on controller
  x -= x % 8; // byte boundary on controller
  w = wbc * 8; // byte boundary on controller
  int16_t wb = (w + ppb - 1) / ppb; // width bytes of bitmap, bitmaps are padded
  int16_t x1 = x < 0 ? 0 : x; // limit
  int16_t y1 = y < 0 ? 0 : y; // limit
  uint16_t w1 = x + w < int16_t(WIDTH) ? w : int16_t(WIDTH) - x; // limit
  uint16_t h1 = y + h < int16_t(HEIGHT) ? h : int16_t(HEIGHT) - y; // limit
  int16_t dx = x1 - x;
  int16_t dy = y1 - y;
  w1 -= dx;
  h1 -= dy;
  if ((w1 <= 0) || (h1 <= 0)) return;
  _Init_4G();
  _writeCommand(0x91); // partial in
  _setPartialRamArea(x1, y1, w1, h1);
  _writeCommand(0x10);
  for (uint16_t i = 0; i < h1; i++) // lines
  {
    for (uint16_t j = 0; j < w1 / ppb; j += bpp) // out bytes
    {
      uint8_t out_byte = 0;
      for (uint16_t k = 0; k < bpp; k++) // in bytes (bpp per out byte)
      {
        uint8_t in_byte;
        // use wb, h of bitmap for index!
        uint32_t idx = mirror_y ? j + k + dx / ppb + uint32_t((h - 1 - (i + dy))) * wb : j + k + dx / ppb + uint32_t(i + dy) * wb;
        if (pgm)
        {
#if defined(__AVR) || defined(ESP8266) || defined(ESP32)
          in_byte = pgm_read_byte(&bitmap[idx]);
#else
          in_byte = bitmap[idx];
#endif
        }
        else
        {
          in_byte = bitmap[idx];
        }
        if (invert) in_byte = ~in_byte;
        for (uint16_t n = 0; n < ppb; n++) // bits, nibbles (ppb per in byte)
        {
          out_byte <<= 1;
          uint8_t nibble = in_byte & mask;
          if (nibble == mask) out_byte |= 0x01;//white
          else if (nibble == 0x00) out_byte |= 0x00;  //black
          else if (nibble >= grey1) out_byte |= 0x01;  //gray1
          else out_byte |= 0x00; //gray2
          in_byte <<= bpp;
        }
      }
      _writeData(out_byte);
    }
  }
  _writeCommand(0x13);
  for (uint16_t i = 0; i < h1; i++) // lines
  {
    for (uint16_t j = 0; j < w1 / ppb; j += bpp) // out bytes
    {
      uint8_t out_byte = 0;
      for (uint16_t k = 0; k < bpp; k++) // in bytes (bpp per out byte)
      {
        uint8_t in_byte;
        // use wb, h of bitmap for index!
        uint32_t idx = mirror_y ? j + k + dx / ppb + uint32_t((h - 1 - (i + dy))) * wb : j + k + dx / ppb + uint32_t(i + dy) * wb;
        if (pgm)
        {
#if defined(__AVR) || defined(ESP8266) || defined(ESP32)
          in_byte = pgm_read_byte(&bitmap[idx]);
#else
          in_byte = bitmap[idx];
#endif
        }
        else
        {
          in_byte = bitmap[idx];
        }
        if (invert) in_byte = ~in_byte;
        for (uint16_t n = 0; n < ppb; n++) // bits, nibbles (ppb per in byte)
        {
          out_byte <<= 1;
          uint8_t nibble = in_byte & mask;
          if (nibble == mask) out_byte |= 0x01;//white
          else if (nibble == 0x00) out_byte |= 0x00;  //black
          else if (nibble >= grey1) out_byte |= 0x00;  //gray1
          else out_byte |= 0x01; //gray2
          in_byte <<= bpp;
        }
      }
      _writeData(out_byte);
    }
  }
  _writeCommand(0x92); // partial out
  delay(1); // yield() to avoid WDT on ESP8266 and ESP32
}

void GxEPD2_420::writeImagePart(const uint8_t bitmap[], int16_t x_part, int16_t y_part, int16_t w_bitmap, int16_t h_bitmap,
                                int16_t x, int16_t y, int16_t w, int16_t h, bool invert, bool mirror_y, bool pgm)
{
    writeImagePart_4G(bitmap, GxEPD_BPP, x_part, y_part, w_bitmap, h_bitmap, x, y, w, h, invert, mirror_y, pgm);
//  if (_initial_write) writeScreenBuffer(); // initial full screen buffer clean
//  delay(1); // yield() to avoid WDT on ESP8266 and ESP32
//  if ((w_bitmap < 0) || (h_bitmap < 0) || (w < 0) || (h < 0)) return;
//  if ((x_part < 0) || (x_part >= w_bitmap)) return;
//  if ((y_part < 0) || (y_part >= h_bitmap)) return;
//  int16_t wb_bitmap = (w_bitmap + 7) / 8; // width bytes, bitmaps are padded
//  x_part -= x_part % 8; // byte boundary
//  w = w_bitmap - x_part < w ? w_bitmap - x_part : w; // limit
//  h = h_bitmap - y_part < h ? h_bitmap - y_part : h; // limit
//  x -= x % 8; // byte boundary
//  w = 8 * ((w + 7) / 8); // byte boundary, bitmaps are padded
//  int16_t x1 = x < 0 ? 0 : x; // limit
//  int16_t y1 = y < 0 ? 0 : y; // limit
//  int16_t w1 = x + w < int16_t(WIDTH) ? w : int16_t(WIDTH) - x; // limit
//  int16_t h1 = y + h < int16_t(HEIGHT) ? h : int16_t(HEIGHT) - y; // limit
//  int16_t dx = x1 - x;
//  int16_t dy = y1 - y;
//  w1 -= dx;
//  h1 -= dy;
//  if ((w1 <= 0) || (h1 <= 0)) return;
//  if (_refresh_mode == grey_refresh) _Force_Init_Full();
//  else if (_refresh_mode == full_refresh) _Init_Part();
//  _writeCommand(0x91); // partial in
//  _setPartialRamArea(x1, y1, w1, h1);
//  _writeCommand(0x13);
//  _startTransfer();
//  for (int16_t i = 0; i < h1; i++)
//  {
//    for (int16_t j = 0; j < w1 / 8; j++)
//    {
//      uint8_t data;
//      // use wb_bitmap, h_bitmap of bitmap for index!
//      int16_t idx = mirror_y ? x_part / 8 + j + dx / 8 + ((h_bitmap - 1 - (y_part + i + dy))) * wb_bitmap : x_part / 8 + j + dx / 8 + (y_part + i + dy) * wb_bitmap;
//      if (pgm)
//      {
//#if defined(__AVR) || defined(ESP8266) || defined(ESP32)
//        data = pgm_read_byte(&bitmap[idx]);
//#else
//        data = bitmap[idx];
//#endif
//      }
//      else
//      {
//        data = bitmap[idx];
//      }
//      if (invert) data = ~data;
//      _transfer(data);
//    }
//  }
//  _endTransfer();
//  _writeCommand(0x92); // partial out
//  delay(1); // yield() to avoid WDT on ESP8266 and ESP32
}

void GxEPD2_420::writeImagePart_4G(const uint8_t bitmap[], uint8_t bpp, int16_t x_part, int16_t y_part, int16_t w_bitmap, int16_t h_bitmap,
                                   int16_t x, int16_t y, int16_t w, int16_t h, bool invert, bool mirror_y, bool pgm)
{
  uint16_t ppb = (bpp == 2 ? 4 : (bpp == 4 ? 2 : (bpp == 8 ? 1 : 0)));
  uint8_t mask = (bpp == 2 ? 0xC0 : (bpp == 4 ? 0xF0 : 0xFF));
  uint8_t grey1 = (bpp == 2 ? 0x80 : 0xA0); // demo limit for 4bpp
  if (ppb == 0) return;
  if (_initial_write) writeScreenBuffer(); // initial full screen buffer clean
  delay(1); // yield() to avoid WDT on ESP8266 and ESP32
  if ((w_bitmap < 0) || (h_bitmap < 0) || (w < 0) || (h < 0)) return;
  if ((x_part < 0) || (x_part >= w_bitmap)) return;
  if ((y_part < 0) || (y_part >= h_bitmap)) return;
  int16_t wb_bitmap = (w_bitmap + ppb - 1) / ppb; // width bytes, bitmaps are padded
  x_part -= x_part % ppb; // byte boundary
  w = w_bitmap - x_part < w ? w_bitmap - x_part : w; // limit
  h = h_bitmap - y_part < h ? h_bitmap - y_part : h; // limit
  int16_t wbc = (w + 7) / 8; // width bytes on controller
  x -= x % 8; // byte boundary on controller
  w = wbc * 8; // byte boundary on controller
  int16_t x1 = x < 0 ? 0 : x; // limit
  int16_t y1 = y < 0 ? 0 : y; // limit
  uint16_t w1 = x + w < int16_t(WIDTH) ? w : int16_t(WIDTH) - x; // limit
  uint16_t h1 = y + h < int16_t(HEIGHT) ? h : int16_t(HEIGHT) - y; // limit
  int16_t dx = x1 - x;
  int16_t dy = y1 - y;
  w1 -= dx;
  h1 -= dy;
  if ((w1 <= 0) || (h1 <= 0)) return;
  _Init_4G();
  _writeCommand(0x91); // partial in
  _setPartialRamArea(x1, y1, w1, h1);
  _writeCommand(0x10);
  for (uint16_t i = 0; i < h1; i++) // lines
  {
    for (uint16_t j = 0; j < w1 / ppb; j += bpp) // out bytes
    {
      uint8_t out_byte = 0;
      for (uint16_t k = 0; k < bpp; k++) // in bytes (bpp per out byte)
      {
        uint8_t in_byte;
        // use wb_bitmap, h_bitmap of bitmap for index!
        uint32_t idx = mirror_y ? x_part / ppb + j + k + dx / ppb + uint32_t((h_bitmap - 1 - (y_part + i + dy))) * wb_bitmap : x_part / ppb + j + k + dx / ppb + uint32_t(y_part + i + dy) * wb_bitmap;
        if (pgm)
        {
#if defined(__AVR) || defined(ESP8266) || defined(ESP32)
          in_byte = pgm_read_byte(&bitmap[idx]);
#else
          in_byte = bitmap[idx];
#endif
        }
        else
        {
          in_byte = bitmap[idx];
        }
        if (invert) in_byte = ~in_byte;
        for (uint16_t n = 0; n < ppb; n++) // bits, nibbles (ppb per in byte)
        {
          out_byte <<= 1;
          uint8_t nibble = in_byte & mask;
          if (nibble == mask) out_byte |= 0x01;//white
          else if (nibble == 0x00) out_byte |= 0x00;  //black
          else if (nibble >= grey1) out_byte |= 0x01;  //gray1
          else out_byte |= 0x00; //gray2
          in_byte <<= bpp;
        }
      }
      _writeData(out_byte);
    }
  }
  _writeCommand(0x13);
  for (uint16_t i = 0; i < h1; i++) // lines
  {
    for (uint16_t j = 0; j < w1 / ppb; j += bpp) // out bytes
    {
      uint8_t out_byte = 0;
      for (uint16_t k = 0; k < bpp; k++) // in bytes (bpp per out byte)
      {
        uint8_t in_byte;
        // use wb_bitmap, h_bitmap of bitmap for index!
        uint32_t idx = mirror_y ? x_part / ppb + j + k + dx / ppb + uint32_t((h_bitmap - 1 - (y_part + i + dy))) * wb_bitmap : x_part / ppb + j + k + dx / ppb + uint32_t(y_part + i + dy) * wb_bitmap;
        if (pgm)
        {
#if defined(__AVR) || defined(ESP8266) || defined(ESP32)
          in_byte = pgm_read_byte(&bitmap[idx]);
#else
          in_byte = bitmap[idx];
#endif
        }
        else
        {
          in_byte = bitmap[idx];
        }
        if (invert) in_byte = ~in_byte;
        for (uint16_t n = 0; n < ppb; n++) // bits, nibbles (ppb per in byte)
        {
          out_byte <<= 1;
          uint8_t nibble = in_byte & mask;
          if (nibble == mask) out_byte |= 0x01;//white
          else if (nibble == 0x00) out_byte |= 0x00;  //black
          else if (nibble >= grey1) out_byte |= 0x00;  //gray1
          else out_byte |= 0x01; //gray2
          in_byte <<= bpp;
        }
      }
      _writeData(out_byte);
    }
  }
  _writeCommand(0x92); // partial out
  delay(1); // yield() to avoid WDT on ESP8266 and ESP32
}

void GxEPD2_420::writeImage(const uint8_t* black, const uint8_t* color, int16_t x, int16_t y, int16_t w, int16_t h, bool invert, bool mirror_y, bool pgm)
{
  if (black)
  {
    writeImage(black, x, y, w, h, invert, mirror_y, pgm);
  }
}

void GxEPD2_420::writeImagePart(const uint8_t* black, const uint8_t* color, int16_t x_part, int16_t y_part, int16_t w_bitmap, int16_t h_bitmap,
                                int16_t x, int16_t y, int16_t w, int16_t h, bool invert, bool mirror_y, bool pgm)
{
  if (black)
  {
    writeImagePart(black, x_part, y_part, w_bitmap, h_bitmap, x, y, w, h, invert, mirror_y, pgm);
  }
}

void GxEPD2_420::writeNative(const uint8_t* data1, const uint8_t* data2, int16_t x, int16_t y, int16_t w, int16_t h, bool invert, bool mirror_y, bool pgm)
{
  if (data1)
  {
    writeImage(data1, x, y, w, h, invert, mirror_y, pgm);
  }
}

void GxEPD2_420::drawImage(const uint8_t bitmap[], int16_t x, int16_t y, int16_t w, int16_t h, bool invert, bool mirror_y, bool pgm)
{
  writeImage(bitmap, x, y, w, h, invert, mirror_y, pgm);
  refresh(x, y, w, h);
  writeImage(bitmap, x, y, w, h, invert, mirror_y, pgm);
}

void GxEPD2_420::drawImagePart(const uint8_t bitmap[], int16_t x_part, int16_t y_part, int16_t w_bitmap, int16_t h_bitmap,
                               int16_t x, int16_t y, int16_t w, int16_t h, bool invert, bool mirror_y, bool pgm)
{
  writeImagePart(bitmap, x_part, y_part, w_bitmap, h_bitmap, x, y, w, h, invert, mirror_y, pgm);
  refresh(x, y, w, h);
  writeImagePart(bitmap, x_part, y_part, w_bitmap, h_bitmap, x, y, w, h, invert, mirror_y, pgm);
}

void GxEPD2_420::drawImage(const uint8_t* black, const uint8_t* color, int16_t x, int16_t y, int16_t w, int16_t h, bool invert, bool mirror_y, bool pgm)
{
  writeImage(black, color, x, y, w, h, invert, mirror_y, pgm);
  refresh(x, y, w, h);
  writeImage(black, color, x, y, w, h, invert, mirror_y, pgm);
}

void GxEPD2_420::drawImagePart(const uint8_t* black, const uint8_t* color, int16_t x_part, int16_t y_part, int16_t w_bitmap, int16_t h_bitmap,
                               int16_t x, int16_t y, int16_t w, int16_t h, bool invert, bool mirror_y, bool pgm)
{
  writeImagePart(black, color, x_part, y_part, w_bitmap, h_bitmap, x, y, w, h, invert, mirror_y, pgm);
  refresh(x, y, w, h);
  writeImagePart(black, color, x_part, y_part, w_bitmap, h_bitmap, x, y, w, h, invert, mirror_y, pgm);
}

void GxEPD2_420::drawNative(const uint8_t* data1, const uint8_t* data2, int16_t x, int16_t y, int16_t w, int16_t h, bool invert, bool mirror_y, bool pgm)
{
  writeNative(data1, data2, x, y, w, h, invert, mirror_y, pgm);
  refresh(x, y, w, h);
  writeNative(data1, data2, x, y, w, h, invert, mirror_y, pgm);
}

void GxEPD2_420::refresh(bool partial_update_mode)
{
  if (partial_update_mode) refresh(0, 0, WIDTH, HEIGHT);
  else
  {
    if (_refresh_mode == forced_full_refresh) _refresh_mode = full_refresh;
    if (_refresh_mode == fast_refresh) _Init_Full();
    if (_refresh_mode == grey_refresh) _Update_4G();
    else _Update_Full();
    _initial_refresh = false; // initial full update done
  }
}

void GxEPD2_420::refresh(int16_t x, int16_t y, int16_t w, int16_t h)
{
  if (_initial_refresh) return refresh(false); // initial update needs be full update
  if (_refresh_mode == forced_full_refresh) return refresh(false);
  // intersection with screen
  int16_t w1 = x < 0 ? w + x : w; // reduce
  int16_t h1 = y < 0 ? h + y : h; // reduce
  int16_t x1 = x < 0 ? 0 : x; // limit
  int16_t y1 = y < 0 ? 0 : y; // limit
  w1 = x1 + w1 < int16_t(WIDTH) ? w1 : int16_t(WIDTH) - x1; // limit
  h1 = y1 + h1 < int16_t(HEIGHT) ? h1 : int16_t(HEIGHT) - y1; // limit
  if ((w1 <= 0) || (h1 <= 0)) return; 
  // make x1, w1 multiple of 8
  w1 += x1 % 8;
  if (w1 % 8 > 0) w1 += 8 - w1 % 8;
  x1 -= x1 % 8;
  if (_refresh_mode == full_refresh) _Init_Part();
  if (usePartialUpdateWindow) _writeCommand(0x91); // partial in
  _setPartialRamArea(x1, y1, w1, h1);
  if (_refresh_mode == grey_refresh) _Update_4G();
  else _Update_Part();
  if (usePartialUpdateWindow) _writeCommand(0x92); // partial out
}

void GxEPD2_420::powerOff(void)
{
  _PowerOff();
}

void GxEPD2_420::hibernate()
{
  _PowerOff();
  if (_rst >= 0)
  {
    _writeCommand(0x07); // deep sleep
    _writeData(0xA5);    // check code
    _hibernating = true;
  }
}

void GxEPD2_420::_setPartialRamArea(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
  uint16_t xe = (x + w - 1) | 0x0007; // byte boundary inclusive (last byte)
  uint16_t ye = y + h - 1;
  x &= 0xFFF8; // byte boundary
  _writeCommand(0x90); // partial window
  _writeData(x / 256);
  _writeData(x % 256);
  _writeData(xe / 256);
  _writeData(xe % 256);
  _writeData(y / 256);
  _writeData(y % 256);
  _writeData(ye / 256);
  _writeData(ye % 256);
  _writeData(0x01); // don't see any difference
  //_writeData(0x00); // don't see any difference
}

void GxEPD2_420::_PowerOn()
{
  if (!_power_is_on)
  {
    _writeCommand(0x04);
    _waitWhileBusy("_PowerOn", power_on_time);
  }
  _power_is_on = true;
}

void GxEPD2_420::_PowerOff()
{
  _writeCommand(0x02); // power off
  _waitWhileBusy("_PowerOff", power_off_time);
  _power_is_on = false;
  _refresh_mode = full_refresh;
}

void GxEPD2_420::_InitDisplay()
{
  if (_hibernating) _reset();
  _writeCommand(0x01); // POWER SETTING
  _writeData (0x03);   // VDS_EN, VDG_EN internal
  _writeData (0x00);   // VCOM_HV, VGHL_LV=16V
  _writeData (0x2b);   // VDH=11V
  _writeData (0x2b);   // VDL=11V
  _writeCommand(0x06); // boost soft start
  _writeData (0x17);   // A
  _writeData (0x17);   // B
  _writeData (0x17);   // C
  _writeCommand(0x00); // panel setting
  _writeData(0x3f);    // 300x400 B/W mode, LUT set by register
  _writeCommand(0x30); // PLL setting
  _writeData (0x3a);   // 3a 100HZ   29 150Hz 39 200HZ 31 171HZ
  _writeCommand(0x61); // resolution setting
  _writeData (WIDTH / 256);
  _writeData (WIDTH % 256);
  _writeData (HEIGHT / 256);
  _writeData (HEIGHT % 256);
  _writeCommand(0x82); // vcom_DC setting
  //_writeData (0x08);   // -0.1 + 8 * -0.05 = -0.5V from demo
  _writeData (0x12);   // -0.1 + 18 * -0.05 = -1.0V from OTP, slightly better
  //_writeData (0x1c);   // -0.1 + 28 * -0.05 = -1.5V test, worse
  _writeCommand(0x50); // VCOM AND DATA INTERVAL SETTING
  //_writeData(0x97);    // WBmode:VBDF 17|D7 VBDW 97 VBDB 57   WBRmode:VBDF F7 VBDW 77 VBDB 37  VBDR B7
  _writeData(0xd7);    // border floating to avoid flashing
}

const unsigned char GxEPD2_420::lut_20_vcom0_full[] PROGMEM =
{
  0x00, 0x08, 0x00, 0x00, 0x00, 0x02,
  0x60, 0x28, 0x28, 0x00, 0x00, 0x01,
  0x00, 0x14, 0x00, 0x00, 0x00, 0x01,
  0x00, 0x12, 0x12, 0x00, 0x00, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00
};

const unsigned char GxEPD2_420::lut_21_ww_full[] PROGMEM =
{
  0x40, 0x08, 0x00, 0x00, 0x00, 0x02,
  0x90, 0x28, 0x28, 0x00, 0x00, 0x01,
  0x40, 0x14, 0x00, 0x00, 0x00, 0x01,
  0xA0, 0x12, 0x12, 0x00, 0x00, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char GxEPD2_420::lut_22_bw_full[] PROGMEM =
{
  0x40, 0x08, 0x00, 0x00, 0x00, 0x02,
  0x90, 0x28, 0x28, 0x00, 0x00, 0x01,
  0x40, 0x14, 0x00, 0x00, 0x00, 0x01,
  0xA0, 0x12, 0x12, 0x00, 0x00, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char GxEPD2_420::lut_23_wb_full[] PROGMEM =
{
  0x80, 0x08, 0x00, 0x00, 0x00, 0x02,
  0x90, 0x28, 0x28, 0x00, 0x00, 0x01,
  0x80, 0x14, 0x00, 0x00, 0x00, 0x01,
  0x50, 0x12, 0x12, 0x00, 0x00, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char GxEPD2_420::lut_24_bb_full[] PROGMEM =
{
  0x80, 0x08, 0x00, 0x00, 0x00, 0x02,
  0x90, 0x28, 0x28, 0x00, 0x00, 0x01,
  0x80, 0x14, 0x00, 0x00, 0x00, 0x01,
  0x50, 0x12, 0x12, 0x00, 0x00, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// full screen update LUT 0~3 gray
const unsigned char GxEPD2_420::lut_20_vcom0_4G[] PROGMEM =
{
  0x00, 0x0A, 0x00, 0x00, 0x00, 0x01,
  0x60, 0x14, 0x14, 0x00, 0x00, 0x01,
  0x00, 0x14, 0x00, 0x00, 0x00, 0x01,
  0x00, 0x13, 0x0A, 0x01, 0x00, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00
};
//R21
const unsigned char GxEPD2_420::lut_21_ww_4G[] PROGMEM =
{
  0x40, 0x0A, 0x00, 0x00, 0x00, 0x01,
  0x90, 0x14, 0x14, 0x00, 0x00, 0x01,
  0x10, 0x14, 0x0A, 0x00, 0x00, 0x01,
  0xA0, 0x13, 0x01, 0x00, 0x00, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
//R22H  r
const unsigned char GxEPD2_420::lut_22_bw_4G[] PROGMEM =
{
  0x40, 0x0A, 0x00, 0x00, 0x00, 0x01,
  0x90, 0x14, 0x14, 0x00, 0x00, 0x01,
  0x00, 0x14, 0x0A, 0x00, 0x00, 0x01,
  0x99, 0x0C, 0x01, 0x03, 0x04, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
//R23H  w
const unsigned char GxEPD2_420::lut_23_wb_4G[] PROGMEM =
{
  0x40, 0x0A, 0x00, 0x00, 0x00, 0x01,
  0x90, 0x14, 0x14, 0x00, 0x00, 0x01,
  0x00, 0x14, 0x0A, 0x00, 0x00, 0x01,
  0x99, 0x0B, 0x04, 0x04, 0x01, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
//R24H  b
const unsigned char GxEPD2_420::lut_24_bb_4G[] PROGMEM =
{
  0x80, 0x0A, 0x00, 0x00, 0x00, 0x01,
  0x90, 0x14, 0x14, 0x00, 0x00, 0x01,
  0x20, 0x14, 0x0A, 0x00, 0x00, 0x01,
  0x50, 0x13, 0x01, 0x00, 0x00, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

// partial update waveform

// same waveform as by demo code from Good Display
//#define T1  0 // color change charge balance pre-phase
//#define T2  0 // color change or sustain charge balance pre-phase
//#define T3  0 // color change or sustain phase
//#define T4 25 // color change phase

// new waveform created by Jean-Marc Zingg for the actual panel
#define T1 25 // color change charge balance pre-phase
#define T2  1 // color change or sustain charge balance pre-phase
#define T3  4 // color change or sustain phase, was 2 before
#define T4 25 // color change phase

// for new waveform without sustain phase: uncomment next 2 lines, not good for fat fonts
//#define T2  0 // color change or sustain charge balance pre-phase // 0 no sustain
//#define T3  0 // color change or sustain phase // 0 no sustain

// "balanced flash once" variant
//#define T1  0 // color change charge balance pre-phase
//#define T2 25 // color change or sustain charge balance pre-phase
//#define T3 25 // color change or sustain phase
//#define T4  0 // color change phase

const unsigned char GxEPD2_420::lut_20_vcom0_partial[] PROGMEM =
{
  0x00, T1, T2, T3, T4, 1, // 00 00 00 00
  0x00,  1,  0,  0,  0, 1, // gnd phase
};

const unsigned char GxEPD2_420::lut_21_ww_partial[] PROGMEM =
{ // 10 w
  0x18, T1, T2, T3, T4, 1, // 00 01 10 00
  0x00,  1,  0,  0,  0, 1, // gnd phase
};

const unsigned char GxEPD2_420::lut_22_bw_partial[] PROGMEM =
{ // 10 w
  0x5A, T1, T2, T3, T4, 1, // 01 01 10 10
  0x00,  1,  0,  0,  0, 1, // gnd phase
};

const unsigned char GxEPD2_420::lut_23_wb_partial[] PROGMEM =
{ // 01 b
  0xA5, T1, T2, T3, T4, 1, // 10 10 01 01
  0x00,  1,  0,  0,  0, 1, // gnd phase
};

const unsigned char GxEPD2_420::lut_24_bb_partial[] PROGMEM =
{ // 01 b
  0x24, T1, T2, T3, T4, 1, // 00 10 01 00
  0x00,  1,  0,  0,  0, 1, // gnd phase
};

void GxEPD2_420::_Force_Init_Full()
{
  _Init_Full();
  _refresh_mode = forced_full_refresh;
}

void GxEPD2_420::_Init_Full()
{
  _InitDisplay();
  _writeCommand(0x20);
  _writeDataPGM(lut_20_vcom0_full, sizeof(lut_20_vcom0_full));
  _writeCommand(0x21);
  _writeDataPGM(lut_21_ww_full, sizeof(lut_21_ww_full));
  _writeCommand(0x22);
  _writeDataPGM(lut_22_bw_full, sizeof(lut_22_bw_full));
  _writeCommand(0x23);
  _writeDataPGM(lut_23_wb_full, sizeof(lut_23_wb_full));
  _writeCommand(0x24);
  _writeDataPGM(lut_24_bb_full, sizeof(lut_24_bb_full));
  _PowerOn();
  _refresh_mode = full_refresh;
}

void GxEPD2_420::_Init_4G()
{
  Serial.println("_Init_4G");
  _InitDisplay();
  _writeCommand(0x20);
  _writeDataPGM(lut_20_vcom0_4G, sizeof(lut_20_vcom0_4G));
  _writeCommand(0x21);
  _writeDataPGM(lut_21_ww_4G, sizeof(lut_21_ww_4G));
  _writeCommand(0x22);
  _writeDataPGM(lut_22_bw_4G, sizeof(lut_22_bw_4G));
  _writeCommand(0x23);
  _writeDataPGM(lut_23_wb_4G, sizeof(lut_23_wb_4G));
  _writeCommand(0x24);
  _writeDataPGM(lut_24_bb_4G, sizeof(lut_24_bb_4G));
  _PowerOn();
  _refresh_mode = grey_refresh;
}

void GxEPD2_420::_Init_Part()
{
  Serial.println("_Init_Part");
  _InitDisplay();
  _writeCommand(0x20);
  _writeDataPGM(lut_20_vcom0_partial, sizeof(lut_20_vcom0_partial), 44 - sizeof(lut_20_vcom0_partial));
  _writeCommand(0x21);
  _writeDataPGM(lut_21_ww_partial, sizeof(lut_21_ww_partial), 42 - sizeof(lut_21_ww_partial));
  _writeCommand(0x22);
  _writeDataPGM(lut_22_bw_partial, sizeof(lut_22_bw_partial), 42 - sizeof(lut_22_bw_partial));
  _writeCommand(0x23);
  _writeDataPGM(lut_23_wb_partial, sizeof(lut_23_wb_partial), 42 - sizeof(lut_23_wb_partial));
  _writeCommand(0x24);
  _writeDataPGM(lut_24_bb_partial, sizeof(lut_24_bb_partial), 42 - sizeof(lut_24_bb_partial));
  _PowerOn();
  _refresh_mode = fast_refresh;
}

void GxEPD2_420::_Update_Full()
{
  _writeCommand(0x12); //display refresh
  _waitWhileBusy("_Update_Full", full_refresh_time);
}

void GxEPD2_420::_Update_4G()
{
  _writeCommand(0x12); //display refresh
  _waitWhileBusy("_Update_4G", full_refresh_time);
}

void GxEPD2_420::_Update_Part()
{
  _writeCommand(0x12); //display refresh
  _waitWhileBusy("_Update_Part", partial_refresh_time);
}
