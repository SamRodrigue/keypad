#include "keypad.h"

// TODO: To be removed or replaced with either no checks or better error handling.
#include <stdio.h>
void passert(bool check, char const * message)
{
	if (check)
		return;

	printf(message);
	assert(false);
}

static bool _keypad_process(uint32_t input, uint8_t col, struct keypad_event_queue *event_queue)
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
			_keypad_process(input_state, col, event_queue);
			keypad_input_state[col] = input_state;
		}

		gpio_put(key_out[col], 1);
	}
}

bool keypad_get(uint8_t row, uint8_t col)
{
	passert(row < KEY_IN_SIZE && col < KEY_OUT_SIZE, "keypad_get out of range");
	return keypad_input_state[col] & (1ul << key_in[row]);
}


static void _gpio_set_pull_up_masked(uint32_t gpio_mask)
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
	_gpio_set_pull_up_masked(KEY_IN_MASK);
	gpio_put_masked(KEY_OUT_MASK, 1);
}