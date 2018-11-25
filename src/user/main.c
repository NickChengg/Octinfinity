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



u16 const servo_OC_min = 190, servo_OC_midd = 290, servo_OC_max = 390; //min->left, max->right (160,285,410)
u16 mag_FL = 0, mag_FR = 0, mag_BL = 0, mag_BR = 0; //magnetic reading in (front/back)(left/right)
u16 servo_OC = 290; //midd
u16 const motor1_Speed = 35, motor2_Speed = 35;// test vlaue, or changed by encoder. range: -100 to 100
float const MOTOR_COMPENSATE = 1;

static double turning_proportion = 0; //from reading of mag sensors
static const float directionComp = 0.9;


static float sensitivity = 1.3; //0-1, increse to turn more, vice versa
u8 debounce1 = 0, debounce2 = 0, debounce3 = 0;
u8 started = 0;
u32 escape_ticks = 0, diff_ticks = 0;
u32 looping_count=0;
typedef enum {
	forward = 0,
	backward = 1
} DIRECTION;

DIRECTION moveDirection = forward;



void mag_read() {
	switch (moveDirection) {
		case forward: {
			mag_FL = get_adc(ADC_IO_1);
			mag_FR = get_adc(ADC_IO_2);
			mag_BL = get_adc(ADC_IO_3);
			mag_BR = get_adc(ADC_IO_4);
			break;
		}
		case backward: {
			mag_FL = get_adc(ADC_IO_4);
			mag_FR = get_adc(ADC_IO_3);
			mag_BL = get_adc(ADC_IO_2);
			mag_BR = get_adc(ADC_IO_1);
			break;
		}
	}
}

double proportion_cal(int L_reading, int R_reading) { //left right reading from magnetic sensors
	switch (1) { //test diff algo
		case 1: {
			const u32 max_reading = 1200; //max reading of sensor(closest), test! 
			const u32 min_reading = 300; //min reading of sensor(farest), i.e. the value of background noise, test!
			s16 turning_percentage = 0; //turning percentage for servo, 100 for 100% of turning; -ve = left, +ve = right

			if (ABS(mag_FR - mag_FL) * sensitivity < (max_reading - min_reading)) {
				turning_percentage = (float)(R_reading - L_reading) / (max_reading - min_reading) * 100 * sensitivity;
			}
			else { // fix the range within -100 and 100
				turning_percentage = R_reading > L_reading ? 100 : -100;
			}
			tft_prints(0, 5, "TURN: %d\n ", turning_percentage);
			uart_tx_str(COM1, "TURN: %d\n ", turning_percentage);
			if (moveDirection == forward) 
				return  turning_percentage / 100; //(5000 * (1.5 + turning_percentage * 0.006) / 20); // OC servo
			else return -turning_percentage / 100;
		}
		case 2: {
			double distance_diff = 0;
			double static distance_max_diff = 0;
			u8 servo_max_angle = 150; //max angle of servo
			if (ABS(1/sqrt(mag_FR) - 1/sqrt(mag_FL)) < distance_max_diff) {
				distance_diff = (1/sqrt(mag_FR) - 1/sqrt(mag_FL)) ;
			}
			else { 
				distance_max_diff = ABS(1/sqrt(mag_FR) - 1/sqrt(mag_FL));
			}
			tft_prints(0, 6, "TURN: %d\n ", (int) distance_diff/distance_max_diff * 100);
			uart_tx_str(COM1, "TURN: %d\n ", (int) distance_diff/distance_max_diff * 100);
			
			return distance_diff/distance_max_diff;
		}
	}
}

void mag_compare() {
	const u32 acceptable_diff = 0; //turn only when excess acceptable difference, test! really need??

	if (ABS(mag_FL - mag_FR) > acceptable_diff) {
		if(moveDirection)//reduce servo output when move in reverse direction
		{
			turning_proportion = proportion_cal(mag_FL, mag_FR);
			servo_OC = (int) servo_OC_midd + (servo_OC_max - servo_OC_midd) * turning_proportion* directionComp;//L>R -> turn L
		}
		else
		{
			turning_proportion = proportion_cal(mag_BR, mag_BL);
			servo_OC = (int) servo_OC_midd + (servo_OC_max - servo_OC_midd) * turning_proportion ;//L>R -> turn L
		}
	}
	else if (ABS(mag_BL - mag_BR) > acceptable_diff) {
		turning_proportion = proportion_cal(mag_BR, mag_BL);
		if(moveDirection)//reduce servo output when move in reverse direction
		{
			servo_OC = (int) servo_OC_midd - (servo_OC_max - servo_OC_midd) * turning_proportion* directionComp; //L>R -> turn R
		}else
		{
			servo_OC = (int) servo_OC_midd - (servo_OC_max - servo_OC_midd) * turning_proportion ;
		}
	}
	
	//if (ABS(servo_OC - servo_OC_midd) > servo_OC_max - servo_OC_midd) 
	{
		//servo_OC =  servo_OC > servo_OC_max? servo_OC_max: servo_OC;
		//servo_OC =  servo_OC < servo_OC_min? servo_OC_min: servo_OC;
	}
	
	//servo_OC = turning_porportion(mag_FL, mag_FR) - turning_porportion(mag_BR, mag_BL); //alternative
}


