#include "buzzer.h"

#define BUZZER_PIN &PC0

/**
 * @brief      Initialize the Onboard Buzzer
 */
void buzzer_init(void) {
	gpio_init(BUZZER_PIN, GPIO_Mode_Out_PP);
}

/**
 * @brief      Start Buzzing
 */
void buzzer_on(void) {
	gpio_set(BUZZER_PIN);
}

/**
 * @brief      Stop Buzzing
 */
void buzzer_off(void) {
	gpio_reset(BUZZER_PIN);
}

