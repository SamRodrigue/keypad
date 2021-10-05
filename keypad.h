#ifndef _KEYPAD_H_
#define _KEYPAD_H_

#include "pico/stdlib.h"
#include <stdio.h>
#define passert(check, message) if (!(check)) { printf("%s:%d %s", __FILE__, __LINE__, message); assert(false); }

#ifndef KEYPAD_OUT
   #error KEYPAD_OUT must be defined example: #define KEYPAD_OUT 1, 2, 3
#endif
#ifndef KEYPAD_IN
   #error KEYPAD_IN must be defined example: #define KEYPAD_IN 4, 5
#endif

#define KEYPAD_LEVEL_ON 1
#define KEYPAD_LEVEL_OFF !KEYPAD_LEVEL_ON

#define KEYPAD_COUNT_N(_0, _1, _2, _3, _4, _5, _6, _7, N, ...) N
#define KEYPAD_COUNT(...) KEYPAD_COUNT_N(__VA_ARGS__, 8, 7, 6, 5, 4, 3, 2, 1)

#define KEYPAD_MASK_1(_1) (1ul << _1)
#define KEYPAD_MASK_2(_1, ...) KEYPAD_MASK_1(_1) | KEYPAD_MASK_1(__VA_ARGS__)
#define KEYPAD_MASK_3(_1, ...) KEYPAD_MASK_1(_1) | KEYPAD_MASK_2(__VA_ARGS__)
#define KEYPAD_MASK_4(_1, ...) KEYPAD_MASK_1(_1) | KEYPAD_MASK_3(__VA_ARGS__)
#define KEYPAD_MASK_5(_1, ...) KEYPAD_MASK_1(_1) | KEYPAD_MASK_4(__VA_ARGS__)
#define KEYPAD_MASK_6(_1, ...) KEYPAD_MASK_1(_1) | KEYPAD_MASK_5(__VA_ARGS__)
#define KEYPAD_MASK_7(_1, ...) KEYPAD_MASK_1(_1) | KEYPAD_MASK_6(__VA_ARGS__)
#define KEYPAD_MASK_8(_1, ...) KEYPAD_MASK_1(_1) | KEYPAD_MASK_7(__VA_ARGS__)
#define KEYPAD_MASK(...) (KEYPAD_COUNT_N(__VA_ARGS__, \
	                                KEYPAD_MASK_8, KEYPAD_MASK_7, KEYPAD_MASK_6, KEYPAD_MASK_5, \
	                                KEYPAD_MASK_4, KEYPAD_MASK_3, KEYPAD_MASK_2, KEYPAD_MASK_1)(__VA_ARGS__))

#define KEYPAD_IN_SIZE KEYPAD_COUNT(KEYPAD_IN)
#define KEYPAD_OUT_SIZE KEYPAD_COUNT(KEYPAD_OUT)

#define KEYPAD_IN_MASK KEYPAD_MASK(KEYPAD_IN)
#define KEYPAD_OUT_MASK KEYPAD_MASK(KEYPAD_OUT)

static uint32_t const keypad_in[] = { KEYPAD_IN };
static uint32_t const keypad_out[] = { KEYPAD_OUT };

static uint32_t keypad_input_state[KEYPAD_OUT_SIZE] = { 0 };

enum event_type
{
	KEY_RELEASE,
	KEY_PRESSED
};

struct keypad_event
{
	enum event_type type;
	uint8_t col;
	uint8_t row;
};


#define time_off() sleep_ms(1)
#define time_on() sleep_ms(1)

void keypad_task(void (*action)(struct keypad_event))
{
	for (uint8_t col = 0; col < KEYPAD_OUT_SIZE; ++col)
	{
		time_off();
		gpio_put(keypad_out[col], KEYPAD_LEVEL_ON);
		time_on();

		uint32_t input_state = ((KEYPAD_LEVEL_ON == 1) ? gpio_get_all() : ~gpio_get_all()) & KEYPAD_IN_MASK;
		if (keypad_input_state[col] != input_state)
		{
			for (uint8_t row = 0; row < KEYPAD_IN_SIZE; ++row)
			{
				uint32_t mask = 1ul << keypad_in[row];

				uint32_t previous_state = keypad_input_state[col] & mask;
				uint32_t current_state = input_state & mask;

				if (previous_state == current_state)
					continue;

				keypad_input_state[col] ^= mask;
				
				struct keypad_event event = {
					current_state == 0ul ? KEY_RELEASE : KEY_PRESSED,
					col, row };

				action(event);
			}
		}

		gpio_put(keypad_out[col], KEYPAD_LEVEL_OFF);
	}
}


bool keypad_get(uint8_t row, uint8_t col)
{
	passert(row < KEYPAD_IN_SIZE && col < KEYPAD_OUT_SIZE, "keypad_get out of range");
	return keypad_input_state[col] & (1ul << keypad_in[row]);
}


static void _gpio_set_pull_masked(uint32_t gpio_mask)
{
	for(uint gpio = 0; gpio < 32; ++gpio) {
		if (gpio_mask & 1ul)
			gpio_set_pulls(gpio, KEYPAD_LEVEL_ON == 0, KEYPAD_LEVEL_ON == 1);

		gpio_mask >>= 1;
	}
}


void keypad_init()
{
	// GPIO
	gpio_init_mask(KEYPAD_IN_MASK | KEYPAD_OUT_MASK);
	gpio_set_dir_in_masked(KEYPAD_IN_MASK);
	gpio_set_dir_out_masked(KEYPAD_OUT_MASK);
   _gpio_set_pull_masked(KEYPAD_IN_MASK);
	gpio_put_masked(KEYPAD_OUT_MASK, (KEYPAD_LEVEL_ON == 1) ? 0x0ul : ~0x0ul);
}

#endif