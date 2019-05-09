#include <stdio.h>
#include <stdint.h>

#include "mzapo_lib.h"
#include "mzapo_regs.h"
#include "mzapo_phys.h"
#include "mzapo_parlcd.h"


void delay(int ms) {
    parlcd_delay(ms);
}

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

void clear_lcd() {
  parlcd_write_cmd(parlcd_mem_base, 0x2c);
  for (int j=0; j<lcd_rows; j++) {
    for (int i=0; i<lcd_cols; i++) {
      parlcd_write_data(parlcd_mem_base, 0x00);
    }  
  }
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
    uint32_t v = ((((uint32_t)r << 8 ) | g ) << 8 ) | b;
    if (led == 1)
       *(volatile uint32_t*)(mem_base + SPILED_REG_LED_RGB1_o) = v;
    if (led == 2)
       *(volatile uint32_t*)(mem_base + SPILED_REG_LED_RGB2_o) = v;
}

uint32_t read_knobs_value() {
    return  *(volatile uint32_t *) (mem_base + SPILED_REG_KNOBS_8BIT_o);
}

void draw_lcd() {
    parlcd_write_cmd(parlcd_mem_base, 0x2c);
    for (int j=0; j<lcd_cols; j++) {
      for (int i=0; i<lcd_rows; i++) {
           parlcd_write_data(parlcd_mem_base, lcd_map[i][j]);
        }
    }

}
