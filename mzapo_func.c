#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "hsv_rgb.h"
#include "font_rom8x16.c"
#include "mzapo_lib.h"
#include "mzapo_func.h"

const uint16_t clWhite = 0x0000;
const uint16_t clBlack = 0xffff;
const uint16_t clGray  = 0x8410;

const uint16_t clBlue  = 0xff80; // 0x001f;
const uint16_t clGreen = 0xf81f; // 0x07e0;
const uint16_t clRed   = 0x07ff; // 0xf800;

const uint16_t clSky     = 0xff8f; // 0x0010;
const uint16_t clDkGreen = 0xf41f;// 0x0400;
const uint16_t clDkRed   = 0x87ff; // 0x8000;

const uint16_t font_height = 16;
const uint16_t font_width = 8;

#define id_menu_screen_main 0
#define id_menu_screen_settings 1
#define id_menu_screen_set_led_color 2


unsigned char get_led_red(uint16_t color) {
    return (unsigned char)(255 - ((color & 0xF800) >> 8));
}

unsigned char get_led_green(uint16_t color) {
    return (unsigned char)(255 - ((color & 0x07E0) >> 3));
}

unsigned char get_led_blue(uint16_t color) {
    return (unsigned char)(255 - ((color & 0x001F) << 3));
}

uint16_t get_led_color(int r, int g, int b) {
    uint16_t color = 0;

    color |= ((255-r) >> 3) << 11;
    color |= ((255-g) >> 3) << 6;
    color |= ((255-b) >> 3);

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
    test_colors[5] = clDkRed;
    test_colors[6] = clDkGreen;
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

char* get_mode_value() {
    switch (settings.menu_settings_leds) {
        case 0:
            return "mode: both";
        case 1:
            return "mode: LED1";
        case 2:
            return "mode: LED2";
        default:
            return "mode: n/a";
    }
}

void draw_lcd_map() {
    char num_str[20];

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


    switch (settings.menu_screen) {
        case id_menu_screen_main:
            fill_font_text(330, 8, "MAIN MENU", clBlack);

            fill_rect_map(330, 44, 8, 8, clBlue);
            fill_font_text(342, 40, "settings", clWhite);

            fill_rect_map(330, 64, 8, 8, clRed);
            fill_font_text(342, 60, "quit", clWhite);
            break;
        case id_menu_screen_settings:
            fill_font_text(330, 8, "SETTINGS", clBlack);

            fill_rect_map(330, 44, 8, 8, clBlue);
            fill_font_text(342, 40, "set color", clWhite);

            fill_rect_map(330, 64, 8, 8, clGreen);
            fill_font_text(342, 60, get_mode_value(), clWhite);

            fill_rect_map(330, 84, 8, 8, clRed);
            fill_font_text(342, 80, "back", clWhite);
            break;
        case id_menu_screen_set_led_color:
            fill_font_text(330, 8, "SET LED COLOR", clBlack);

            fill_rect_map(330, 44, 8, 8, clRed);
            fill_font_text(342, 40, "back", clWhite);

            fill_font_text(342, 60, get_mode_value(), clWhite);

            sprintf(num_str, "%d", settings.knob_red);
            fill_rect_map(330, 94, 8, 8, clRed);
            fill_rect_map(333, 92, 2, 12, clRed);
            fill_font_text(342, 90, "hue", clWhite);
            fill_font_text(380, 90, num_str, clWhite);

            sprintf(num_str, "%d", settings.knob_green);
            fill_rect_map(330, 114, 8, 8, clGreen);
            fill_rect_map(333, 112, 2, 12, clGreen);
            fill_font_text(342, 110, "sat", clWhite);
            fill_font_text(380, 110, num_str, clWhite);

            sprintf(num_str, "%d", settings.knob_blue);
            fill_rect_map(330, 134, 8, 8, clBlue);
            fill_rect_map(333, 132, 2, 12, clBlue);
            fill_font_text(342, 130, "val", clWhite);
            fill_font_text(380, 130, num_str, clWhite);

            struct HSV hsv = {settings.knob_red, settings.knob_green, settings.knob_blue};
            struct RGB rgb = HSVToRGB(hsv);
            sprintf(num_str, "RGB: %d %d %d", rgb.R, rgb.G, rgb.B);
            fill_font_text(342, 160, num_str, clWhite);

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

void set_rgb_led_logged(int led, unsigned char r, unsigned char g, unsigned char b) {
   fprintf(fp, "-set-RGB-LED %i: %i %i %i \n", led, r, g, b);

   set_rgb_led(led, r, g, b);
}

void set_led_settings() {
    set_rgb_led_logged(1,
            get_led_red(settings.color_led_1),
            get_led_green(settings.color_led_1),
            get_led_blue(settings.color_led_1));
    set_rgb_led_logged(2,
            get_led_red(settings.color_led_2),
            get_led_green(settings.color_led_2),
            get_led_blue(settings.color_led_2));
}

void update_led_color_settings(int h, int s, int v) {
    settings.knob_red = (uint8_t)h;
    settings.knob_green = (uint8_t)s;
    settings.knob_blue = (uint8_t)v;

    struct HSV hsv;
    hsv.H = h; hsv.S = s; hsv.V = v;
    struct RGB rgb = HSVToRGB(hsv);

    switch (settings.menu_settings_leds) {
        case 0:
            settings.color_led_1 = get_led_color(rgb.R, rgb.G, rgb.B);
            settings.color_led_2 = get_led_color(rgb.R, rgb.G, rgb.B);
            draw_lcd_map();
            set_led_settings();
            break;
        case 1:
            settings.color_led_1 = get_led_color(rgb.R, rgb.G, rgb.B);
            draw_lcd_map();
            set_led_settings();
            break;
        case 2:
            settings.color_led_2 = get_led_color(rgb.R, rgb.G, rgb.B);
            draw_lcd_map();
            set_led_settings();
            break;
        default:
            break;
    }

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

        if (settings.menu_screen == id_menu_screen_set_led_color) {
            bk =  rgb_knobs_value      & 0xFF; // blue knob position
            gk = (rgb_knobs_value>>8)  & 0xFF; // green knob position
            rk = (rgb_knobs_value>>16) & 0xFF; // red knob position

            if ((bk != settings.knob_blue) ||
                (gk != settings.knob_green) ||
                (rk != settings.knob_red)) {
                update_led_color_settings(rk, gk, bk);
            }
        }


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
            case id_menu_screen_main:
                if (rb > 0) { // menu: quit
                    fprintf(fp, "menu: quit\n");
                    quit = 0;
                    break;
                }
                if (bb > 0) { // menu: settings
                    fprintf(fp, "menu: settings\n");
                    settings.menu_screen = id_menu_screen_settings;
                    draw_lcd_map();
                    break;
                }
            case id_menu_screen_settings:
                if (rb > 0) { // menu: settings/back
                    fprintf(fp, "menu: settings/back\n");
                    settings.menu_screen = id_menu_screen_main;
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
                    settings.menu_screen = id_menu_screen_set_led_color;
                    update_led_color_settings(rk, gk, bk);
                    break;
                }

            case id_menu_screen_set_led_color:
                if (rb > 0) { // menu: settings/set/back
                    fprintf(fp, "menu: settings/set/back\n");
                    settings.menu_screen = id_menu_screen_settings;
                    draw_lcd_map();
                    break;
                }

            default:
                break;
        }

    }

    draw_stop_lcd_map();
    clear_rgb_leds();
}
