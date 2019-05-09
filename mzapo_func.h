/******************************************************************* */
#ifndef MZAPO_FUNC_H
#define MZAPO_FUNC_H

#include <stdint.h>

typedef struct {
    uint16_t color_led_1;
    uint16_t color_led_2;
    uint16_t menu_screen;
    uint16_t menu_settings_leds;
} ledSettings;

ledSettings settings;

void main_cycle();

#endif /* MZAPO_FUNC_H */