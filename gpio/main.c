#include <stdint.h>
#include "nrf_delay.h"
#include "boards.h"
#include "nrf_gpio.h"

#define DELAY 1000
	
uint8_t device_id[] = { 6, 5, 8, 0 };
uint8_t delays_passed = 0;
uint8_t active_idx = 0;

uint32_t button = NRF_GPIO_PIN_MAP(1, 6);

uint32_t leds[] = { NRF_GPIO_PIN_MAP(0, 6), NRF_GPIO_PIN_MAP(0, 8), NRF_GPIO_PIN_MAP(1, 9), NRF_GPIO_PIN_MAP(0, 12) };

// infinite loop if device id is { 0, 0, 0, 0 }
// think about it better
void is_led_valid()
{
	if (delays_passed >= device_id[active_idx]) 
	{
		nrf_gpio_pin_write(leds[active_idx], 1);
		delays_passed = 0;
		active_idx = (active_idx + 1) % LEDS_NUMBER;
		is_led_valid();
	}
}

void init_gpio()
{
	nrf_gpio_cfg_input(button, NRF_GPIO_PIN_PULLUP);

	for(int i = 0; i < LEDS_NUMBER; i++) 
	{
		nrf_gpio_cfg_output(leds[i]);
		nrf_gpio_pin_write(leds[i], 1);
	}
}

int main(void)
{
	init_gpio();
	while (true)
	{
		is_led_valid();

		if (!nrf_gpio_pin_read(button)) 
		{
			nrf_gpio_pin_write(leds[active_idx], 0);
			nrf_delay_ms(DELAY);
			++delays_passed;
		}
	}

	return 0;
}
