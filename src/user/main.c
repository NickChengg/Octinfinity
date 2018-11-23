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
u8 moveDirection1 = 0, moveDirection2 = 0;
u8 select_motor = 1;


void UARTOnReceiveHandler(const u8 received){
    //whenever you type something in coolterm, 
    //each character will triger this function 
    //the character will be the argument for this function
	switch (received) {
		case '1': select_motor= 1; break;
		case '2': select_motor= 2; break;
		
		case '=':
		case '+': if (select_motor == 1)
								OC1 += 1;
							else OC2 += 1; break;
		case '_':
		case '-': if (select_motor == 1)
								OC1 -= 1;
							else OC2 -= 1; break;
		case '3': if (select_motor == 1)
								OC1 = 30;
							else OC2 = 30; break;
		case '4': if (select_motor == 1)
								OC1 = 40;
							else OC2 = 40; break;
		case '5': if (select_motor == 1)
								OC1 = 50;
							else OC2 = 50; break;
		case '6': if (select_motor == 1)
								OC1 = 60;
							else OC2 = 60; break;
		case '7': if (select_motor == 1)
								OC1 = 70;
							else OC2 = 70; break;
		case '8': if (select_motor == 1)
								OC1 = 80;
							else OC2 = 80; break;
		case '9': if (select_motor == 1)
								OC1 = 90;
							else OC2 = 90; break;
		case '0': if (select_motor == 1)
								OC1 = 0;
							else OC2 = 0; break;
		case 'D':
		case 'd': if (select_motor == 1)
								moveDirection1 = (moveDirection1 + 1)%2;
							else moveDirection2 = (moveDirection2 + 1)%2;
							break;
		case 'S':
		case 's': OC1 = 0;
							OC2 = 0; break;
		case 'M':
		case 'm': servo_control(SERVO2, 300);
	}
	
		uart_tx_str(COM1, "-----------------\nOC1:%d\nOC2:%d\nmotor sel:%d\n\n", OC1, OC2, select_motor);
		uart_tx_str(COM1, "Dir1:%d\nDir2:%d\n\nmotor sel:%d\n", moveDirection1, moveDirection2);
    return;
}



int main() {
	// Initialize Everything Here
	rcc_init();
	ticks_init();
	oled_init();
	buttons_init();
	tft_init(PIN_ON_TOP, WHITE, BLACK, RED, YELLOW); 
	servo_init(SERVO2, 287, 5000, 300);
	motor_init(MOTOR1, 144, 100, 100, moveDirection1); //at rest
	motor_init(MOTOR2, 144, 100, 100, moveDirection2); //at rest
	uart_init(COM1,115200); //debug
  uart_rx_init(COM1,&UARTOnReceiveHandler);
	/*while(1){
		for(u16 i=200;i<=400;i++){
			servo_control(SERVO2,i);
			delay(5);
		}
		for(u16 i=400;i>=200;i--){
			servo_control(SERVO2,i);
			delay(5);
			//for(u8 j=0;j<=255; j++){}
		}
	}*/
	
	while (1) {
		static u32 this_ticks = 0;
		while (get_ticks() == this_ticks);
		this_ticks = get_ticks();

		static u32 last_led_ticks=0;
		if ((this_ticks - last_led_ticks) >= 10) {
			last_led_ticks = this_ticks;
			//Code in here will run every xx ms
			/*
			if (!button_pressed(BUTTON1) && debounce1) {	//button pressed
				debounce1 = 0;
			}
			if (button_pressed(BUTTON1) && !debounce1) {
				debounce1 = 1;
				if (select_motor == 1)
					OC1 += 1;
				else OC2 += 1;
				
			}
			
			if (!button_pressed(BUTTON2) && debounce2) {
				debounce2 = 0;
			}
			if (button_pressed(BUTTON2) && !debounce2) {
				debounce2 = 1;
				if (select_motor == 1)
					OC1 -= 1;
				else OC2 -= 1;
				
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
			}*/
			tft_clear();
			tft_prints(0,0,"OC1:%d\nOC2:%d\n\nmotor sel:%d", OC1, OC2, select_motor);
			tft_update();
			motor_control(MOTOR1, 100 - ABS((int)OC1), moveDirection1);
			motor_control(MOTOR2, 100 - ABS((int)OC2), moveDirection2);
		}
	}
}
