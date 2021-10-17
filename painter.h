#ifndef __PAINTER_H__
#define __PAINTER_H__

#include "display.h"

#include <string.h>

#define PAINTER_FONT_WIDTH 5
#define PAINTER_FONT_COUNT 12

#define PAINTER_FONT_COMS 16
#define PAINTER_FONT_PAGES 3

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

static uint8_t const painter_large_font[PAINTER_FONT_COUNT+1][PAINTER_FONT_COMS][PAINTER_FONT_PAGES] = {
	{{ 0x00, 0x00, 0x00 },{ 0xF0, 0xFF, 0x0F },{ 0xFC, 0xFF, 0x3F },{ 0xFE, 0xFF, 0x7F },{ 0xFE, 0xFF, 0x7F },{ 0x1F, 0x00, 0xF8 },{ 0x0F, 0x00, 0xF0 },{ 0x0F, 0x00, 0xF0 },{ 0x0F, 0x00, 0xF0 },{ 0x0F, 0x00, 0xF0 },{ 0x1F, 0x00, 0xF8 },{ 0xFE, 0xFF, 0x7F },{ 0xFE, 0xFF, 0x7F },{ 0xFC, 0xFF, 0x3F },{ 0xF0, 0xFF, 0x0F },{ 0x00, 0x00, 0x00 }},
	{{ 0x00, 0x00, 0x00 },{ 0x00, 0x00, 0x00 },{ 0x00, 0x00, 0x00 },{ 0x78, 0x00, 0x00 },{ 0x78, 0x00, 0x00 },{ 0x78, 0x00, 0x00 },{ 0x7C, 0x00, 0x00 },{ 0x7C, 0x00, 0x00 },{ 0xFE, 0xFF, 0xFF },{ 0xFF, 0xFF, 0xFF },{ 0xFF, 0xFF, 0xFF },{ 0xFF, 0xFF, 0xFF },{ 0xFF, 0xFF, 0xFF },{ 0x00, 0x00, 0x00 },{ 0x00, 0x00, 0x00 },{ 0x00, 0x00, 0x00 }},
	{{ 0x00, 0x00, 0x00 },{ 0xF0, 0x01, 0xFC },{ 0xFC, 0x01, 0xFF },{ 0xFE, 0x81, 0xFF },{ 0xFE, 0xC1, 0xFF },{ 0x1F, 0xE0, 0xFF },{ 0x0F, 0xE0, 0xF3 },{ 0x0F, 0xF0, 0xF1 },{ 0x0F, 0xF0, 0xF0 },{ 0x0F, 0xF8, 0xF0 },{ 0x1F, 0x7E, 0xF0 },{ 0xFE, 0x7F, 0xF0 },{ 0xFE, 0x3F, 0xF0 },{ 0xFC, 0x1F, 0xF0 },{ 0xF0, 0x07, 0xF0 },{ 0x00, 0x00, 0x00 }},
	{{ 0x00, 0x00, 0x00 },{ 0xF0, 0x01, 0x0F },{ 0xFC, 0x01, 0x3F },{ 0xFE, 0x01, 0x7F },{ 0xFE, 0x01, 0x7F },{ 0x1F, 0x00, 0xF8 },{ 0x0F, 0x00, 0xF0 },{ 0x0F, 0x3C, 0xF0 },{ 0x0F, 0x3C, 0xF0 },{ 0x0F, 0x3C, 0xF0 },{ 0x1F, 0x7E, 0xF8 },{ 0xFE, 0xFF, 0x7F },{ 0xFE, 0xFF, 0x7F },{ 0xFC, 0xF7, 0x3F },{ 0xF0, 0xE3, 0x1F },{ 0x00, 0x00, 0x00 }},
	{{ 0x00, 0x00, 0x00 },{ 0x00, 0xC0, 0x07 },{ 0x00, 0xF0, 0x07 },{ 0x00, 0xFC, 0x07 },{ 0x00, 0xFF, 0x07 },{ 0xC0, 0xBF, 0x07 },{ 0xF0, 0x8F, 0x07 },{ 0xFC, 0x83, 0x07 },{ 0xFF, 0xFF, 0xFF },{ 0xFF, 0xFF, 0xFF },{ 0xFF, 0xFF, 0xFF },{ 0xFF, 0xFF, 0xFF },{ 0xFF, 0xFF, 0xFF },{ 0x00, 0x80, 0x07 },{ 0x00, 0x80, 0x07 },{ 0x00, 0x00, 0x00 }},
	{{ 0x00, 0x00, 0x00 },{ 0xF0, 0x1F, 0x0F },{ 0xFE, 0x1F, 0x3F },{ 0xFF, 0x1F, 0x7F },{ 0xFF, 0x1F, 0x7F },{ 0x3F, 0x0F, 0xF8 },{ 0x8F, 0x07, 0xF0 },{ 0x8F, 0x07, 0xF0 },{ 0x8F, 0x07, 0xF0 },{ 0x8F, 0x07, 0xF0 },{ 0x8F, 0x0F, 0xF8 },{ 0x0F, 0xFF, 0x7F },{ 0x0F, 0xFF, 0x7F },{ 0x0F, 0xFE, 0x3F },{ 0x00, 0xF8, 0x0F },{ 0x00, 0x00, 0x00 }},
	{{ 0x00, 0x00, 0x00 },{ 0xF8, 0xFF, 0x0F },{ 0xFE, 0xFF, 0x3F },{ 0xFE, 0xFF, 0x7F },{ 0xFF, 0xFF, 0x7F },{ 0x1F, 0x0F, 0xF8 },{ 0x8F, 0x07, 0xF0 },{ 0x8F, 0x07, 0xF0 },{ 0x8F, 0x07, 0xF0 },{ 0x8F, 0x07, 0xF0 },{ 0x9F, 0x0F, 0xF8 },{ 0x3F, 0xFF, 0x7F },{ 0x3E, 0xFF, 0x7F },{ 0x3E, 0xFE, 0x3F },{ 0x38, 0xF8, 0x0F },{ 0x00, 0x00, 0x00 }},
	{{ 0x00, 0x00, 0x00 },{ 0x0F, 0x00, 0x00 },{ 0x0F, 0x00, 0x00 },{ 0x0F, 0x00, 0x00 },{ 0x0F, 0x00, 0xE0 },{ 0x0F, 0x00, 0xFC },{ 0x0F, 0x80, 0xFF },{ 0x0F, 0xF0, 0xFF },{ 0x0F, 0xFE, 0xFF },{ 0xCF, 0xFF, 0x1F },{ 0xFF, 0xFF, 0x03 },{ 0xFF, 0x7F, 0x00 },{ 0xFF, 0x0F, 0x00 },{ 0xFF, 0x01, 0x00 },{ 0x3F, 0x00, 0x00 },{ 0x00, 0x00, 0x00 }},
	{{ 0x00, 0x00, 0x00 },{ 0xF0, 0xE3, 0x0F },{ 0xFC, 0xF7, 0x3F },{ 0xFE, 0xFF, 0x7F },{ 0xFE, 0xFF, 0x7F },{ 0x1F, 0x7E, 0xF8 },{ 0x0F, 0x3C, 0xF0 },{ 0x0F, 0x3C, 0xF0 },{ 0x0F, 0x3C, 0xF0 },{ 0x0F, 0x3C, 0xF0 },{ 0x1F, 0x7E, 0xF8 },{ 0xFE, 0xFF, 0x7F },{ 0xFE, 0xFF, 0x7F },{ 0xFC, 0xF7, 0x3F },{ 0xF0, 0xE3, 0x1F },{ 0x00, 0x00, 0x00 }},
	{{ 0x00, 0x00, 0x00 },{ 0xF0, 0x1F, 0x0C },{ 0xFC, 0x7F, 0x3C },{ 0xFE, 0xFF, 0x7C },{ 0xFE, 0xFF, 0x7C },{ 0x1F, 0xF0, 0xF9 },{ 0x0F, 0xE0, 0xF1 },{ 0x0F, 0xE0, 0xF1 },{ 0x0F, 0xE0, 0xF1 },{ 0x0F, 0xE0, 0xF1 },{ 0x1F, 0xF0, 0xF8 },{ 0xFE, 0xFF, 0x7F },{ 0xFE, 0xFF, 0x7F },{ 0xFC, 0xFF, 0x3F },{ 0xF0, 0xFF, 0x1F },{ 0x00, 0x00, 0x00 }},
	{{ 0x00, 0x00, 0x00 },{ 0x00, 0x3C, 0x00 },{ 0x00, 0x3C, 0x00 },{ 0x00, 0x3C, 0x00 },{ 0x00, 0x3C, 0x00 },{ 0x00, 0x3C, 0x00 },{ 0xE0, 0xFF, 0x07 },{ 0xE0, 0xFF, 0x07 },{ 0xE0, 0xFF, 0x07 },{ 0xE0, 0xFF, 0x07 },{ 0x00, 0x3C, 0x00 },{ 0x00, 0x3C, 0x00 },{ 0x00, 0x3C, 0x00 },{ 0x00, 0x3C, 0x00 },{ 0x00, 0x3C, 0x00 },{ 0x00, 0x00, 0x00 }},
	{{ 0x00, 0x00, 0x00 },{ 0x00, 0x3C, 0x00 },{ 0x00, 0x3C, 0x00 },{ 0x00, 0x3C, 0x00 },{ 0x00, 0x3C, 0x00 },{ 0x00, 0x3C, 0x00 },{ 0x00, 0x3C, 0x00 },{ 0x00, 0x3C, 0x00 },{ 0x00, 0x3C, 0x00 },{ 0x00, 0x3C, 0x00 },{ 0x00, 0x3C, 0x00 },{ 0x00, 0x3C, 0x00 },{ 0x00, 0x3C, 0x00 },{ 0x00, 0x3C, 0x00 },{ 0x00, 0x3C, 0x00 },{ 0x00, 0x00, 0x00 }},
	{{ 0x00, 0x00, 0x00 },{ 0x00, 0x00, 0x00 },{ 0x00, 0x00, 0x00 },{ 0x00, 0x00, 0x00 },{ 0x00, 0x00, 0x00 },{ 0x00, 0x00, 0x00 },{ 0x00, 0x00, 0x00 },{ 0x00, 0x00, 0x00 },{ 0x00, 0x00, 0x00 },{ 0x00, 0x00, 0x00 },{ 0x00, 0x00, 0x00 },{ 0x00, 0x00, 0x00 },{ 0x00, 0x00, 0x00 },{ 0x00, 0x00, 0x00 },{ 0x00, 0x00, 0x00 },{ 0x00, 0x00, 0x00 }}
};

