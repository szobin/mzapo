#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

#include "mzapo_regs.h"
#include "mzapo_phys.h"
#include "mzapo_parlcd.h"
#include "font_types.h"
#include "font_rom8x16.c"

unsigned char *mem_base;
unsigned char *parlcd_mem_base;

// uint16_t lcd_rows = 480;
uint16_t lcd_cols = 320;

FILE *fp;

void clear_32_leds() {
  *(volatile uint32_t*)(mem_base + SPILED_REG_LED_LINE_o) = 0x00000000;
}

void clear_rgb_leds() {
   *(volatile uint32_t*)(mem_base + SPILED_REG_LED_RGB1_o) = 0x00000000;
   *(volatile uint32_t*)(mem_base + SPILED_REG_LED_RGB2_o) = 0x00000000;
}

int init_mem() {
    mem_base = map_phys_address(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE, 0);
    if (mem_base == NULL)
        return 1;
    return 0;
}

int init_lcd() {
    parlcd_mem_base = map_phys_address(PARLCD_REG_BASE_PHYS, PARLCD_REG_SIZE, 0);
    if (parlcd_mem_base == NULL)
        return 2;

    parlcd_hx8357_init(parlcd_mem_base);
    return 0;
}

void clear_dev() {
    clear_32_leds();	
    clear_rgb_leds();
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
    uint32_t v = ((((uint32_t)r << 8 ) | g ) << 8 ) | b;
    if (led == 1)
       *(volatile uint32_t*)(mem_base + SPILED_REG_LED_RGB1_o) = v;
    if (led == 2)
       *(volatile uint32_t*)(mem_base + SPILED_REG_LED_RGB2_o) = v;
}

uint32_t read_knobs_value() {
    return  *(volatile uint32_t *) (mem_base + SPILED_REG_KNOBS_8BIT_o);
}

void draw_lcd(uint16_t r, uint16_t c, int ch) {
    uint16_t p = r * lcd_cols + c;

    uint16_t fmap;
    uint16_t ofs = 0;
    for (int i=0; i<16; i++) {
        fmap = rom8x16_bits[ch*16+i];
        for (int j=0; j<8; j++) {
          if ((fmap & 0x01) == 0) {
             *(volatile uint16_t*)(parlcd_mem_base + PARLCD_REG_CMD_o + p + j + ofs) = 0x2c;
          } else {
             *(volatile uint16_t*)(parlcd_mem_base + PARLCD_REG_CMD_o + p + j + ofs) = 0x00;
          }
          fmap = fmap << 1;
        }
        ofs += lcd_cols;
    }

}


void main_cycle() {
    uint16_t nn, s;
    uint32_t rgb_knobs_value;
    int rk, gk, bk, rb, gb, bb, quit;

    quit = 0;
    nn = 0; s = 0;

    while(quit == 0) {
        rgb_knobs_value = read_knobs_value();

        bk =  rgb_knobs_value      & 0xFF; // blue knob position
        gk = (rgb_knobs_value>>8)  & 0xFF; // green knob position
        rk = (rgb_knobs_value>>16) & 0xFF; // red knob position

        bb = (rgb_knobs_value>>24) & 1;    // blue button
        gb = (rgb_knobs_value>>25) & 1;    // green button
        rb = (rgb_knobs_value>>26) & 1;    // red button

        if ((rb > 0) && (gb > 0)) {
          draw_lcd(0, 0, 'Q');
          quit = 1;
          fprintf(fp, "quit");
        }

        if (bb+gb+rb > 0) {
           nn++;

           if ( bb > 0) {
               set_rgb_led(1, 0, 0, (unsigned char)bk);
               fprintf(fp, "%i k RGB: %i %i %i  b: %i %i %i\n", nn, rk, gk, bk, rb, gb, bb);
               s = 1;
           }

           if ( gb > 0) {
               set_rgb_led(1, 0, (unsigned char)gk, 0);
               fprintf(fp, "%i k RGB: %i %i %i  b: %i %i %i\n", nn, rk, gk, bk, rb, gb, bb);
               s = 1;
           }

           if ( rb > 0) {
               set_rgb_led(1, (unsigned char)rk, 0, 0);
               fprintf(fp, "%i k RGB: %i %i %i  b: %i %i %i\n", nn, rk, gk, bk, rb, gb, bb);
               s = 1;
           }

        } else {

            if (s == 1) {
                clear_rgb_leds();
                s = 0;
            }

        }
    }
}


int main(){
    fp = fopen("log.txt", "w");
    fprintf(fp, "mzAPO log started\n");
    if (init_dev() != 0) {
      fprintf(fp, "-error: device init failed\n");
      return(1);
    }
    fprintf(fp, "-device init OK\n");

    main_cycle();

    clear_dev();

    fclose(fp);
    return 0;
}