#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA 4
#define I2C_SCL 5

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

static uint32_t input_state_data[2][KEY_OUT_SIZE];
static bool input_state_swapped;
static uint32_t *input_state;
static uint32_t *input_state_previous;
static bool alt_key_printable;

#define ALT_COL 0
#define ALT_ROW 3

bool alt_held()
{
	return input_state[0] & (1ul << key_in[3]);
}

bool is_alt_key(size_t row, size_t col)
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
		if (~input_state[col] & input & mask)
		{
			// Alt key
			if (is_alt_key(row, col))
			{
				alt_key_printable = true;
				continue;
			}

			// Is alt key currently held
			if (alt_held(input_state))
			{
				alt_key_printable = false;
				putchar(keys_alt[row][col]);
			}
			else
				putchar(keys[row][col]);
		}

		// Event Release Key
		else if (input_state[col] & ~input & mask)
		{
			// If alt is released
			if (is_alt_key(row, col))
			{
				if (alt_key_printable)
					putchar(keys[row][col]);
			}
		}
	}
}


void keypad_task()
{
	bool cycle_input_state = false;
	for (size_t col = 0; col < KEY_OUT_SIZE; ++col)
	{
		time_off();
		gpio_put(key_out[col], 0);
		time_on();

		uint32_t keypad_in_state = ~gpio_get_all() & KEY_IN;
		if (input_state[col] != keypad_in_state)
		{
			cycle_input_state = true;
			keypad_process(keypad_in_state, col);
			input_state[col] = keypad_in_state;
		}

		gpio_put(key_out[col], 1);
	}
}


int main()
{
	stdio_init_all();

	// I2C Initialisation. Using it at 400Khz.
	i2c_init(I2C_PORT, 400*1000);

	gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
	gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
	gpio_pull_up(I2C_SDA);
	gpio_pull_up(I2C_SCL);

	// Keypad GPIO
	gpio_init(KEY_IN_0);
	gpio_init(KEY_IN_1);
	gpio_init(KEY_IN_2);
	gpio_init(KEY_IN_3);
	gpio_set_dir(KEY_IN_0, GPIO_IN);
	gpio_set_dir(KEY_IN_1, GPIO_IN);
	gpio_set_dir(KEY_IN_2, GPIO_IN);
	gpio_set_dir(KEY_IN_3, GPIO_IN);
	gpio_pull_up(KEY_IN_0);
	gpio_pull_up(KEY_IN_1);
	gpio_pull_up(KEY_IN_2);
	gpio_pull_up(KEY_IN_3);

	// Configure GPIO IRQ
	// gpio_set_irq_enabled_with_callback(KEY_IN_0, GPIO_IRQ_EDGE_FALL, true, &keypad_event);
	// gpio_set_irq_enabled(KEY_IN_1, GPIO_IRQ_EDGE_FALL, true);
	// gpio_set_irq_enabled(KEY_IN_2, GPIO_IRQ_EDGE_FALL, true);
	// gpio_set_irq_enabled(KEY_IN_3, GPIO_IRQ_EDGE_FALL, true);

	gpio_init(KEY_OUT_0);
	gpio_init(KEY_OUT_1);
	gpio_init(KEY_OUT_2);
	gpio_set_dir(KEY_OUT_0, GPIO_OUT);
	gpio_set_dir(KEY_OUT_1, GPIO_OUT);
	gpio_set_dir(KEY_OUT_2, GPIO_OUT);
	gpio_put(KEY_OUT_0, 1);
	gpio_put(KEY_OUT_1, 1);
	gpio_put(KEY_OUT_2, 1);

	input_state_swapped = false;
	input_state = input_state_data[0];
	input_state_previous = input_state_data[1];

	while (1)
	{
		keypad_task();
	}

	return 0;
}
