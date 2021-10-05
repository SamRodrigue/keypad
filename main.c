// Keypad GPIO index and order
#define KEYPAD_IN 12, 13, 14, 15
#define KEYPAD_OUT 9, 10, 11
#include "keypad.h"

#include <stdio.h>
#include <string.h>

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
			if (keypad_get(row, col))
				putchar(keys[row][col]);
			else
				putchar('-');
		}
		putchar('\n');
	}
}

// Action to perform on a keypad event
void key_action(struct keypad_event event)
{
	print_keypad();

	switch (event.type)
	{
	case KEY_RELEASE:
		break;

	case KEY_PRESSED:
		if (++cursor == SCREEN_SIZE)
			cursor = 0;
		screen[cursor] = keys[event.row][event.col];
		break;
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

	while (1)
	{
		keypad_task(&key_action);
	}

	return 0;
}