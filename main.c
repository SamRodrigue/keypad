// Keypad GPIO index and order
#define KEYPAD_IN 12, 13, 14, 15
#define KEYPAD_OUT 9, 10, 11

#include "keypad.h"
#include "display.h"
#include "painter.h"

#include <stdio.h>

#include "pico/stdlib.h"
#include "pico/multicore.h"

#define DISPLAY0_ADDR 0x3c
#define DISPLAY1_ADDR 0x3d

#define SCREEN_SIZE 16

static char const keys[KEYPAD_IN_SIZE][KEYPAD_OUT_SIZE] = {
	'1', '2', '3',
	'4', '5', '6',
	'7', '8', '9',
	':', '0', ':'
};

static char const keys_alt[KEYPAD_IN_SIZE][KEYPAD_OUT_SIZE] = {
	'a', ' ', '\b',
	'd', '\v', '\a',
	'g', '\0', '\f',
	'=', '\r', '\n'
};

static char screen[SCREEN_SIZE];
static uint8_t cursor;

static display_t display[2];


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
				painter_text(keys[row][col], 10*col, row, &display[0]);
			}
			else
			{
				putchar('-');
				painter_text('-', 10*col, row, &display[0]);
			}
		}
		putchar('\n');

		display_draw(&display[0]);
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
		if (++cursor == SCREEN_SIZE)
			cursor = 0;
		screen[cursor] = keys[event->row][event->col];
		break;

	case UNDEFINED:
		passert(false, "Undefined event received");
	}

	for (uint8_t i = 0; i <= cursor; ++i)
	{
		putchar(screen[i]);
		painter_text(screen[i], 7*i, 7, &display[1]);
	}

	display_draw(&display[1]);
}


int main()
{
	stdio_init_all();

	display[0].address = DISPLAY0_ADDR;
	display[1].address = DISPLAY1_ADDR;

	for (uint8_t d = 0; d < 2; ++d)
		display_init(4, 5, &display[d]);

	// painter_init_canvas()

	keypad_init();

	cursor = SCREEN_SIZE - 1;
	memset(screen, '\0', SCREEN_SIZE);
	print_keypad();

	multicore_launch_core1(&keypad_main);

	while (1)
	{
		uint32_t index = multicore_fifo_pop_blocking();
		struct keypad_event event;
		keypad_get_event(index, &event);
		key_action(&event);
		//passert(multicore_fifo_rvalid(), "Message still in fifo"); // Check for missed events
		//painter_flush();
	}

	return 0;
}