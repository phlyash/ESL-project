#ifndef PWM_SYSTICK_H
#define PWM_SYSTICK_H

#include <stdint.h>
#include "nrfx_systick.h"

// for converting to SI due to herz using which is (1 / seconds)
#define MICROSECONDS_IN_SECOND 1e6

typedef struct {
    uint32_t frequency_hz;      /* pwm frequency */
    uint8_t step;               /* step for duty cycle */
    uint32_t current_duty;      /* current duty cycle */
    bool direction;             /* false - up, true - down */
    bool state;                 /* output */
    nrfx_systick_state_t time;  /* time since last call */
} pwm_systick_t;

#define INIT_PWM_SYSTICK    \
{                           \
    .frequency_hz = 1000,   \
    .step = 1,              \
    .current_duty = 1,      \
    .direction = false,     \
}


void pwm_systick_start_timer(pwm_systick_t* settings);

// this function is for polling usage
// so it should be called with high freq
bool pwm_systick_state(pwm_systick_t* settings);


#endif