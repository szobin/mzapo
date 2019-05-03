#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

#include "mzapo_regs.h"
#include "mzapo_phys.h"
#include "mzapo_parlcd.h"
#include "font_types.h"
#include "font_rom8x16.c"

unsigned char *mem_base;

// uint16_t lcd_rows = 480;
// uint16_t lcd_cols = 320;

FILE *fp;

int init_mem() {
    mem_base = map_phys_address(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE, 0);
    if (mem_base == NULL)
        return 1;
    return 0;
}

void set_led(int led, unsigned char r, unsigned char g, unsigned char b) {
    uint32_t v = (((r << 8 ) | g ) << 8 ) | b;
    if (led == 1)
       *(volatile uint32_t*)(mem_base + SPILED_REG_LED_RGB1_o) = v;
    if (led == 2)
        *(volatile uint32_t*)(mem_base + SPILED_REG_LED_RGB2_o) = v;
}

uint32_t read_knobs_value() {
    return  *(volatile uint32_t *) (mem_base + SPILED_REG_KNOBS_8BIT_o);
}


int main(){

    if (init_mem() != 0) return(1);

    fp = fopen("log.txt", "w");

    set_led(1, 100,100,100);
    set_led(2, 100,100,100);

    uint16_t nn, s;
    uint32_t rgb_knobs_value;
    int rk, gk, bk, rb, gb, bb;

    bb = 0;
    nn = 0; s = 0;

    while(bb == 0) {
        rgb_knobs_value = read_knobs_value();

        bk =  rgb_knobs_value      & 0xFF; // blue knob position
        gk = (rgb_knobs_value>>8)  & 0xFF; // green knob position
        rk = (rgb_knobs_value>>16) & 0xFF; // red knob position
        bb = (rgb_knobs_value>>24) & 1;    // blue button
        gb = (rgb_knobs_value>>25) & 1;    // green button
        rb = (rgb_knobs_value>>26) & 1;    // red button
        usleep(250);

        fprintf(fp, "%i k RGB: %i %i %i  b: %i %i %i\n", nn, rk, gk, bk, rb, gb, bb);
        nn++;

        if ( gb > 0) {
            set_led(1, (unsigned char)rk, (unsigned char)gk, (unsigned char)bk);
            s = 1;
        } else {
            if (s == 1) {
                set_led(1, 0, 0, 0);
                s = 0;
            }
        }
    }

    set_led(1, 0, 0, 0);
    set_led(2, 0, 0, 0);

    fclose(fp);
    return 0;
}