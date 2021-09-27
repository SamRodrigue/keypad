#include <stdio.h>
#include "pico/stdlib.h"

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

#define KEY_IN \
	((1ul << KEY_IN_0) | \
	 (1ul << KEY_IN_1) | \
	 (1ul << KEY_IN_2) | \
	 (1ul << KEY_IN_3))

#define KEY_OUT \
	((1ul << KEY_OUT_0) | \
	 (1ul << KEY_OUT_1) | \
	 (1ul << KEY_OUT_2))

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
static bool alt_key_printable;

#define ALT_COL 0
#define ALT_ROW 3


bool keypad_is_alt_held()
{
	return keypad_input_state[ALT_COL] & (1ul << key_in[ALT_ROW]);
}


bool keypad_is_alt_key(size_t row, size_t col)
{
	return row == ALT_ROW && col == ALT_COL;
}


#define time_off() sleep_ms(1)
#define time_on() sleep_ms(1)


bool keypad_process(uint32_t input, size_t col)
{
	for (size_t row = 0; row < KEY_IN_SIZE; ++row)
	{
		uint32_t mask = 1ul << key_in[row];

		// Event Pressed
		if (~keypad_input_state[col] & input & mask)
		{
			// Alt key
			if (keypad_is_alt_key(row, col))
			{
				alt_key_printable = true;
				continue;
			}

			// Is alt key currently held
			if (keypad_is_alt_held(keypad_input_state))
			{
				alt_key_printable = false;
				putchar(keys_alt[row][col]);
			}
			else
				putchar(keys[row][col]);
		}

		// Event Release Key
		else if (keypad_input_state[col] & ~input & mask)
		{
			// If alt is released
			if (keypad_is_alt_key(row, col))
			{
				if (alt_key_printable)
					putchar(keys[row][col]);
			}
		}
	}
}


void keypad_task()
{
	for (size_t col = 0; col < KEY_OUT_SIZE; ++col)
	{
		time_off();
		gpio_put(key_out[col], 0);
		time_on();

		uint32_t input_state = ~gpio_get_all() & KEY_IN;
		if (keypad_input_state[col] != input_state)
		{
			keypad_process(input_state, col);
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
	gpio_init_mask(KEY_IN | KEY_OUT);
	gpio_set_dir_in_masked(KEY_IN);
	gpio_set_dir_out_masked(KEY_OUT);
	gpio_set_pull_up_masked(KEY_IN);
	gpio_put_masked(KEY_OUT, 1);
}


int main()
{
	stdio_init_all();
	keypad_init();

	while (1)
	{
		keypad_task();
	}

	return 0;
}
