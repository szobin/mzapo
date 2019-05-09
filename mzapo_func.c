#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "font_rom8x16.c"
#include "mzapo_lib.h"
#include "mzapo_func.h"

const uint16_t clWhite = 0x0000;
const uint16_t clBlack = 0xffff;
const uint16_t clGray  = 0x8410;

const uint16_t clBlue  = 0x001f;
const uint16_t clGreen = 0x07e0;
const uint16_t clRed   = 0xf800;

const uint16_t clSky     = 0x0010;
const uint16_t clLtGreen = 0x0400;
const uint16_t clLtRed   = 0x8000;

const uint16_t font_height = 16;
const uint16_t font_width = 8;

unsigned char get_led_red(uint16_t color) {
    return (unsigned char)((color & 0xF800) >> 8);
}

unsigned char get_led_green(uint16_t color) {
    return (unsigned char)((color & 0x07E0) >> 3);
}

unsigned char get_led_blue(uint16_t color) {
    return (unsigned char)((color & 0x001F) << 3);
}

uint16_t get_led_color(int r, int g, int b) {
    uint16_t color = 0;

    color |= (r >> 3) << 11;
    color |= (g >> 3) << 6;
    color |= (b >> 3);

    return color;
}


void fill_map(uint16_t bk_color) {
    for (int i=0; i<lcd_rows; i++) {
        for (int j=0; j<lcd_cols; j++) {
            lcd_map[i][j] = bk_color;
        }
    }
}

void fill_rect_map(int x, int y, int w, int h, uint16_t color) {
    for (int i=y; i<y+h; i++) {
        for (int j=x; j<x+w; j++) {
            lcd_map[i][j] = (uint16_t)color;
        }
    }

}

void fill_test_map() {

    uint16_t test_colors[8];
    test_colors[0] = clWhite;
    test_colors[1] = clRed;
    test_colors[2] = clGreen;
    test_colors[3] = clBlue;
    test_colors[4] = clBlack;
    test_colors[5] = clLtRed;
    test_colors[6] = clLtGreen;
    test_colors[7] = clSky;

    for (int i=0; i<lcd_rows / 40; i++) {
        for (int j=0; j<lcd_cols / 40; j++) {
            fill_rect_map(j*40, i*40, 40, 40, test_colors[(i+j) % 8]);
        }
    }
}

void fill_font_text(int x, int y, char *text, uint16_t font_color) {
    size_t l = strlen(text);
    int ch;
    uint16_t m, fm;
    for (int k=0; k<l; k++) {
        ch = text[k];
        for (int r = 0; r < font_height; r++) {
            m = rom8x16_bits[r+ch*font_height];
            fm = 0x8000;
            if (m != 0)
                for (int c = 0; c < font_width; c++) {
                    if ((m & fm) > 0 ) {
                        lcd_map[y+r][x+c+k*font_width] = font_color;
                    }
                    fm >>= 01;
                }

        }
    }
}

void draw_lcd_map() {
    fill_map(clGray);
    fill_rect_map(0, 0, 320, 32, clWhite);
    fill_font_text(80, 10, "LED 1", clBlack);
    fill_font_text(180, 10, "LED 2", clBlack);

    fill_font_text(10, 40, "mode", clBlack);
    fill_font_text(74, 40, "static", clWhite);
    fill_font_text(174, 40, "static", clWhite);

    fill_font_text(11, 71, "color", clBlack);

    fill_rect_map(83, 68, 24,24, clWhite);
    fill_rect_map(85, 70, 20,20, settings.color_led_1);

    fill_rect_map(183, 68, 24,24, clWhite);
    fill_rect_map(185, 70, 20,20, settings.color_led_2);


    fill_rect_map(320, 32, 160, 320, clBlack);
    fill_rect_map(320, 0, 160, 32, clGray);

    fill_font_text(330, 8, "MENU", clBlack);

    switch (settings.menu_screen) {
        case 0:
            fill_rect_map(330, 44, 8, 8, clBlue);
            fill_font_text(342, 40, "settings", clWhite);

            fill_rect_map(330, 64, 8, 8, clRed);
            fill_font_text(342, 60, "quit", clWhite);
            break;
        case 1:
            fill_rect_map(330, 44, 8, 8, clBlue);
            fill_font_text(342, 40, "set", clWhite);

            fill_rect_map(330, 64, 8, 8, clGreen);
            switch (settings.menu_settings_leds) {
                case 0:
                    fill_font_text(342, 60, "mode: both", clWhite);
                    break;
                case 1:
                    fill_font_text(342, 60, "mode: LED1", clWhite);
                    break;
                case 2:
                    fill_font_text(342, 60, "mode: LED2", clWhite);
                    break;
                default:
                    break;
            }

            fill_rect_map(330, 84, 8, 8, clRed);
            fill_font_text(342, 80, "back", clWhite);
            break;

        default:
            ;
    }
    draw_lcd();
}

