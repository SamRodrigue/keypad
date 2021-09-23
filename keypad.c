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
    1ul << KEY_IN_0 | \
    1ul << KEY_IN_1 | \
    1ul << KEY_IN_2 | \
    1ul << KEY_IN_3

#define KEY_OUT \
    1ul << KEY_OUT_0 | \
    1ul << KEY_OUT_1 | \
    1ul << KEY_OUT_2

struct keypad_state
{
    uint8_t row;
    uint8_t col;
    bool pressed;
};

static struct keypad_state keypad_state = { .pressed = false };

static char const keys[4][3] = {
    '1', '2', '3',
    '4', '5', '6',
    '7', '8', '9',
    '+', '0', '-'
};

#define time_off() sleep_ms(1)
#define time_on() sleep_ms(1)

static uint8_t columne;

void keypad_task()
{
    time_off();
    columne = 2;
    gpio_put(KEY_OUT_0, 0);
    time_on();
    gpio_put(KEY_OUT_0, 1);

    time_off();
    columne = 1;
    gpio_put(KEY_OUT_1, 0);
    time_on();
    gpio_put(KEY_OUT_1, 1);

    time_off();
    columne = 0;
    gpio_put(KEY_OUT_2, 0);
    time_on();
    gpio_put(KEY_OUT_2, 1);
}

void keypad_event(uint gpio, uint32_t events)
{
    keypad_state.col = columne;
    keypad_state.row = gpio == KEY_IN_0 ? 3 :
                       gpio == KEY_IN_1 ? 2 :
                       gpio == KEY_IN_2 ? 1 :
                       gpio == KEY_IN_3 ? 0 : 0xff;
    assert(keypad_state.row != 0xff);
    keypad_state.pressed = true;
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

    gpio_set_irq_enabled_with_callback(KEY_IN_0, GPIO_IRQ_EDGE_FALL, true, &keypad_event);
    gpio_set_irq_enabled(KEY_IN_1, GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled(KEY_IN_2, GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled(KEY_IN_3, GPIO_IRQ_EDGE_FALL, true);

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
        if (keypad_state.pressed)
        {
            keypad_state.pressed = false;
            printf("%c row: %d col: %d\n", keys[keypad_state.row][keypad_state.col], keypad_state.row, keypad_state.col);
        }
    }

    return 0;
}
