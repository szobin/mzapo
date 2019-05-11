#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "mzapo_lib.h"
#include "font_types.h"

int ppm_nn = 0;
uint8_t r = 40;

void delay(int ms) {
    // parlcd_delay(ms);
}

void clear_32_leds() {
    fprintf(fp, "-clear32leds\n");
}

void clear_rgb_leds() {
    fprintf(fp, "-clear_rgb_leds\n");
}

int init_mem() {
    fprintf(fp, "-init-mem\n");
    return 0;
}

int init_lcd() {
    fprintf(fp, "-init-lcd\n");
    return 0;
}

void clear_lcd() {
    fprintf(fp, "-clear-lcd\n");
}


void clear_dev() {
    clear_32_leds();	
    clear_rgb_leds();
    clear_lcd();
    fprintf(fp, "-device clear OK\n");
}

int init_dev() {
    int r;

    r = init_mem();
    if (r > 0) return r;

    r = init_lcd(); 
    if (r > 0) return r;

    clear_dev();
    return 0;
}

void set_rgb_led(int led, unsigned char r, unsigned char g, unsigned char b) {
    fprintf(fp, "--set-RGB-LED %i: %x \n", led, ((((uint32_t)r << 8 ) | g ) << 8 ) | b);
}

uint32_t read_knobs_value() {
    int c = getchar();
    fprintf(fp, "kb: %i\n", c);
    if (c == 113) return 0x7000000; // q - quit

    if (c == 114) return 0x4000000+r;  // r
    if (c == 103) return 0x2000000+r;  // g
    if (c == 98)  return 0x1000000+r;  // b

    if (c == 43)  {
        r += 5;
        return r;  // b
    }

    if (c == 45)  {
        r -= 5;
        return r;  // b
    }

    return r;
}


typedef struct {
    unsigned char r,g,b;
} grbPx;

grbPx pixels[lcd_rows][lcd_cols];

void writePPM(const char *filename)
{
    FILE *fppm;
    //open file for output
    fppm = fopen(filename, "wb");
    if (!fppm) {
        fprintf(stderr, "Unable to open file '%s'\n", filename);
        return;
    }

    //write the header file
    //image format
    fprintf(fppm, "P6\n");

    //image size
    fprintf(fppm, "%d\n%d\n", lcd_cols, lcd_rows);

    // rgb component depth
    fprintf(fppm, "%d\n",255);

    for (int r=0;r<lcd_rows;r++)
        for (int c=0;c<lcd_cols;c++) {
            int rr, gg, bb, p;
            p = lcd_map[r][c];
            bb = 255 - ((p & 0x001F) << 3); // 5+3
            gg = 255 - ((p & 0x07E0) >> 3); // 6+2
            rr = 255 - ((p & 0xF800) >> 8); // 5+3
            pixels[r][c].b = (unsigned char)(bb);
            pixels[r][c].g = (unsigned char)(gg);
            pixels[r][c].r = (unsigned char)(rr);
        }

    // pixel pixels
    fwrite(pixels, (size_t)3 * lcd_cols, (size_t)lcd_rows, fppm);
    fclose(fppm);
}


void draw_lcd() {
    ppm_nn++;
    char name[11] = "lcd_0n.ppm";
    int zero = '0';

    name[5] = (char)(ppm_nn+zero);
    writePPM(name);
    fprintf(fp, "-draw-lcd\n");
}
