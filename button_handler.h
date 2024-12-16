#ifndef BUTTON_HANDLER_H
#define BUTTON_HANDLER_H


#include "nrfx_systick.h"
#include "app_timer.h"
#include "nrfx_gpiote.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include <stdbool.h>

typedef struct {
    app_timer_id_t timer;
    bool is_working;
	uint32_t timeout_ticks;
} interrupt_timer_t;

bool is_button_pressed(void);
bool is_button_holded(void);
bool is_double_click(void);
void init_handlers(void);

#endif