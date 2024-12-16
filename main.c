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

#include "pwm_module.h"
#include "button_handler.h"
#include "led_module.h"

uint8_t DEVICE_ID[] = { 6, 5, 8, 0 };

void logs_init(void)
{
    ret_code_t ret = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(ret);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}

void periph_init(void)
{
	init_leds();
	app_timer_init();
	logs_init();
	init_handlers();
	pwm_init();
	pwm_start();
}

int main(void)
{
	periph_init();

    while (true)
    {
		leds_main();
        LOG_BACKEND_USB_PROCESS();
        NRF_LOG_PROCESS();
		nrf_delay_ms(20);
    }

	return 0;
}