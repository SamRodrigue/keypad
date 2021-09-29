#ifndef _KEYPAD_H_
#define _KEYPAD_H_

#include "pico/stdlib.h"

// Keypad configuration
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

#define KEYPAD_MAX_EVENT_SIZE 4

static uint32_t const key_in[KEY_IN_SIZE] = { KEY_IN_3, KEY_IN_2, KEY_IN_1, KEY_IN_0 };
static uint32_t const key_out[KEY_OUT_SIZE] = { KEY_OUT_2, KEY_OUT_1, KEY_OUT_0 };

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

struct keypad_event_queue
{
	uint8_t size;
	struct keypad_event events[KEYPAD_MAX_EVENT_SIZE];
};

void keypad_init();

void keypad_task(struct keypad_event_queue *event_queue);

bool keypad_get(uint8_t row, uint8_t col);

#endif