void motor_move() {
	
	motor_control(MOTOR1, (int)(100- (motor2_Speed )* MOTOR_COMPENSATE), moveDirection);
	motor_control(MOTOR2, (int) (100-motor1_Speed), moveDirection);
}
int main() {
	// Initialize Everything Here
	rcc_init();
	ticks_init();
	oled_init();
	buttons_init();
	
	moveDirection=0;
	
	adc_channel_init(ADC_IO_1); //magnetic sensor in front left
	adc_channel_init(ADC_IO_2); //magnetic sensor in front right
	adc_channel_init(ADC_IO_3); //magnetic sensor in back left
	adc_channel_init(ADC_IO_4); //magnetic sensor in back right
	adc_init();
	//buzzer_init();
	servo_init(SERVO2, 287, 5000, servo_OC_midd); //5000 * x /20	//x=1.5->mid//375
	motor_init(MOTOR1, 144, 100, 100, moveDirection); //at rest
	motor_init(MOTOR2, 144, 100, 100, moveDirection); //at rest
	tft_init(PIN_ON_TOP, WHITE, BLACK, RED, YELLOW); //debug
	uart_init(COM1,115200); //debug
	
	static u32 this_ticks = 0;
	
	/*
	while(1)
	{
		while(get_ticks()==this_ticks);
		this_ticks=get_ticks();
		if(button_pressed(BUTTON1))
		{
			sensitivity+=0.1;
			servo_control(SERVO2,servo_OC_midd+sensitivity*10);
		}
		if(button_pressed(BUTTON2))
		{
			sensitivity-=0.1;
			servo_control(SERVO2,servo_OC_midd+sensitivity*10);
		}
	}
	*/
	
	while (1) {
		
		while (get_ticks() == this_ticks);
		this_ticks = get_ticks();
		
		if(button_pressed(BUTTON1))
		{
			looping_count=0;
		}
		
		
			//Code in here will run every 25ms
		while (!started || button_pressed(BUTTON1)) {
			started = 1;
			escape_ticks = this_ticks;
			
		}
		//led_on(LED1);
		if (!button_pressed(BUTTON1) && debounce1) {	//button pressed
			debounce1 = 0;
		}
		if (button_pressed(BUTTON1) && !debounce1) {
			debounce1 = 1;
			led_on(LED1);
			if (moveDirection) {
				moveDirection = 0;
				diff_ticks = get_ticks() - escape_ticks;
			}
			else {
				moveDirection = 1;
				diff_ticks = get_ticks() - escape_ticks;
			}
			
			motor_control(MOTOR1, 100, moveDirection); //stop
			motor_control(MOTOR2, 100, moveDirection);

			delay(1000);
			escape_ticks = get_ticks();
		}
		if (!button_pressed(BUTTON2) && debounce2) {	//button pressed
			debounce2 = 0;
		}
		if (button_pressed(BUTTON2) && !debounce2) {
			debounce2 = 1;
			sensitivity +=0.1;
		}
		if (!button_pressed(BUTTON3) && debounce3) {	//button pressed
			debounce3 = 0;
		}
		if (button_pressed(BUTTON3) && !debounce3) {
			debounce3 = 1;
			sensitivity -= 0.1;
			if (sensitivity < 0)
				sensitivity = 0;
		}
		
			
			tft_clear();
			mag_read();
			mag_compare();
			servo_control(SERVO2, servo_OC);
			motor_move();
			looping_count++;
			tft_prints(0, 0, "FL%d FR%d \nBL%d BR%d\nOC: %d\ndir: %d\ndiff_ticks: %d \nsensitivity: %.1f\nLoop Count:%d",
								mag_FL, mag_FR, mag_BL, mag_BR, servo_OC, moveDirection, diff_ticks, sensitivity, looping_count);
			tft_update();
			uart_tx_str(COM1, "FL%d FR%d \nBL%d BR%d\nOC: %d\ndir: %d\ndiff_ticks: %d \nsensitivity: %.1f", mag_FL, mag_FR, mag_BL, mag_BR, servo_OC, moveDirection, diff_ticks, sensitivity);
			if(looping_count>2200)
			{
				looping_count=0;
				servo_control(SERVO2, servo_OC_midd);
				motor_control(MOTOR1, 100, moveDirection); //stop
				motor_control(MOTOR2, 100, moveDirection);
				
				delay(2200);
				moveDirection=!moveDirection;
			}
			static u32 last_led_ticks=0;
		if ((this_ticks - last_led_ticks) >= 25) {
			last_led_ticks = this_ticks;
		}
	}
}