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

#define GRAB_PIN &PA0 //grapping hold/release
#define GRAB_UP_PIN &PA2 //grapping up/down
#define THROW_PIN &PA1 //th8row /reset (&PB12 WORK)


//def
typedef enum {	//movement
STILL = 0 , //stay still, indicate waiting for next movement
FORWARD = 1, //move forward
BACKWARD = 2, //for manual mode only
LEFTTURN = 3, // 90 degree left turn
RIGHTTURN = 4, // 90 degree right turn
ABOUTTURN = 5, // 180 degree right turn
	
GRAB_PICK = 10,	//picking, up and hold
GRAB_RELEASE = 11, //releasing, down and release
	
MANUAL_GRAB_UP = 15,	//picking, up
MANUAL_GRAB_DOWN = 16, //releasing, down 
MANUAL_GRAB_HOLD = 17, //picking, hold
MANUAL_GRAB_RELEASE = 18, //releasing, loose
MANUAL_THROW_SET = 19, //throwing
MANUAL_THROW_RESET = 20, //threw, restore position
	
THROW = 25, //inclde set ans reset
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
	
THROWING = 10, // action (include throw set and reset)
LEAVE_TZ_TURN = 11, // right turn (~30 degree)

STAY = 15,

MANUAL = 20, //enter manual mode
} PROGRESS;



//init
MOVEMENT movement= STILL;
//PROGRESS progress = INIT;
PROGRESS progress = MANUAL;//////////CHAGEEEEEE
u32 escape_ticks, difference_ticks = 0; //time of start turning; record the ticks difference(manual mode)
u8 move_count = 0; //no. of grip+turn to go
u8 picked = 0, grabbed = 0, threw = 0;

//testing value
u16 const motor_fullSpeed = 90;
double const motor1_compensate =1, motor2_compensate =1; // tNiest value, range ~100, <=100
double const motor_turnTO_proportion = 0.3, motor_turnAWAY_proportion = 1, motor_manual_turning_compensate = 0.7;// test vlaue, turn TO the direction(TO < AWAY). 
u32 const forward_ticks = 750, turn_left_ticks = 1950, turn_right_ticks = 1500, turn_180_ticks = 950; // test value, for ticks difference of turning 90 or 180 degree
u32 const turn_30_ticks = 20; //test value, for ticks difference of turning 30 degree(to face to house)

void grabHold() {
	gpio_write(GRAB_PIN,1);
}
void grabRelease() {
	gpio_write(GRAB_PIN,0);
}

void grabUp() {
	gpio_write(GRAB_UP_PIN,1);
}
void grabDown() {
	gpio_write(GRAB_UP_PIN,0);
}

void throwSet() {
	gpio_write(THROW_PIN,1);
	buzzer_on();
}
void throwReset() {
	gpio_write(THROW_PIN,0);
}


void compensate_cal(int L_reading, int R_reading) { //time difference from left&right reading from line sensors
	//edwin: change the speed of each motor
	
	 // 
}

