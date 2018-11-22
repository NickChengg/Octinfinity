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


//def
typedef enum {	//movement
STILL = 0 , //stay still, indicate waiting for next movement
FORWARD = 1, //move forward
BACKWARD = 2, //for manual mode only
LEFTTURN = 3, // 90 degree left turn
RIGHTTURN = 4, // 90 degree right turn
ABOUTTURN = 5, // 180 degree right turn
ACTION = 6,	//non-movement: picking or throwing
} MOVEMENT;


typedef enum {	//progress
INIT = 0, //wait to start
TO_LZ = 1, //move forward
PICK_RACK = 2, // action
	
TO_TZ_1TURN = 3, // about turn
TO_TZ_2MOVE = 4, // forward
TO_TZ_3TURN = 5, // leftturn
TO_TZ_4MOVE = 6, // forward
TO_TZ_5TURN = 7, // right turn
TO_TZ_6MOVE = 8, // forward
TO_TZ_7TURN = 9, // about turn (~150 degree to right)
	
THROWING = 10, // action
LEAVE_TZ_TURN = 11, // right turn (~30 degree)

STAY = 15,

MANUAL = 20, //enter manual mode
} PROGRESS;



//init
MOVEMENT movement= STILL;
PROGRESS progress = INIT;
MOVEMENT manual_move = STILL; //for manual
u32 escape_ticks, difference_ticks = 0; //time of start turning; record the ticks difference(manual mode)
u8 move_count = 0; //no. of grip+turn to go

//testing value
u16 const motor1_fullSpeed =100, motor2_fullSpeed =100; // test value, range ~100, <=100
int32_t const motor_turnTO_OC = -100, motor_turnAWAY_OC = 100;// test vlaue, turn TO the direction(TO < AWAY). range: -100 to 100
u32 const turn_90_ticks = 100, turn_180_ticks = 200; // test value, for ticks difference of turning 90 or 180 degree
u32 const turn_30_ticks = 20; //test value, for ticks difference of turning 30 degree(to face to house)

void compensate_cal(int L_reading, int R_reading) { //time difference from left&right reading from line sensors
	//edwin: change the speed of each motor
	
	 // 
}

void motor_move(int motor_left_OC, int motor_right_OC) {
	u8 moveDirection_left = motor_left_OC > 0 ? 0 : 1; //
	u8 moveDirection_right = motor_right_OC > 0 ? 0 : 1; //
	motor_control(MOTOR1, 100 - ABS((int)motor_left_OC), moveDirection_left);
	motor_control(MOTOR2, 100 - ABS((int)motor_right_OC), moveDirection_right);
}
void motor_action(u32 this_ticks) {
	switch (progress) {
		case INIT: {
			if (1) {	//start condition
				//init next stage
				progress = TO_LZ;
				movement = FORWARD;
				move_count = 5; //sample value, change later
			}
			break;
		}
		case TO_LZ: {
			if (movement == STILL) { //finish movement, arrived LZ
				//init next stage
				progress = PICK_RACK;
				movement = ACTION;
			}
			break;
		}
		case PICK_RACK: {
			// code in "movement" switch-> action
			if (movement == STILL) { 
				//init next stage
				progress = TO_TZ_1TURN; //about turn
				movement = ABOUTTURN;
			}
			break;
		}
		case TO_TZ_1TURN: { //about turn
			if (movement == STILL) { 
				//init next stage
				progress = TO_TZ_2MOVE; //forward
				movement = FORWARD;
				move_count = 5; //sample value, change later
			}
			break;
		}
		case TO_TZ_2MOVE: { //forward
			if (movement == STILL) { 
				//init next stage
				progress = TO_TZ_3TURN; //left turn
				movement = LEFTTURN;
			}
			break;
		}
		case TO_TZ_3TURN: { //left turn
			if (movement == STILL) { 
				//init next stage
				progress = TO_TZ_4MOVE; //forward
				movement = FORWARD;
				move_count = 5; //sample value, change later
			}
			break;
		}
		case TO_TZ_4MOVE: { //forward
			if (movement == STILL) { 
				//init next stage
				progress = TO_TZ_5TURN; //right turn
				movement = RIGHTTURN;
			}
			break;
		}
		case TO_TZ_5TURN: { //right turn
			if (movement == STILL) { 
				//init next stage
				progress = TO_TZ_6MOVE; //forward
				movement = FORWARD;
				move_count = 5; //sample value, change later
			}
			break;
		}
		case TO_TZ_6MOVE: { //forward
			if (movement == STILL) { 
				//init next stage
				progress = TO_TZ_7TURN; //about turn
				movement = ABOUTTURN; //lesser than an about turn
				escape_ticks = this_ticks - turn_30_ticks; //sample value, change later
			}
			break;
		}
		case TO_TZ_7TURN: { //about turn (~150 degree to right)
			if (movement == STILL) { 
				//init next stage
				progress = THROWING; //action
				movement = ACTION;
			}
			break;
		}
		case THROWING: {
			// more code below, controlling the moving component
			
			
			if (movement == STILL) { 
				//init next stage
				progress = LEAVE_TZ_TURN; //
				movement = RIGHTTURN; 
				escape_ticks = this_ticks - (turn_90_ticks - turn_30_ticks); //sample value, change later
			}
			break;
		}
		case LEAVE_TZ_TURN: { //turn ~30 degree to right
			if (movement == STILL) { 
				//init next stage
				progress = STAY; //stay still, end of tasks
				movement = ABOUTTURN; //lesser than an about turn
				escape_ticks = this_ticks - turn_30_ticks; //sample value, change later
			}
			break;
		}
		
		
		case MANUAL: {
			if (this_ticks - escape_ticks>1000) {
				buzzer_off();
			}
			switch (manual_move) {
				case STILL: motor_move(0,0);break;
				case FORWARD: motor_move(50,50);break;
				case BACKWARD: motor_move(-50,-50);break;
				case LEFTTURN: motor_move(50,50);break;
				case RIGHTTURN: motor_move(50,50);break;
				default:break;
			}
			return;
		}
		default: break;
	}
	
	switch (movement) {
		case STILL:
			motor_move(0,0);
			break;
		case FORWARD: {
			if (move_count > 0) {
				motor_move(motor1_fullSpeed, motor2_fullSpeed);
				/*edwin: line sensor
				
				if (cross line)*/ {
					move_count--;
					escape_ticks = this_ticks;//need?
				}
			}
			else movement = STILL;
			break;
		}
		case LEFTTURN: {
			if (escape_ticks == 0) { //init escape ticks
				escape_ticks = this_ticks;
			}
			else { //start turning
				if ((this_ticks - escape_ticks) <= turn_90_ticks) { //turn
					motor_move(motor_turnTO_OC, motor_turnAWAY_OC);
				}
				else { //stop turning
					escape_ticks = 0;
					movement = STILL;
				}
			}
			break;
		}
		case RIGHTTURN: {
			if (escape_ticks == 0) { //init the escape ticks
				escape_ticks = this_ticks;
			}
			else { //start turning
				if ((this_ticks - escape_ticks) <= turn_90_ticks) { //turn
					motor_move(motor_turnAWAY_OC, motor_turnTO_OC);
				}
				else { //stop turning
					escape_ticks = 0;
					movement = STILL;
				}
			}
			break;
		}
		case ABOUTTURN: {
			if (escape_ticks == 0) { //init escape ticks
				escape_ticks = this_ticks;
			}
			else { //start turning
				if ((this_ticks - escape_ticks) <= turn_180_ticks) { //turn
					motor_move(motor_turnAWAY_OC, motor_turnTO_OC);
				}
				else { //stop turning
					escape_ticks = 0;
					movement = STILL;
				}
			}
			break;
		}
		case ACTION: {
			switch (progress) {
				case PICK_RACK: {
					// some code below
					
					
					if (1) {// picked, can be moved to next progress
						movement = STILL; //indicate end of action
					}
					break;
				}
				case THROWING: {
					// some code below
					//throw:
					
					
					if (1) {// throwed, can be moved to next progress
						movement = STILL; //indicate end of action
					}
					break;
				}
				default: break;
			}
		}
		default: break;
	}
}

