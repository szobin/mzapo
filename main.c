#include <stdio.h>
#include <stdint.h>

#include "devlib/mzapo_lib.h"

void main_cycle() {
    uint16_t nn, s;
    uint32_t rgb_knobs_value;
    int rk, gk, bk, rb, gb, bb;

    nn = 0; s = 0;
    fill_map(lcd_map);
    draw_lcd(lcd_map);

    while(1) {
        rgb_knobs_value = read_knobs_value();

        bk =  rgb_knobs_value      & 0xFF; // blue knob position
        gk = (rgb_knobs_value>>8)  & 0xFF; // green knob position
        rk = (rgb_knobs_value>>16) & 0xFF; // red knob position

        bb = (rgb_knobs_value>>24) & 1;    // blue button
        gb = (rgb_knobs_value>>25) & 1;    // green button
        rb = (rgb_knobs_value>>26) & 1;    // red button

        if ((rb > 0) && (gb > 0)) {
            fprintf(fp, "quit");
            break;
        }

        if ((bb+gb+rb > 0) && (s == 0)) {
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

    fill_font_text(lcd_map, 10, 10, "TEST");

    main_cycle();

    clear_dev();

    fclose(fp);
    return 0;
}