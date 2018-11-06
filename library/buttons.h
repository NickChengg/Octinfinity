#ifndef __BUTTONS_H
#define __BUTTONS_H 

/*	
 *	Simple Button Library for 2018-19 Internal Mainboard
 *
 *  Author  : Anshuman Medhi
 *  Contact : amedhi@connect.ust.hk 
 *						68925193
 *
 */

#include "gpio.h"

#define NO_LISTENER ((ButtonListener)0)

typedef void (*ButtonListener)(void);

typedef enum {
	BUTTON1,
	BUTTON2,
	BUTTON3,
} Button;

/**
 * @brief      Initialize all onboard buttons
 */
void buttons_init(void);

/**
 * @brief      Returns whether the given button is pressed
 *
 * @param      button  The button
 *
 * @return     Logical Binary value of the state of the Button
 */
u8 button_pressed(Button button);

// HINT: what about a button hold listener? /**
//  * { list_item_description }
// @brief      Button update function with button pressed and released listeners */
//
void button_update(void);

/**
 * @brief      Sets the button down listener.
 *
 * @param      button    The button
 * @param[in]  listener  The listener
 */
void set_button_down_listener(Button button, ButtonListener listener);

/**
 * @brief      Resets the button down listener.
 *
 * @param      button    The button
 */
void reset_button_down_listener(Button button);

/**
 * @brief      Sets the button up listener.
 *
 * @param      button    The button
 * @param[in]  listener  The listener
 */
void set_button_up_listener(Button button, ButtonListener listener);

/**
 * @brief      Resets the button up listener.
 *
 * @param      button    The button
 */
void reset_button_up_listener(Button button);

#endif
