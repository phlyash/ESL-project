#include <stdbool.h>
#include <stdint.h>

#include "nordic_common.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "app_timer.h"

#include "nrf_log_backend_usb.h"

#include "app_usbd.h"
#include "app_usbd_serial_num.h"

#include "nrfx_gpiote.h"
#include "nrf_delay.h"
#include "nrfx_systick.h"

#include "pwm_systick.h"


#define GPIO_LED_TURN_OFF 1
#define GPIO_LED_TURN_ON 0
#define LEDS_NUMBER 4

typedef struct {
    app_timer_id_t timer;
    bool is_working;
	uint32_t timeout_ticks;
} itq_timer_t;
	
uint8_t DEVICE_ID[] = { 6, 5, 8, 0 };
uint8_t active_led = 0;
uint8_t blinks_passed = 0;
bool debounce_passed = false;
bool first_click_passed = false;
bool is_active = false;
APP_TIMER_DEF(debounce_timer_id);
APP_TIMER_DEF(double_click_timer_id);
itq_timer_t debounce_timer;
itq_timer_t double_click_timer;

uint32_t button_pin = NRF_GPIO_PIN_MAP(1, 6);
uint32_t leds[] = { NRF_GPIO_PIN_MAP(0, 6), NRF_GPIO_PIN_MAP(0, 8), NRF_GPIO_PIN_MAP(1, 9), NRF_GPIO_PIN_MAP(0, 12) };

void turn_off_led(uint8_t led_idx) 
{
	nrf_gpio_pin_write(leds[led_idx], GPIO_LED_TURN_OFF);
}

void turn_on_led(uint8_t led_idx) 
{
	nrf_gpio_pin_write(leds[led_idx], GPIO_LED_TURN_ON);
}

void start_timer(itq_timer_t* timer, void* context)
{
	timer->is_working = true;
	app_timer_start(timer->timer, timer->timeout_ticks, context);
}

void stop_timer(itq_timer_t* timer)
{
	timer->is_working = false;
	app_timer_stop(timer->timer);
}

// infinite loop if device id is { 0, 0, 0, 0 }
// think about it better
void active_led_switch(void)
{
	if (blinks_passed >= DEVICE_ID[active_led]) 
	{
		turn_off_led(active_led);
		blinks_passed = 0;
		active_led = (active_led + 1) % LEDS_NUMBER;

		active_led_switch();
	}
}

void btn_IRQHandler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
	NRF_LOG_INFO("btn_IRQHandler is_active: %d, first_click: %d", is_active, first_click_passed);
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

void init_timer(itq_timer_t* timer, app_timer_timeout_handler_t handler, app_timer_id_t timer_id, uint32_t timeout_ms)
{
	timer->timer = timer_id;
	app_timer_create(&timer->timer, APP_TIMER_MODE_SINGLE_SHOT, handler);
	timer->is_working = false;
	timer->timeout_ticks = APP_TIMER_TICKS(timeout_ms);
}

void init_gpio(void)
{
	nrfx_gpiote_init();

	nrfx_gpiote_in_config_t btn_cfg = NRFX_GPIOTE_CONFIG_IN_SENSE_TOGGLE(false);
	btn_cfg.pull = NRF_GPIO_PIN_PULLUP;
	nrfx_gpiote_in_init(button_pin, &btn_cfg, btn_IRQHandler);

	nrfx_gpiote_in_event_enable(button_pin, true);

	for(int i = 0; i < LEDS_NUMBER; i++) 
	{
		nrf_gpio_cfg_output(leds[i]);
		nrf_gpio_pin_write(leds[i], 1);
	}
}

void logs_init(void)
{
    ret_code_t ret = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(ret);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}

void periph_init(void)
{
	init_gpio();
	nrfx_systick_init();
	app_timer_init();
	logs_init();
}

void debounce_handler(void* context)
{
	bool press = nrf_gpio_pin_read(button_pin);
	NRF_LOG_INFO("debounce_handler enter, is_active: %d, first: %d, pressed: %d", is_active, first_click_passed, press);
	NRF_LOG_INFO("context ptr: %p is_active ptr: %p click ptr: %p", context, &is_active, &first_click_passed);

	bool* context_bool = (bool*) context;

	if (nrf_gpio_pin_read(button_pin))
		*context_bool = !*context_bool;
	if (context == &is_active && press)
	{
		first_click_passed = false;
		stop_timer(&double_click_timer);
	}

	NRF_LOG_INFO("debounce_handler out, is_active: %d, first: %d", is_active, first_click_passed);

	debounce_timer.is_working = false;
}

void double_click_handler(void* context)
{
	NRF_LOG_INFO("double_click_handler");

	first_click_passed = false;

	double_click_timer.is_working = false;
}

int main(void)
{
	periph_init();
	pwm_systick_t pwm_systick = INIT_PWM_SYSTICK;
	pwm_systick_start_timer(&pwm_systick);
	init_timer(&debounce_timer, debounce_handler, debounce_timer_id, 50);
	init_timer(&double_click_timer, double_click_handler, double_click_timer_id, 1000);

    while (true)
    {

		if (is_active)
			nrf_gpio_pin_write(leds[active_led], pwm_systick_state(&pwm_systick));
		else 
			turn_off_led(active_led);

		if (is_active && pwm_systick.current_duty == 0)
		{
			++blinks_passed;
		}

		active_led_switch();

        LOG_BACKEND_USB_PROCESS();
        NRF_LOG_PROCESS();
    }

	return 0;
}