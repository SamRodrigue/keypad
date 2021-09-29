#include "keypad.h"

#include <stdio.h>
#include <string.h>

#define SCREEN_SIZE 16

static char const keys[KEY_IN_SIZE][KEY_OUT_SIZE] = {
	'1', '2', '3',
	'4', '5', '6',
	'7', '8', '9',
	'+', '0', '-'
};

static char const keys_alt[KEY_IN_SIZE][KEY_OUT_SIZE] = {
	'a', '\t', '\b',
	'd', '\v', '\a',
	'g', '\0', '\f',
	'=', '\r', '\n'
};

int main()
{
	stdio_init_all();
	keypad_init();

	struct keypad_event_queue event_queue;
	struct keypad_event *event;
	char screen[SCREEN_SIZE];
	uint8_t pos = 0;

	memset(screen, '\0', SCREEN_SIZE);

	while (1)
	{
		keypad_task(&event_queue);
		uint8_t event_size = event_queue.size;
		while (event_queue.size > 0)
		{
			--event_queue.size;
			event = &event_queue.events[event_queue.size];

			putchar('\f');
			for (uint8_t row = 0; row < KEY_IN_SIZE; ++row)
			{
				for (uint8_t col = 0; col < KEY_OUT_SIZE; ++col)
				{
					putchar(' ');
					if (keypad_get(row, col))
						putchar(keys[row][col]);
					else
						putchar('-');
				}
				putchar('\n');
			}

			switch (event->type)
			{
			case KEY_RELEASE:
				break;

			case KEY_PRESSED:
				screen[pos++] = keys[event->row][event->col];
				// putchar(keys[event->row][event->col]);
				break;

			default:
				break;
			}
			printf("events: %d\n", event_size);

			for (uint8_t i = 0; i < pos; ++i)
				putchar(screen[i]);
		}
	}

	return 0;
}