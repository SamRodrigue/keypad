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
#define KEY_IN_0 9
#define KEY_IN_1 10
#define KEY_IN_2 11
#define KEY_IN_3 12
#define KEY_OUT_0 13
#define KEY_OUT_1 14
#define KEY_OUT_2 15

#define KEY_IN \
    1 << KEY_IN_0 | \
    1 << KEY_IN_1 | \
    1 << KEY_IN_2 | \
    1 << KEY_IN_3

#define KEY_OUT \
    1 << KEY_OUT_0 | \
    1 << KEY_OUT_1 | \
    1 << KEY_OUT_2

struct keypad_state
{
    size_t row;
    size_t col;
    bool pressed;
};

static char const keys[4][3] = {
    '1', '2', '3',
    '4', '5', '6',
    '7', '8', '9',
    '+', '0', '-'
};

static struct keypad_state keypad_state;

#define time_off() sleep_ms(1)
#define time_on() sleep_ms(1)

bool keypad_in(size_t const out, size_t *in)
{
    // printf("col %d\n", out);
    if (!gpio_get(KEY_IN_0))
        *in = 3;
    else if (!gpio_get(KEY_IN_1))
        *in = 2;
    else if (!gpio_get(KEY_IN_2))
        *in = 1;
    else if (!gpio_get(KEY_IN_3))
        *in = 0;
    else
        return false;

    // printf("row %d\n", *in);
    printf("%c\n", keys[*in][out]);
    return true;
}

void keypad_task()
{
    size_t row;
    size_t col;
    bool pressed;

    time_off();
    gpio_put(KEY_OUT_0, 0);
    time_on();
    col = 2;
    pressed = keypad_in(col, &row);
    gpio_put(KEY_OUT_0, 1);
    if (pressed)
        return;

    time_off();
    gpio_put(KEY_OUT_1, 0);
    time_on();
    col = 1;
    pressed = keypad_in(col, &row);
    gpio_put(KEY_OUT_1, 1);
    if (pressed)
        return;

    time_off();
    gpio_put(KEY_OUT_2, 0);
    time_on();
    col = 0;
    pressed = keypad_in(col, &row);
    gpio_put(KEY_OUT_2, 1);
    if (pressed)
        return;
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

    gpio_init(KEY_OUT_0);
    gpio_init(KEY_OUT_1);
    gpio_init(KEY_OUT_2);
    gpio_set_dir(KEY_OUT_0, GPIO_OUT);
    gpio_set_dir(KEY_OUT_1, GPIO_OUT);
    gpio_set_dir(KEY_OUT_2, GPIO_OUT);
    gpio_put(KEY_OUT_0, 1);
    gpio_put(KEY_OUT_1, 1);
    gpio_put(KEY_OUT_2, 1);

    puts("Hello, world!");

    while (1)
    {
        keypad_task();
    }

    return 0;
}
