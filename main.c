// Keypad GPIO index and order
#define KEYPAD_IN 12, 13, 14, 15
#define KEYPAD_OUT 9, 10, 11

#include "keypad.h"
#include "display.h"

#include <stdio.h>

#include "pico/stdlib.h"
#include "pico/multicore.h"

#define SCREEN_SIZE 16

static uint8_t const font[] = {
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
				for (uint8_t i = 0; i < 5; ++i)
					display_buffer.data[DISPLAY_COMS - 1 - (6*col + i)][row] = font[5*(keys[row][col] - '0')+i];
			}
			else
			{
				putchar('-');
				for (uint8_t i = 0; i < 5; ++i)
					display_buffer.data[DISPLAY_COMS - 1 - (6*col + i)][row] = font[5*11+i];
			}
		}
		putchar('\n');
		display();
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
		for (uint8_t j = 0; j < 5; ++j)
			display_buffer.data[DISPLAY_COMS - 1 - (6*i + j)][7] = font[5*(screen[i] - '0')+j];
			
	}
}


int main()
{
	stdio_init_all();
	display_init();
	keypad_init();

	print_keypad();

	cursor = SCREEN_SIZE - 1;
	memset(screen, '\0', SCREEN_SIZE);

	multicore_launch_core1(&keypad_main);

	while (1)
	{
		uint32_t index = multicore_fifo_pop_blocking();
		struct keypad_event event;
		keypad_get_event(index, &event);
		key_action(&event);
		// TODO: if (mutlicore_fif_rvalid()) // Check for missed events
	}

	return 0;
}