#ifndef LED_MODULE_H
#define LED_MODULE_H

#include "button_handler.h"

#define LEDS_NUMBER 4
#define GPIO_LED_TURN_OFF 1
#define GPIO_LED_TURN_ON 0

// pins could be extern
uint32_t leds[4];

typedef enum {
    NO_INPUT,
    HUE_MODIFICATION,
    SATURATION_MODIFICATION,
    VALUE_MODIFICATION,
} input_mode_t;
#define INPUT_MODE_AMOUNT 4
#define PWM_HUE_MODIFICATION_STEP 1
#define PWM_SATURATION_MODIFICATION_STEP PWM_HUE_MODIFICATION_STEP * 10

typedef struct {
    uint32_t hue;
    uint32_t saturation;
    uint32_t value;
} hsv_t;
#define HUE_STEP 1
#define SATURATION_STEP 1
#define VALUE_STEP 1

typedef struct {
    uint32_t red;
    uint32_t green;
    uint32_t blue;
} rgb_t;

void rgb_to_hsv(rgb_t* rgb, hsv_t* hsv);
void set_hsv(hsv_t* pHsv);
hsv_t* get_current_color(void);
void leds_main(void);
void init_leds(void);

#endif