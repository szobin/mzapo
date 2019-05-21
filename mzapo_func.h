/******************************************************************* */
#ifndef MZAPO_FUNC_H
#define MZAPO_FUNC_H

#include <stdint.h>

typedef struct {
    uint16_t color_led_1;
    uint16_t color_led_2;
    uint16_t menu_screen;
    uint16_t menu_settings_leds;
    uint8_t knob_red;
    uint8_t knob_green;
    uint8_t knob_blue;
    uint8_t view_x2;
} ledSettings;

ledSettings settings;

void main_cycle();

#endif /* MZAPO_FUNC_H */