void UARTOnReceiveHandler(const u8 received){
    //whenever you type something in coolterm, 
    //each character will triger this function 
    //the character will be the argument for this function
	progress = MANUAL;
	
	if (manual_move != STILL) {
		manual_move = STILL;
		difference_ticks = get_ticks() - escape_ticks; 
	}
	else {
		escape_ticks = get_ticks();
		switch (received) {
			case '1': buzzer_on();break; //testing
			case 'W':
			case 'w': {
				manual_move = FORWARD;
				break;
			}
			case 'A':
			case 'a': {
				manual_move = LEFTTURN;
				break;
			}
			case 'S':
			case 's': {
				manual_move = BACKWARD;
				break;
			}
			case 'D':
			case 'd': {
				manual_move = RIGHTTURN;;
				break;
			}
			case 'T':
			case 't': {
					//throw
				break;
			}
			case 'P':
			case 'p': {
					//pick rack or shuttlecock
				break;
			}
		}
	}
}

int main() {
	// Initialize Everything Here
	rcc_init();
	ticks_init();
	oled_init();
	buzzer_init();
	leds_init();
	motor_init(MOTOR1, 144, 100, 100, 1); //at rest
	motor_init(MOTOR2, 144, 100, 100, 1); //at rest
	tft_init(PIN_ON_TOP, WHITE, BLACK, RED, YELLOW); //debug
	uart_init(COM1,115200); //debug
  uart_rx_init(COM1,&UARTOnReceiveHandler);
	
	
	
	while (1) {
		static u32 this_ticks = 0;
		while (get_ticks() == this_ticks);
		this_ticks = get_ticks();

		static u32 last_led_ticks=0;
		if ((this_ticks - last_led_ticks) >= 25) {
			last_led_ticks = this_ticks;
			//Code in here will run every 25ms
			tft_clear();
			
			motor_action(this_ticks);
			
			uart_tx_str(COM1, "movement: %d\nprogress: %d", movement,progress);
			tft_prints(0, 0, "movement: %d\nprogress: %d", movement,progress);
			if (progress == MANUAL) {
				uart_tx_str(COM1, "maual_movement: %d\nticks diff: %d", manual_move,difference_ticks);
				tft_prints(0, 5, "maual_movement: %d\nticks diff: %d", manual_move,difference_ticks);
			}
			tft_update();
		}
	}
}
