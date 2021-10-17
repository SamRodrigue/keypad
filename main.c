// Keypad GPIO index and order
#define KEYPAD_IN 12, 13, 14, 15
#define KEYPAD_OUT 9, 10, 11

#include "keypad.h"
#include "display.h"
#include "field.h"
#include "painter.h"

#include <stdio.h>

#include "pico/stdlib.h"
#include "pico/multicore.h"

#define DISPLAY0_ADDR 0x3c
#define DISPLAY1_ADDR 0x3d

#define SCREEN_SIZE 36

static char const keys[KEYPAD_IN_SIZE][KEYPAD_OUT_SIZE] = {
	'1', '2', '3',
	'4', '5', '6',
	'7', '8', '9',
	'+', '0', '-'
};

static char const keys_alt[KEYPAD_IN_SIZE][KEYPAD_OUT_SIZE] = {
	'a', ' ', '\b',
	'd', '\v', '\a',
	'g', '\0', '\f',
	'=', '\r', '\n'
};

static char screen[SCREEN_SIZE];
static uint8_t cursor;

static display_t displays[2];
static field_t field[2];


void print_keypad()
{
	putchar('\f');
	for (uint8_t row = 0; row < KEYPAD_IN_SIZE; ++row)
	{
		for (uint8_t col = 0; col < KEYPAD_OUT_SIZE; ++col)
		{
			putchar(' ');
			if (keypad_state(row, col))
			{
				putchar(keys[row][col]);
				painter_text(keys[row][col], 10*col, 10*row);
			}
			else
			{
				putchar('-');
				painter_text('-', 10*col, 10*row);
			}
		}
		putchar('\n');

	}
}

// Action to perform on a keypad event
void key_action(struct keypad_event const *event)
{
	print_keypad();

	switch (event->type)
	{
	case KEY_RELEASE:
		break;

	case KEY_PRESSED:
		field_putchar(keys[event->row][event->col], &field[0]);
		painter_field(&field[0]);
		field_putchar(keys[event->row][event->col], &field[1]);
		painter_field(&field[1]);
		if (++cursor == SCREEN_SIZE)
			cursor = 0;
		screen[cursor] = keys[event->row][event->col];
		break;

	case UNDEFINED:
		passert(false, "Undefined event received");
	}
}


int main()
{
	stdio_init_all();

	displays[0].address = DISPLAY0_ADDR;
	displays[1].address = DISPLAY1_ADDR;

	for (uint8_t d = 0; d < 2; ++d)
		display_init(4, 5, &displays[d]);

	painter_init(displays, 2);

	field_init(32, 6, 14, 20, &field[0]);
	field_init(32, 38, 14, 14, &field[1]);

	keypad_init();

	multicore_launch_core1(&keypad_main);

	while (1)
	{
		uint32_t index = multicore_fifo_pop_blocking();
		struct keypad_event event;
		keypad_get_event(index, &event);
		key_action(&event);
		//passert(multicore_fifo_rvalid(), "Message still in fifo"); // Check for missed events
		painter_flush();
	}

	return 0;
}