#include <stdio.h>
#include <conio.h>
#include <stdint.h>
#include <unistd.h>

#include "mzapo_regs.h"
#include "mzapo_phys.h"
#include "mzapo_parlcd.h"
#include "font_types.h"
#include "font_rom8x16.c"

unsigned char *mem_base;
unsigned char *parlcd_mem_base;

uint16_t lcd_rows = 480;
uint16_t lcd_cols = 320;

FILE *fp;

int init_mem() {
    mem_base = map_phys_address(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE, 0);
    if (mem_base == NULL)
        return 1;

    parlcd_mem_base = map_phys_address(PARLCD_REG_BASE_PHYS, PARLCD_REG_SIZE, 0);
    if (parlcd_mem_base == NULL)
        return 2;

    parlcd_hx8357_init(parlcd_mem_base);
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

void draw_lcd(uint16_t r, uint16_t c, int ch) {
    uint16_t p = r * lcd_cols + c;

    uint16_t fmap;
    uint16_t ofs = 0;
    for (int i=0; i<16; i++) {
        fmap = rom8x16_bits[ch*16+i];
        for (int j=0; j<8; j++) {
          if (fmap && 0x01 == 0) {
             *(volatile uint16_t*)(parlcd_mem_base + PARLCD_REG_CMD_o + p + j + ofs) = 0x2c;
          } else {
             *(volatile uint16_t*)(parlcd_mem_base + PARLCD_REG_CMD_o + p + j + ofs) = 0x00;
          }
          fmap = fmap << 1;
        }
        ofs += lcd_cols;
    }

}

int main(){

    if (init_mem() != 0) return(1);

    fp = fopen("log.txt", "w");

    set_led(1, 200,100,50);
    set_led(2, 100,200,50);

    int ch = '1';
    uint16_t r, c;
    uint32_t rgb_knobs_value;
    int rk, gk, bk, rb, gb, bb;

    r = 0; c = 0;

    while(ch != 'q') {
        rgb_knobs_value = read_knobs_value();

        bk =  rgb_knobs_value      & 0xFF; // blue knob position
        gk = (rgb_knobs_value>>8)  & 0xFF; // green knob position
        rk = (rgb_knobs_value>>16) & 0xFF; // red knob position
        bb = (rgb_knobs_value>>24) & 1;    // blue button
        gb = (rgb_knobs_value>>25) & 1;    // green button
        rb = (rgb_knobs_value>>26) & 1;    // red button
        usleep(100);

        fprintf(fp, "k RGB: %i %i %i  b: %i %i %i\n", rk, gk, bk, rb, gb, bb);

        if(kbhit()) {
            ch = getch();
            draw_lcd(r, c, ch);
            c += 1;
        }
    }

    set_led(1, 0,0,0);
    set_led(2, 0,0,0);
    fclose(fp);

    return 0;
}