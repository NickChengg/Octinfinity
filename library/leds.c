#include "leds.h"

/**
 * @brief      Initialize all LEDS On the 2018-19 Internal Mainboard
 */
void leds_init(void) {
	gpio_init(LED1, GPIO_Mode_Out_PP);
	gpio_init(LED2, GPIO_Mode_Out_PP);
}

/**
 * @brief      Turn on the LED
 *
 * @param[in]  led   The LED
 */
void led_on(LED led) {
	gpio_set(led);
}

/**
 * @brief      Turn off the LED
 *
 * @param[in]  led   The LED
 */
void led_off(LED led) {
	gpio_reset(led);
}
