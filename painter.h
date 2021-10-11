#ifndef __PAINTER_H__
#define __PAINTER_H__

#include "display.h"

#include <string.h>

#define PAINTER_FONT_WIDTH 5
#define PAINTER_FONT_COUNT 12

static uint8_t const painter_font[PAINTER_FONT_COUNT][PAINTER_FONT_WIDTH] = {
	0x7E, 0xA1, 0x91, 0x89, 0x7E, // 0
	0x80, 0x86, 0xFF, 0x80, 0x80, // 1
	0xC6, 0xA1, 0x91, 0x91, 0x8E, // 2
	0x46, 0x81, 0x91, 0x91, 0x6E, // 3
	0x30, 0x28, 0x26, 0x21, 0xFF, // 4
	0x4F, 0x89, 0x89, 0x89, 0x71, // 5
	0x78, 0x96, 0x91, 0x91, 0x60, // 6
	0x07, 0x01, 0xE1, 0x11, 0x0F, // 7
	0x6E, 0x91, 0x91, 0x91, 0x6E, // 8
	0x0E, 0x91, 0x91, 0x51, 0x3E, // 9
	0x10, 0x10, 0x7E, 0x10, 0x10, // +
	0x10, 0x10, 0x10, 0x10, 0x10  // -
};

// display[d].buffer.data[6*col + i][row] = font[5*(keys[row][col] - '0')+i];

static uint8_t const * painter_font_bitmap(char text)
{
   if (text >= '0' && text <= '9')
      return painter_font[text - '0'];
   else if (text == '+')
      return painter_font[10];
   else if (text == '-')
      return painter_font[11];

   return NULL;
}

void painter_text(const char text, uint8_t x, uint8_t y, display_t *display)
{
   uint8_t const * bitmap = painter_font_bitmap(text);
   if (!bitmap)
      return;

   for (uint8_t i = 0; i < PAINTER_FONT_WIDTH; ++i)
      display->buffer.data[x+i][y] = bitmap[i];
}

#endif