typedef struct end
{
	uint8_t com;
	uint8_t page;
} end_t;

typedef struct shelf
{
	bool needs_flush;
	end_t high_end;
	end_t low_end;
} shelf_t;

typedef struct painter
{
	display_t *display;
	uint8_t count;
	shelf_t shelf;
} painter_t;

static painter_t painter;


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


static uint8_t const * painter_large_font_bitmap(char text)
{
	if (text >= '0' && text <= '9')
		return painter_large_font[text - '0'];
	else if (text == '+')
		return painter_large_font[10];
	else if (text == '-')
		return painter_large_font[11];
	else if (text == ' ')
		return painter_large_font[12];

	return NULL;
}


static void painter_make_dirty(uint8_t x, uint8_t y, uint16_t width, uint8_t height)
{
	uint8_t com = x;
	uint8_t page = y / 8;

	if (!painter.shelf.needs_flush)
	{
		painter.shelf.needs_flush = true;

		painter.shelf.low_end.com = com;
		painter.shelf.low_end.page = page;

		com += width - 1;
		page = (y + height) / 8 - 1;

		painter.shelf.high_end.com = com;
		painter.shelf.high_end.page = page;

		return;
	}

	if (com < painter.shelf.low_end.com)
		painter.shelf.low_end.com = com;

	if (page < painter.shelf.low_end.page)
		painter.shelf.low_end.page = page;

	com += width - 1;
	if (com > painter.shelf.high_end.com)
		painter.shelf.high_end.com = com;

	page = (y + height) / 8 - 1;
	if (page > painter.shelf.high_end.page)
		painter.shelf.high_end.page = page;
}


