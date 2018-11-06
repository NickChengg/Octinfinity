#include "buttons.h"

typedef struct {
	const GPIOPin* pin;
	u8 pressed;
	ButtonListener button_down;
	ButtonListener button_up;
} ButtonStruct;

#define NO_OF_BUTTONS (3)
static ButtonStruct BUTTONS[NO_OF_BUTTONS] = {
	{&PC13, 0, 0, 0},
	{&PC14, 0, 0, 0},
	{&PC15, 0, 0, 0},
};

/**
 * @brief      Initialize all onboard buttons
 */
void buttons_init() {
	for (int i = 0; i < NO_OF_BUTTONS; ++i) {
		gpio_init(BUTTONS[i].pin, GPIO_Mode_IPU);
	}
}

/**
 * @brief      Returns whether the given button is pressed
 *
 * @param      button  The Button
 *
 * @return     Logical Binary value of the state of the Button
 */
u8 button_pressed(Button button) {
	return !gpio_read(BUTTONS[button].pin);
}

/**
 * @brief      Button update function with button pressed and released listeners
 */
void button_update(void) {
	for (int i = 0; i < NO_OF_BUTTONS; ++i) {
		u8 pressed = button_pressed(i);

		if (BUTTONS[i].button_down && pressed && !BUTTONS[i].pressed) {
			BUTTONS[i].button_down();
		}
		if (BUTTONS[i].button_up && !pressed && BUTTONS[i].pressed) {
			BUTTONS[i].button_up();
		}
		
		BUTTONS[i].pressed = pressed;
	}
}

/**
 * @brief      Sets the button down listener.
 *
 * @param      button    The button
 * @param[in]  listener  The listener
 */
void set_button_down_listener(Button button, ButtonListener listener) {
	BUTTONS[button].button_down = listener;
}

/**
 * @brief      Resets the button down listener.
 *
 * @param      button    The button
 */
void reset_button_down_listener(Button button) {
	BUTTONS[button].button_down = 0;
}

/**
 * @brief      Sets the button up listener.
 *
 * @param      button    The button
 * @param[in]  listener  The listener
 */
void set_button_up_listener(Button button, ButtonListener listener) {
	BUTTONS[button].button_up = listener;
}

/**
 * @brief      Resets the button up listener.
 *
 * @param      button    The button
 */
void reset_button_up_listener(Button button) {
	BUTTONS[button].button_up = 0;
}
