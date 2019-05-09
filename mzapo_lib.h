/******************************************************************* */
#ifndef MZAPO_LIB_H
#define MZAPO_LIB_H

unsigned char *mem_base;
unsigned char *parlcd_mem_base;

FILE *fp;

#define lcd_cols 480
#define lcd_rows 320

uint16_t lcd_map[lcd_rows][lcd_cols];

void delay(int ms);

int init_dev();
void clear_dev();

void clear_32_leds();
void clear_rgb_leds();

int init_mem();

void clear_lcd();

void set_rgb_led(int led, unsigned char r, unsigned char g, unsigned char b);

uint32_t read_knobs_value();

void draw_lcd();

#endif /* MZAPO_LIB_H */
