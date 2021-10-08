// SSD1306 https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf

#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include <stdlib.h>
#include <string.h>

#define I2C_PORT i2c0
#define I2C_SDA_PIN 4
#define I2C_SCL_PIN 5

#define DISPLAY_ADDR 0x3c
#define DISPLAY_PAGES 8
#define DISPLAY_COMS 128
#define DISPLAY_BUFFER (DISPLAY_PAGES * DISPLAY_COMS)

static uint8_t display_init_sequence[] = {
	0x00,
	0xae,       // [F] Set display enable: [0] 0 off 1 on
	0xd5, 0x80, // [T] Set display clock divide: [3:0] divide ratio (+1). [7:4] oscillator frequency
	0xa8, 0x3f, // [H] Set multiplex ratio: [5:0] 16-64 MUX (+1), 0-14 is invalid
	0xd3, 0x00, // [H] Set display offset: [5:0] vertical shift by COM
	0x40,       // [H] Set display start line: [5:0] RAM display start line
	0x8d, 0x14, // [C] Set charge pump: [2] 0 disable charge pump 1 enable
	0x20, 0x01, // Set memory addressing mode: [1:0] 00 horizontal 01 vertical 10 page 11 invalid
	0xa1,       // [H] Set segment re-map, column address to map to SEG0: a0 = 0, a1 = 127
	0xc0,       // [H] Set COM output scan direction: [3] 0 normal COM0-COM[N-1] 1 reverse
	0xda, 0x12, // [H] Set COM pins order: [4] 0 sequential 1 alternative [5] 0 disable left/right 1 enable left/right
	0x81, 0xcf, // [F] Set contract control: [7:0] 1 to 256 contrast value (+1)
	0xd9, 0xf1, // [T] Set pre-charge period: [3:0] phase 1 period, [7:4] phase 2 period
	0xd8, 0x20, // ?
	0xa4,       // [F] Set display output according to GDDRAM: [0] 0 enable 1 ignore
	0xa6,       // [F] Set display iversion: [0] 0 normal (active high) 1 inverse (active low)
	0x2e,       // [S] Stop scrolling
	0xaf        // [F] Set display enable: [0] 0 off 1 on
};

static uint8_t display_pager[] = {
	0x00,          // non-continuous/command
	// set coumn address: start address, end address
	0x21, 0x00, DISPLAY_COMS - 1,
	// set page address: start address, end address
	0x22, 0x00, DISPLAY_PAGES - 1
};

struct __attribute__((__packed__)) display_buffer {
	uint8_t header;
	uint8_t data[DISPLAY_COMS][DISPLAY_PAGES];
};

struct display_buffer display_buffer;


inline void display()
{
	i2c_write_blocking(I2C_PORT, DISPLAY_ADDR, (uint8_t*)&display_buffer, sizeof(display_buffer), false);
}


void display_init()
{
	i2c_init(I2C_PORT, 400 * 1000);
	gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
	gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
	gpio_pull_up(I2C_SDA_PIN);
	gpio_pull_up(I2C_SCL_PIN);

	display_buffer.header = 0x40; // Set display start line register from 0-63 [5:0]
	memset(display_buffer.data, 0, DISPLAY_BUFFER);

	i2c_write_blocking(I2C_PORT, DISPLAY_ADDR, display_init_sequence, sizeof(display_init_sequence), false);

	i2c_write_blocking(I2C_PORT, DISPLAY_ADDR, display_pager, sizeof(display_pager), false);

	// uint8_t i = 0;
	// while(1)
	// {
	// 	for (uint8_t c = 0; c < DISPLAY_COMS; ++c)
	// 	{
	// 		for (uint8_t p = 0; p < DISPLAY_PAGES; ++p)
	// 		{
	// 			display_buffer.data[c][p] = c + i;
	// 		}
	// 	}

	// 	display();
	// 	i++;
	// }
}

#endif