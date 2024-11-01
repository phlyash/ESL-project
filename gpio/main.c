#include <stdint.h>
#include "nrf_delay.h"
#include "nrf_gpio.h"

#define GPIO_LED_TURN_OFF 1
#define GPIO_LED_TURN_ON 0

#define LEDS_NUMBER 4
#define DELAY 1000
#define BLINK_DELAY 20
	
uint8_t DEVICE_ID[] = { 6, 5, 8, 0 };
uint8_t delays_passed = 0;
uint8_t active_led = 0;

uint32_t button = NRF_GPIO_PIN_MAP(1, 6);

uint32_t leds[] = { NRF_GPIO_PIN_MAP(0, 6), NRF_GPIO_PIN_MAP(0, 8), NRF_GPIO_PIN_MAP(1, 9), NRF_GPIO_PIN_MAP(0, 12) };

void turn_off_led(uint8_t led_idx) 
{
	nrf_gpio_pin_write(leds[led_idx], GPIO_LED_TURN_OFF);
}

void turn_on_led(uint8_t led_idx) 
{
	nrf_gpio_pin_write(leds[led_idx], GPIO_LED_TURN_ON);
}

// infinite loop if device id is { 0, 0, 0, 0 }
// think about it better
void active_led_switch(void)
{
	if (delays_passed >= DEVICE_ID[active_led]) 
	{
		turn_off_led(active_led);
		delays_passed = 0;
		active_led = (active_led + 1) % LEDS_NUMBER;

		active_led_switch();
	}

	turn_on_led(active_led);
}

void init_gpio(void)
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
		if (!nrf_gpio_pin_read(button)) 
		{
			turn_off_led(active_led);
			nrf_delay_ms(BLINK_DELAY);
			turn_on_led(active_led);
			nrf_delay_ms(DELAY);
			++delays_passed;
		}

		active_led_switch();
	}

	return 0;
}
