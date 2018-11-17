/*	
 *	UST Robotics Team Internal 2018
 *
 *  Author  : Anshuman Medhi
 *  Contact : amedhi@connect.ust.hk 
 *						68925193
 *
 */

#include "main.h"

// Include Library Headers Here
#include "math.h"
#include "rcc.h"
#include "ticks.h"
#include "gpio.h"
#include "leds.h"
#include "buttons.h"
#include "buzzer.h"
#include "uart.h"
#include "lcd_main.h"
#include "oled.h"
#include "camera.h"
#include "pwm.h"
#include "adc.h"

u16 fullSpeed =100;
int16_t OC1 = 0, OC2 = 0;//
u32 move_tick = 0; //count 1s
u8 debounce1 = 0, debounce2 = 0, debounce3 = 0;
u8 moveDirection1 = 1, moveDirection2 = 1;
u8 select_motor = 1;

int main() {
	// Initialize Everything Here
	rcc_init();
	ticks_init();
	buttons_init();
	tft_init(PIN_ON_TOP, WHITE, BLACK, RED, YELLOW); 
	motor_init(MOTOR1, 144, fullSpeed, OC1, moveDirection1); //at rest
	motor_init(MOTOR2, 144, fullSpeed, OC2, moveDirection2); //at rest
	
	while (1) {
		static u32 this_ticks = 0;
		while (get_ticks() == this_ticks);
		this_ticks = get_ticks();

		static u32 last_led_ticks=0;
		if ((this_ticks - last_led_ticks) >= 10) {
			last_led_ticks = this_ticks;
			//Code in here will run every xx ms
			
			if (!button_pressed(BUTTON1) && debounce1) {	//button pressed
				debounce1 = 0;
			}
			if (button_pressed(BUTTON1) && !debounce1) {
				debounce1 = 1;
				if (select_motor == 1)
					OC1 += fullSpeed / 20;
				else OC2 += fullSpeed / 20;
				
			}
			
			if (!button_pressed(BUTTON2) && debounce2) {
				debounce2 = 0;
			}
			if (button_pressed(BUTTON2) && !debounce2) {
				debounce2 = 1;
				if (select_motor == 1)
					OC1 -= fullSpeed / 20;
				else OC2 -= fullSpeed / 20;
				
			}
			
			if (!button_pressed(BUTTON3) && debounce3) {
				debounce3 = 0;
			}
			if (button_pressed(BUTTON3) && !debounce3) {
				debounce3 = 1;
				if (select_motor == 1) {
					select_motor = 2;
				}
				else select_motor = 1;
			}

			OC1 = OC1>100? 100: OC1;
			OC1 = OC1<-100? -100: OC1;
			OC2 = OC2>100? 100: OC2;
			OC2 = OC2<-100? -100: OC2;
			
			if (OC1>=0) {
				moveDirection1 = 1;
			}
			else {
				moveDirection1 = 0;
			}
			if (OC2>=0) {
				moveDirection2 = 1;
			}
			else {
				moveDirection2 = 0;
			}
			tft_clear();
			tft_prints(0,0,"OC1:%d\nOC2:%d\n\nmotor sel:%d", OC1, OC2, select_motor);
			tft_update();
			motor_control(MOTOR1, ABS((int)OC1), moveDirection1);
			motor_control(MOTOR2, ABS((int)OC2), moveDirection2);
		}
	}
}
