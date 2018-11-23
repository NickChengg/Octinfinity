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
#include "ultrasonic.h"

const int cycle=5;//in ms, 1 cycle for signal, 5 cycle after receive echo
int TOTAL=0;
static long int output= 0;
const double TRAN_CM=1*340.0*100/72000000.0;
float dist_cm=0;
static uint32_t this_ticks = 0;
static u32 us_ticks = 0;


void EchoPrint()
{
	//FLAG=1;
	TOTAL=0;
}


void EXTI7_IRQHandler()
{
	uint32_t temp=SysTick->VAL;//fix the clock cycle
	if(temp-ULTRA_EMIT>10)
	{
			if(temp-ULTRA_EMIT>10)
			{
				OUT_NUM=temp-ULTRA_EMIT;//now sysclock-sysclock of sending signal
			}
			else
			{
				//OUT_NUM=temp+72000000-ULTRA_EMIT;
				OUT_NUM=temp;
			}
			
			//if(temp>1000000)//check out of bound by clock
			if(OUT_NUM>1000000)
			{
				OUT_NUM=1000000;
			}
			ULTRA_EMIT=SysTick->VAL;
			//OUT_NUM=temp;
			//translate into cm
			dist_cm=OUT_NUM*TRAN_CM;
			FLAG=1;
		}
	
}




u16 const servo_OC_min = 275, servo_OC_midd = 300, servo_OC_max = 425; //min->left, max->right (160,285,410)
u16 mag_FL = 0, mag_FR = 0, mag_BL = 0, mag_BR = 0; //magnetic reading in (front/back)(left/right)
u16 servo_OC = 300; //midd
u16 const motor1_maxSpeed =60,motor2_maxSpeed =60;
u16 const motor1_minSpeed = 35, motor2_minSpeed = 35;// test vlaue, or changed by encoder. range: -100 to 100
static u16 turning_proportion; //from reading of mag sensors

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
			const float sensitivity = 1; //0-1, increse to turn more, vice versa
			s16 turning_percentage = 0; //turning percentage for servo, 100 for 100% of turning; -ve = left, +ve = right

			if (ABS(mag_FR - mag_FL) * sensitivity < (max_reading - min_reading)) {
				turning_percentage = (float)(R_reading - L_reading) / (max_reading - min_reading) * 100 * sensitivity;
			}
			else { // fix the range within -100 and 100
				turning_percentage = R_reading > L_reading ? 100 : -100;
			}
			tft_prints(0, 5, "TURN: %d\n ", turning_percentage);
			uart_tx_str(COM1, "TURN: %d\n ", turning_percentage);
			
			return  turning_percentage / 100; //(5000 * (1.5 + turning_percentage * 0.006) / 20); // OC servo
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
			tft_prints(0, 5, "TURN: %d\n ", (int) distance_diff/distance_max_diff * 100);
			uart_tx_str(COM1, "TURN: %d\n ", (int) distance_diff/distance_max_diff * 100);
			
			return distance_diff/distance_max_diff;
		}
	}
}

void mag_compare() {
	const u32 acceptable_diff = 0; //turn only when excess acceptable difference, test! really need??

	if (ABS(mag_FL - mag_FR) > acceptable_diff) {
		turning_proportion = proportion_cal(mag_FL, mag_FR);
		servo_OC = (int) servo_OC_midd + (servo_OC_max - servo_OC_midd) * turning_proportion;//L>R -> turn L
	}
	else if (ABS(mag_BL - mag_BR) > acceptable_diff) {
		turning_proportion = proportion_cal(mag_BR, mag_BL);
		servo_OC = (int) servo_OC_midd + (servo_OC_max - servo_OC_midd) * turning_proportion; //L>R -> turn R
	}
	
	if (ABS(servo_OC - servo_OC_midd) > servo_OC_max - servo_OC_midd) {
		servo_OC =  servo_OC > servo_OC_max? servo_OC_max: servo_OC_min;
	}
	
	//servo_OC = turning_porportion(mag_FL, mag_FR) - turning_porportion(mag_BR, mag_BL); //alternative
}


void motor_move() {
	u16 motor1_OC = (motor1_maxSpeed - motor1_minSpeed)* turning_proportion + motor1_minSpeed;
	u16 motor2_OC = (motor2_maxSpeed - motor2_minSpeed)* turning_proportion + motor2_minSpeed;
	
	motor_control(MOTOR1, 100-(int)motor1_OC, moveDirection);
	motor_control(MOTOR2, 100-(int)motor2_OC, moveDirection);
}
int main() {
	// Initialize Everything Here
	rcc_init();
	ticks_init();
	oled_init();
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
	
	tft_init(0, WHITE, RED, GREEN, DARK_RED);
	tft_clear();
	tft_prints(0,0,"hello",TOTAL);
	tft_update();
	
	us_init();
	//setReceive_listener(EchoPrint);//set interupt to receive ultrasonic
	gpio_exti_init(GPIO8,EXTI_Trigger_Rising_Falling);
	//gpio_exti_init(GPIO8,EXTI_Trigger_Rising);
	ULTRA_EMIT=SysTick->VAL;
	
	while (1) {
		
		
		//while (get_ticks() == this_ticks)
		while(SysTick->VAL==us_ticks)//the speed of code become 1/72,000,000
		{
			
		}
		//this_ticks =get_ticks(); 
		us_ticks =SysTick->VAL;//
		
		set_cycle(us_ticks);
		//this_ticks = SysTick->VAL;//get_ticks();
		
		
		
		
		//long int reflection=set_cycle(TOTAL,cycle);
		static u32 last_led_ticks=0;
		if ((this_ticks - last_led_ticks) >= 25) {
			last_led_ticks = this_ticks;
			
		
		

			//Code in here will run every 25ms
			if(this_ticks!=get_ticks())
			{
			if(FLAG)
		{
			tft_clear();
			tft_prints(0,0,"??? %lu \nDetected at distances:%f\n%f"	,OUT_NUM,dist_cm,TRAN_CM);
			tft_update();
			FLAG=0;
		}
			
			led_on(LED1);
			tft_clear();
			mag_read();
			mag_compare();
			servo_control(SERVO2, servo_OC);
			motor_move();
			tft_prints(0, 0, "FL%d FR%d \nBL%d BR%d\nOC: %d", mag_FL, mag_FR, mag_BL, mag_BR, servo_OC);
			tft_update();
			uart_tx_str(COM1, "FL:%d  FR:%d\n BL:%d BR:%d\n", mag_FL, mag_FR, mag_BL, mag_BR);
	}
		}
	}
}
