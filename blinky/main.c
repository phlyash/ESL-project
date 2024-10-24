#include <stdint.h>
#include "nrf_delay.h"
#include "boards.h"

#define DELAY 1000
#define DELAY_ON_OFF 500

int main(void)
{
	bsp_board_init(BSP_INIT_LEDS);

	uint8_t device_id = { 6, 5, 8, 0 };
	while (true)
	{
		for(int i = 0; i < LEDS_NUMBER; i++)
		{
			bsp_board_led_invert(i);
			nrf_delay_ms(DELAY_ON_OFF);
		}
		bsp_board_led_off(i);
		nrf_delay_ms(DELAY);
	}

	return 0;
}
