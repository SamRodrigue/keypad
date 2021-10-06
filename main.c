// Keypad GPIO index and order
#define KEYPAD_IN 12, 13, 14, 15
#define KEYPAD_OUT 9, 10, 11
#include "keypad.h"

#include <stdio.h>
#include <string.h>

#include "pico/stdlib.h"
#include "pico/multicore.h"

#define SCREEN_SIZE 16

static char const keys[KEYPAD_IN_SIZE][KEYPAD_OUT_SIZE] = {
	'1', '2', '3',
	'4', '5', '6',
	'7', '8', '9',
	'+', '0', '-'
};

static char const keys_alt[KEYPAD_IN_SIZE][KEYPAD_OUT_SIZE] = {
	'a', '\t', '\b',
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
				putchar(keys[row][col]);
			else
				putchar('-');
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
		if (++cursor == SCREEN_SIZE)
			cursor = 0;
		screen[cursor] = keys[event->row][event->col];
		break;

	case UNDEFINED:
		passert(false, "Undefined event received");
	}

	for (uint8_t i = 0; i <= cursor; ++i)
		putchar(screen[i]);
}


int main()
{
	stdio_init_all();
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
	}

	return 0;
}

// TODO, shared unordered memory. Pass pointers or index to multicore fifo
// index = multicore_fifo_pop_blocking(); // Causes core to wait for message.

// if (mutlicore_fif_rvalid()) // Check for missed events