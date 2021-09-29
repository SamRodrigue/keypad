#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"

// TODO: To be removed or replaced with either no checks or better error handling.
void passert(bool check, char const * message)
{
	if (check)
		return;

	printf(message);
	assert(false);
}

// Keypad defines
#define KEY_IN_SIZE 4
#define KEY_IN_0 9
#define KEY_IN_1 10
#define KEY_IN_2 11
#define KEY_IN_3 12

#define KEY_OUT_SIZE 3
#define KEY_OUT_0 13
#define KEY_OUT_1 14
#define KEY_OUT_2 15

#define KEY_IN_MASK \
	((1ul << KEY_IN_0) | \
	 (1ul << KEY_IN_1) | \
	 (1ul << KEY_IN_2) | \
	 (1ul << KEY_IN_3))

#define KEY_OUT_MASK \
	((1ul << KEY_OUT_0) | \
	 (1ul << KEY_OUT_1) | \
	 (1ul << KEY_OUT_2))

#define KEY_ALT_COL 0
#define KEY_ALT_ROW 3

static uint32_t const key_in[KEY_IN_SIZE] = { KEY_IN_3, KEY_IN_2, KEY_IN_1, KEY_IN_0 };
static uint32_t const key_out[KEY_OUT_SIZE] = { KEY_OUT_2, KEY_OUT_1, KEY_OUT_0 };

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

static uint32_t keypad_input_state[KEY_OUT_SIZE] = { 0, 0, 0 };

enum event_type
{
	KEY_RELEASE,
	KEY_PRESSED,
	NONE
};

struct keypad_event
{
	enum event_type type;
	uint8_t col;
	uint8_t row;
};

#define KEYPAD_MAX_EVENT_SIZE 4

struct keypad_event_queue
{
	uint8_t size;
	struct keypad_event events[KEYPAD_MAX_EVENT_SIZE];
};

bool keypad_process(uint32_t input, uint8_t col, struct keypad_event_queue *event_queue)
{
	for (uint8_t row = 0; row < KEY_IN_SIZE; ++row)
	{
		struct keypad_event *event;
		uint32_t mask = 1ul << key_in[row];

		bool previous = keypad_input_state[col] & mask;
		bool current = input & mask;

		if (previous == current)
			continue;

		event = &event_queue->events[event_queue->size];

		passert(++event_queue->size <= KEYPAD_MAX_EVENT_SIZE,
		        "keypad_process: event_queue size exceeded");

		event->type = current ? KEY_PRESSED : KEY_RELEASE;
		event->col = col;
		event->row = row;
	}
}


#define time_off() sleep_ms(1)
#define time_on() sleep_ms(1)

void keypad_task(struct keypad_event_queue *event_queue)
{
	event_queue->size = 0;
	for (uint8_t col = 0; col < KEY_OUT_SIZE; ++col)
	{
		time_off();
		gpio_put(key_out[col], 0);
		time_on();

		uint32_t input_state = ~gpio_get_all() & KEY_IN_MASK;
		if (keypad_input_state[col] != input_state)
		{
			keypad_process(input_state, col, event_queue);
			keypad_input_state[col] = input_state;
		}

		gpio_put(key_out[col], 1);
	}
}


void gpio_set_pull_up_masked(uint32_t gpio_mask)
{
	for(uint gpio = 0; gpio < 32; ++gpio) {
		if (gpio_mask & 1)
			gpio_pull_up(gpio);

		gpio_mask >>= 1;
	}
}


void keypad_init()
{
	// GPIO
	gpio_init_mask(KEY_IN_MASK | KEY_OUT_MASK);
	gpio_set_dir_in_masked(KEY_IN_MASK);
	gpio_set_dir_out_masked(KEY_OUT_MASK);
	gpio_set_pull_up_masked(KEY_IN_MASK);
	gpio_put_masked(KEY_OUT_MASK, 1);
}

#define SCREEN_SIZE 16

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
					if (keypad_input_state[col] & (1ul << key_in[row]))
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
