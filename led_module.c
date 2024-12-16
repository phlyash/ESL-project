#include "led_module.h"
#include "pwm_module.h"
#include "button_handler.h"
#include <stdint.h>
#include <stdbool.h>

input_mode_t current_mode = NO_INPUT;
static hsv_t hsv = 
{
    .hue = 50,
    .saturation = 50,
    .value = 50,
};
static rgb_t rgb;
static int32_t led1_duty_cycle = 0;
uint32_t leds[4] = { NRF_GPIO_PIN_MAP(0, 6), NRF_GPIO_PIN_MAP(0, 8), NRF_GPIO_PIN_MAP(1, 9), NRF_GPIO_PIN_MAP(0, 12) };
uint32_t step = 0;

#define modify_function(arg, increment, max_value) \
static void modify_##arg(void) \
{ \
    static bool increase = true;\
    if (increase)\
        hsv.arg += (increment);\
    else\
        hsv.arg -= (increment);\
    if (hsv.arg >= (max_value))\
    {\
        hsv.arg = (max_value);\
        increase = false;\
    }\
    if (hsv.arg <= 0)\
    {\
        hsv.arg = 0;\
        increase = true;\
    }\
}

static void change_input_mode()
{
    current_mode = (current_mode + 1) % INPUT_MODE_AMOUNT;
    led1_duty_cycle = 0;
    switch(current_mode)
    {
        case NO_INPUT:
            step = 0;
            pwm_set_duty_cycle(PWM_LED1_CHANNEL, 0);
            return;
        case HUE_MODIFICATION:
            step = PWM_HUE_MODIFICATION_STEP;
            break;
        case SATURATION_MODIFICATION:
            step = PWM_SATURATION_MODIFICATION_STEP;
            break;
        case VALUE_MODIFICATION:
            step = 0;
            led1_duty_cycle = PWM_TOP_VALUE;
            return;
    }
}

static void modify_LED1_duty_cycle()
{
    if (led1_duty_cycle + step >= PWM_TOP_VALUE || led1_duty_cycle + step <= 0)
        step = -step;
    led1_duty_cycle += step;
    pwm_set_duty_cycle(PWM_LED1_CHANNEL, led1_duty_cycle);
}

static void hsv_to_rgb()
{
    const int sector = (hsv.hue / 60) % 6;
    const int v_min = (((hsv.value * (PWM_TOP_VALUE - hsv.saturation)) << 8) / PWM_TOP_VALUE) >> 8;
    const int a = ((((hsv.value - v_min) * (hsv.hue % 60)) << 8) / 60) >> 8;
    const int vinc = v_min + a;
    const int v_dec = hsv.value - a;

    switch (sector)
    {
        case 0: 
            rgb.red = hsv.value;
            rgb.green = vinc;
            rgb.blue = v_min;
            return;
        case 1: 
            rgb.red = v_dec;
            rgb.green = hsv.value;
            rgb.blue = v_min;
            return;
        case 2: 
            rgb.red = v_min;
            rgb.green = hsv.value;
            rgb.blue = vinc;
            return;
        case 3: 
            rgb.red = v_min;
            rgb.green = v_dec;
            rgb.blue = hsv.value;
            return;
        case 4: 
            rgb.red = vinc;
            rgb.green = v_min;
            rgb.blue = hsv.value;
            return;
        default:
            rgb.red = hsv.value;
            rgb.green = v_min;
            rgb.blue = v_dec;
            return;
    }
}

static void display_color(void)
{
    hsv_to_rgb();

    pwm_set_duty_cycle(PWM_RED_CHANNEL, rgb.red);
    pwm_set_duty_cycle(PWM_GREEN_CHANNEL, rgb.green);
    pwm_set_duty_cycle(PWM_BLUE_CHANNEL, rgb.blue);
}

// modify_hue function
modify_function(hue, HUE_STEP, 360)

// modify_saturation function
modify_function(saturation, SATURATION_STEP, 100)

// modify value function
modify_function(value, VALUE_STEP, 100)

void leds_main(void)
{
    if (is_double_click())
    {
        change_input_mode();
    }
    modify_LED1_duty_cycle();

    NRF_LOG_INFO("hsv: %d %d %d", hsv.hue, hsv.saturation, hsv.value);

    if(!is_button_holded())
    {
        display_color();
        return;
    }

    switch(current_mode)
    {
        case NO_INPUT:
            display_color();
            break;
        case HUE_MODIFICATION:
            modify_hue();
            display_color();
            break;
        case SATURATION_MODIFICATION:
            modify_saturation();
            display_color();
            break;
        case VALUE_MODIFICATION:
            modify_value();
            display_color();
            break;
    }
}

void init_leds(void)
{
    for(int i = 0; i < LEDS_NUMBER; i++) 
	{
		nrf_gpio_cfg_output(leds[i]);
		nrf_gpio_pin_write(leds[i], 1);
	}
}
