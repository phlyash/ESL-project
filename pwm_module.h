#ifndef PWM_MODULE_H
#define PWM_MODULE_H

#include <stdint.h>

#define PWM_TOP_VALUE 100

#define PWM_LED1_CHANNEL 0
#define PWM_RED_CHANNEL 1
#define PWM_GREEN_CHANNEL 2
#define PWM_BLUE_CHANNEL 3

void pwm_init(void);
void pwm_start(void);
void pwm_set_duty_cycle(uint16_t channel, uint32_t duty_cycle);

#endif