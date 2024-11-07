#include "pwm_systick.h"
#include "nrfx_systick.h"


void pwm_systick_start_timer(pwm_systick_t* pwm_systick)
{
    nrfx_systick_get(&pwm_systick->time);
}

bool pwm_systick_state(pwm_systick_t* pwm_systick)
{
    if (!nrfx_systick_test(&pwm_systick->time, pwm_systick->state ? MICROSECONDS_IN_SECOND / pwm_systick->frequency_hz - pwm_systick->current_duty : pwm_systick->current_duty))
        return pwm_systick->state;

    pwm_systick->state = !pwm_systick->state;

    if ((pwm_systick->current_duty == MICROSECONDS_IN_SECOND / pwm_systick->frequency_hz) && (!pwm_systick->direction))
        pwm_systick->direction = true;
    if ((pwm_systick->current_duty == 0) && (pwm_systick->direction))
        pwm_systick->direction = false;

    if (!pwm_systick->state && pwm_systick->direction)
        pwm_systick->current_duty += -pwm_systick->step;
                
    if (pwm_systick->state && !pwm_systick->direction)
        pwm_systick->current_duty += pwm_systick->step;

    pwm_systick_start_timer(pwm_systick);

    return pwm_systick->state;
}