void motor_move(double motor_left_OC, double motor_right_OC) {
	u8 moveDirection_left = motor_left_OC > 0 ? 0 : 1; //
	u8 moveDirection_right = motor_right_OC > 0 ? 0 : 1; //
	motor_left_OC = motor_left_OC > 100? 100 : motor_left_OC < -100? -100 : motor_left_OC;
	motor_right_OC = motor_right_OC > 100? 100 : motor_right_OC < -100? -100 : motor_right_OC;
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
				movement = GRAB_PICK;
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
				progress = STAY; //stop here
				movement = GRAB_RELEASE;
			}
			break;
		}
		/*
		case THROWING: {
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
		*/
		
		
		case MANUAL: {
			if (get_ticks()-	 escape_ticks>1000){
				buzzer_off();
				//movement=STILL;
			}
			
			switch (movement) { //override moving functions
				case STILL: motor_move(0,0); return;
				case FORWARD: motor_move(motor_fullSpeed * motor1_compensate , motor_fullSpeed * motor2_compensate); return;
				case BACKWARD: motor_move(-motor_fullSpeed * motor1_compensate, -motor_fullSpeed * motor2_compensate); return;
				case LEFTTURN: motor_move(motor_fullSpeed * motor1_compensate * motor_turnTO_proportion * motor_manual_turning_compensate, motor_fullSpeed * motor2_compensate * motor_turnAWAY_proportion * motor_manual_turning_compensate); return;
				case RIGHTTURN: motor_move(motor_fullSpeed * motor1_compensate * motor_turnAWAY_proportion * motor_manual_turning_compensate, motor_fullSpeed * motor2_compensate * motor_turnTO_proportion * motor_manual_turning_compensate); return;
				case MANUAL_GRAB_HOLD: break;
				case MANUAL_GRAB_RELEASE: break;
				case MANUAL_GRAB_UP: break;
				case MANUAL_GRAB_DOWN: break;
				case MANUAL_THROW_SET: break;
				case MANUAL_THROW_RESET: break;
				default:break;
			}
		}
		default: break;
	}
	
	switch (movement) {
		case STILL:
			motor_move(0,0);
			break;
		case FORWARD: {
			if (move_count > 0) {
				motor_move(motor_fullSpeed * motor1_compensate, motor_fullSpeed * motor2_compensate);
				/*edwin: line sensor
				
				if (cross line)*/ {
					move_count--;
					escape_ticks = this_ticks;//need?
				}
			}
			else movement = STILL;
			break;
		}
		case BACKWARD: {
			if (move_count > 0) {
				motor_move(-motor_fullSpeed * motor1_compensate, -motor_fullSpeed * motor2_compensate);
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
					motor_move(motor_fullSpeed * motor1_compensate * motor_turnTO_proportion, motor_fullSpeed * motor2_compensate * motor_turnAWAY_proportion);
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
					motor_move(motor_fullSpeed * motor1_compensate *  motor_turnAWAY_proportion, motor_fullSpeed * motor2_compensate * motor_turnTO_proportion);
				}
				else { //stop turning
					escape_ticks = 0;
					movement = STILL;
				}
			}
			break;
		}
		case ABOUTTURN: { //180 degree to the right
			if (escape_ticks == 0) { //init escape ticks
				escape_ticks = this_ticks;
			}
			else { //start turning
				if ((this_ticks - escape_ticks) <= turn_180_ticks) { //turn
					motor_move(motor_fullSpeed * motor1_compensate *  motor_turnAWAY_proportion, motor_fullSpeed * motor2_compensate * motor_turnTO_proportion);
				}
				else { //stop turning
					escape_ticks = 0;
					movement = STILL;
				}
			}
			break;
		}
		
		case GRAB_PICK: {
			delay(100);
			grabHold(); //hold
			
			delay(100);
			grabUp(); //up
			
			picked = 1;
			movement = STILL;
			break;
		}
		case GRAB_RELEASE: {
			delay(100);
			grabRelease(); //release
			
			delay(100);
			grabDown(); //down
			
			picked = 0;
			movement = STILL;
			break;
		}
		
		case MANUAL_GRAB_HOLD: {
			delay(100);
			grabHold();
			
			grabbed = 1;
			movement = STILL;
			break;
		}
		case MANUAL_GRAB_RELEASE: {
			delay(100);
			grabRelease();
			
			grabbed = 0;
			movement = STILL;
			break;
		}
		case MANUAL_GRAB_UP: {
			delay(100);
			grabUp();
			
			picked = 1;
			movement = STILL;
			break;
		}
		case MANUAL_GRAB_DOWN: {
			delay(100);
			grabDown();
			
			picked = 0;
			movement = STILL;
			break;
		}
		case MANUAL_THROW_SET: {
			delay(100);
			throwSet();
			
			threw = 1;
			movement = STILL;
			break;
		}
		case MANUAL_THROW_RESET: {
			delay(100);
			throwReset();
			
			threw = 0;
			movement = STILL;
			break;
		}

		
		case THROW: {
			delay(100);
			throwSet();
			delay(1000);
			throwReset();
			movement = STILL;
			break;
		}

		default: buzzer_on();break;
	}
}



void UARTOnReceiveHandler(const u8 received){
    //whenever you type something in coolterm, 
    //each character will triger this function 
    //the character will be the argument for this function
	if (progress != MANUAL) {
		progress = MANUAL;
		movement = STILL;
	}
	
	if (movement != STILL) {
		movement = STILL;
		difference_ticks = get_ticks() - escape_ticks; 
		
	}
	else {
		escape_ticks = get_ticks();
		
		switch (received) {
			case '1': buzzer_on();break; //testing
			case 'W':
			case 'w': {
				movement = FORWARD;
				break;
			}
			case 'A':
			case 'a': {
				movement = LEFTTURN;
				break;
			}
			case 'S':
			case 's': {
				movement = BACKWARD;
				break;
			}
			case 'D':
			case 'd': {
				movement = RIGHTTURN;;
				break;
			}
			case 'T':
			case 't': {
					//throw
					if (! threw) 
						movement = MANUAL_THROW_SET;
					else
						movement = MANUAL_THROW_RESET;
				break;
			}
			case 'G':
			case 'g': {
					//
					if (! grabbed) 
						movement = MANUAL_GRAB_HOLD;
					else
						movement = MANUAL_GRAB_RELEASE;
				break;
			}
				
			case 'P':
			case 'p': {
					//
					if (! picked) 
						movement = MANUAL_GRAB_UP;
					else
						movement = MANUAL_GRAB_DOWN;
				break;
			}
			case ' ': movement = STILL;
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
	gpio_init(GRAB_UP_PIN, GPIO_Mode_Out_PP);
	gpio_init(GRAB_PIN, GPIO_Mode_Out_PP);
	gpio_init(THROW_PIN, GPIO_Mode_Out_PP);
	motor_init(MOTOR1, 144, 100, 100, 0); //at rest
	motor_init(MOTOR2, 144, 100, 100, 0); //at rest
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
				uart_tx_str(COM1, "maual_movement: %d\nticks diff: %d", movement,difference_ticks);
				tft_prints(0, 5, "maual_movement: %d\nticks diff: %d", movement,difference_ticks);
			}
			tft_update();
		}
	}
}