void draw_start_lcd_map() {
    // fill_map(clBlue);

    fill_test_map();
    draw_lcd();
    delay(500);

    fill_map(clSky);
    fill_font_text(150, 140, "LOADING...", clRed);
    draw_lcd();
    delay(500);

    draw_lcd_map();

    //fill_font_text(10, 10, "TEST SCREEN", clBlack);
    //fill_font_text(20, 40, "<<RED>>", clRed);
    //fill_font_text(20, 60, "<<GREEN>>", clGreen);
    //fill_font_text(20, 80, "<<BLUE>>", clBlue);
}

void draw_stop_lcd_map() {
    fill_map(clSky);
    fill_font_text(150, 140, "...FINISH", clRed);
    draw_lcd();
}


void set_led_settings() {
    set_rgb_led(1,
            get_led_red(settings.color_led_1),
            get_led_green(settings.color_led_1),
            get_led_blue(settings.color_led_1));
    set_rgb_led(2,
            get_led_red(settings.color_led_2),
            get_led_green(settings.color_led_2),
            get_led_blue(settings.color_led_2));
}

void main_cycle() {
    uint32_t rgb_knobs_value;
    int quit = 1;
    int s = 0;
    int rk, gk, bk, rb, gb, bb, pbb, pgb, prb;

    rb = 0; bb = 0; gb = 0;
    rk = 0; gk = 0; bk = 0;

    settings.color_led_1 = clGreen;
    settings.color_led_2 = clRed;
    settings.menu_screen = 0;
    settings.menu_settings_leds = 0; // both

    draw_start_lcd_map();
    set_led_settings();

    while(quit) {
        rgb_knobs_value = read_knobs_value();

        pbb = (rgb_knobs_value>>24) & 1;    // blue button
        pgb = (rgb_knobs_value>>25) & 1;    // green button
        prb = (rgb_knobs_value>>26) & 1;    // red button
        if (pbb + pgb + prb + s == 0) continue;

        if (s == 0) {
            s = 1;
            bb = pbb;
            gb = pgb;
            rb = prb;

            bk =  rgb_knobs_value      & 0xFF; // blue knob position
            gk = (rgb_knobs_value>>8)  & 0xFF; // green knob position
            rk = (rgb_knobs_value>>16) & 0xFF; // red knob position

            continue;
        }

        if (pbb + pgb + prb > 0) continue; // wait while pressed

        s = 0; // btn released

        switch (settings.menu_screen) {
            case 0:
                if (rb > 0) { // menu: quit
                    fprintf(fp, "menu: quit\n");
                    quit = 0;
                    break;
                }
                if (bb > 0) { // menu: settings
                    fprintf(fp, "menu: settings\n");
                    settings.menu_screen = 1;
                    draw_lcd_map();
                    break;
                }
            case 1:
                if (rb > 0) { // menu: settings/back
                    fprintf(fp, "menu: settings/back\n");
                    settings.menu_screen = 0;
                    draw_lcd_map();
                    break;
                }

                if (gb > 0) { // menu: settings/mode
                    fprintf(fp, "menu: settings/mode\n");
                    settings.menu_settings_leds = (uint16_t)((settings.menu_settings_leds + 1) % 3);
                    draw_lcd_map();
                    break;
                }

                if (bb > 0) { // menu: settings/set
                    fprintf(fp, "menu: settings/set\n");
                    switch (settings.menu_settings_leds) {
                        case 0:
                            settings.color_led_1 = get_led_color(rk, gk, bk);
                            settings.color_led_2 = get_led_color(rk, gk, bk);
                            draw_lcd_map();
                            set_led_settings();
                            break;
                        case 1:
                            settings.color_led_1 = get_led_color(rk, gk, bk);
                            draw_lcd_map();
                            set_led_settings();
                            break;
                        case 2:
                            settings.color_led_2 = get_led_color(rk, gk, bk);
                            draw_lcd_map();
                            set_led_settings();
                            break;
                        default:
                            break;
                    }
                    break;
                }


            default:
                break;
        }

    }

    draw_stop_lcd_map();
    clear_rgb_leds();

}