static void painter_map_page(uint8_t byte, uint8_t x, uint8_t y)
{
	display_t *display = &painter.display[x / DISPLAY_COMS];
	uint8_t com = x % DISPLAY_COMS;
	uint8_t page = y / 8;

	// Y position is at the page boundary
	if (y % 8 == 0)
	{
		display->buffer.data[com][page] = byte;
		return;
	}

	// Y position crosses the page boundary
	// TODO: Clean up variable usage
	uint8_t overlap = y - (8 * (y / 8));
	uint8_t mask = 0xff << overlap;
	display->buffer.data[com][page] &= ~mask;
	display->buffer.data[com][page] |= byte << overlap;

	page++;
	overlap = 8 - overlap;
	mask = 0xff >> overlap;
	display->buffer.data[com][page] &= ~mask;
	display->buffer.data[com][page] |= byte >> overlap;
}


void painter_text(char const text, uint8_t x, uint8_t y)
{
	uint8_t const *bitmap = painter_font_bitmap(text);
	if (!bitmap)
		return;

	for (uint8_t i = 0; i < PAINTER_FONT_WIDTH; ++i)
		painter_map_page(bitmap[i], x+i, y);

	painter_make_dirty(x, y, PAINTER_FONT_WIDTH, 8);
}


void painter_large_text(char const text, uint8_t x, uint8_t y)
{
	uint8_t const *bitmap = painter_large_font_bitmap(text);
	if (!bitmap)
		return;

	for (uint8_t com = 0; com < PAINTER_FONT_COMS; ++com)
		for (uint8_t page = 0; page < PAINTER_FONT_PAGES; ++page)
			painter_map_page(bitmap[com * PAINTER_FONT_PAGES + page], x + com, y + 8 * page);

	painter_make_dirty(x, y, PAINTER_FONT_COMS, 8 * PAINTER_FONT_PAGES);
}

