#include "button_handler.h"

uint32_t button_pin = NRF_GPIO_PIN_MAP(1, 6);	// may be externed or something 

APP_TIMER_DEF(debounce_timer_id);
APP_TIMER_DEF(double_click_timer_id);

interrupt_timer_t debounce_timer;
interrupt_timer_t double_click_timer;

bool debounce_passed = false;
bool first_click_passed = false;
bool is_active = false;
bool double_click = false;
bool is_hold = false;

bool is_button_pressed(void)
{
    return !nrf_gpio_pin_read(button_pin);
}

bool is_button_holded(void)
{
	return is_button_pressed() && is_hold;
}

bool is_double_click(void)
{
	if (double_click)
	{
		double_click = false;
		return true;
	}
	return false;
}

static void init_timer(interrupt_timer_t* timer, app_timer_timeout_handler_t handler, app_timer_id_t timer_id, uint32_t timeout_ms)
{
	timer->timer = timer_id;
	app_timer_create(&timer->timer, APP_TIMER_MODE_SINGLE_SHOT, handler);
	timer->is_working = false;
	timer->timeout_ticks = APP_TIMER_TICKS(timeout_ms);
}

static void start_timer(interrupt_timer_t* timer, void* context)
{
	timer->is_working = true;
	app_timer_start(timer->timer, timer->timeout_ticks, context);
}

static void stop_timer(interrupt_timer_t* timer)
{
	timer->is_working = false;
	app_timer_stop(timer->timer);
}

static void debounce_handler(void* context)
{
	bool* context_bool = (bool*) context;

	if (is_button_pressed())
		*context_bool = !*context_bool;

	if ((context == &is_active) && is_button_pressed())
	{
		first_click_passed = false;
		double_click = true;
		stop_timer(&double_click_timer);
	}

	debounce_timer.is_working = false;
}

static void double_click_handler(void* context)
{
	first_click_passed = false;
	is_hold = true;

	double_click_timer.is_working = false;
}

static void btn_IRQHandler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
	is_hold = false;
	if (debounce_timer.is_working) 
		stop_timer(&debounce_timer);

	if (first_click_passed) 
		start_timer(&debounce_timer, &is_active);
	else 
	{
		start_timer(&debounce_timer, &first_click_passed);
		start_timer(&double_click_timer, NULL);
	}
}

void init_handlers(void)
{
	nrfx_gpiote_init();

	nrfx_gpiote_in_config_t btn_cfg = NRFX_GPIOTE_CONFIG_IN_SENSE_TOGGLE(false);
	btn_cfg.pull = NRF_GPIO_PIN_PULLUP;
	nrfx_gpiote_in_init(button_pin, &btn_cfg, btn_IRQHandler);

	nrfx_gpiote_in_event_enable(button_pin, true);

	init_timer(&debounce_timer, debounce_handler, debounce_timer_id, 50);
    init_timer(&double_click_timer, double_click_handler, double_click_timer_id, 1000);
}
