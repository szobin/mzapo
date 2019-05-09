/******************************************************************* */
#ifndef MZAPO_FUNC_H
#define MZAPO_FUNC_H

#include <stdint.h>

const uint16_t clWhite = 0x0000;
const uint16_t clBlack = 0xffff;
const uint16_t clGray  = 0x8410;

const uint16_t clBlue  = 0x001f;
const uint16_t clGreen = 0x07e0;
const uint16_t clRed   = 0xf800;

const uint16_t clSky     = 0x0010;
const uint16_t clLtGreen = 0x0400;
const uint16_t clLtRed   = 0x8000;

typedef struct {
    uint16_t color_led_1;
    uint16_t color_led_2;
    uint16_t menu_screen;
    uint16_t menu_settings_leds;
} ledSettings;

ledSettings settings;

void main_cycle();

#endif /* MZAPO_FUNC_H */