void painter_field(field_t const *field)
{
	for (uint8_t i = field->shift; i < field->size; ++i)
	{
		uint8_t x = field->x + 16 * (i - field->shift);
		painter_large_text(field->text[i], x, field->y);
	}

	if (field->size >= field->width)
		return;

	for (uint8_t i = field->size - field->shift; i < field->width; ++i)
		painter_large_text(' ', field->x + 16 * i, field->y);
}


void painter_clear()
{
	for (uint8_t d = 0; d < 2; ++d)
		display_clear(&painter.display[d]);

	painter_make_dirty(0, 0, 2 * DISPLAY_COMS, 8 * DISPLAY_PAGES);
}


void painter_flush()
{
	if (!painter.shelf.needs_flush)
		return;

	bool flush_display[2] = { false, false };

	if (painter.shelf.low_end.com < DISPLAY_COMS)
		flush_display[0] = true;
	if (painter.shelf.high_end.com >= DISPLAY_COMS)
		flush_display[1] = true;

	// TODO: rework some of these variables. high_end being reused across multiple scopes.
	end_t low_end = painter.shelf.low_end;
	end_t high_end = painter.shelf.high_end;

	if (flush_display[0])
	{
		display_draw(&painter.display[0]);
		// if (flush_display[1])
		// 	high_end.com = DISPLAY_COMS - 1;

		// display_resize(low_end.com, low_end.page, high_end.com, high_end.page, &painter.display[0]);
		// display_draw(low_end.com, low_end.page, high_end.com, high_end.page, &painter.display[0]);
	}

	high_end = painter.shelf.high_end;

	if (flush_display[1])
	{
		display_draw(&painter.display[1]);
		// if (flush_display[0])
		// 	low_end.com = DISPLAY_COMS;

		// display_resize(low_end.com % DISPLAY_COMS, low_end.page, high_end.com % DISPLAY_COMS, high_end.page, &painter.display[0]);
		// display_draw(low_end.com % DISPLAY_COMS, low_end.page, high_end.com % DISPLAY_COMS, high_end.page, &painter.display[1]);
	}

	painter.shelf.needs_flush = false;

	printf("\ndirty %d %d, %d %d\n", painter.shelf.low_end.com, painter.shelf.low_end.page, painter.shelf.high_end.com, painter.shelf.high_end.page);
}


void painter_init(display_t *display, uint8_t count)
{
	painter.display = display;
	painter.count = count;
	painter.shelf.needs_flush = false;

	painter_clear();
	painter_flush();
}

#endif