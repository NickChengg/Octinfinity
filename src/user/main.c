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
#include "stm32f10x.h" 
#include <math.h>
#include "stm32f10x_tim.h"

static int overflow=0;
void NVIC_Configuration(void);
void TIM_Configuration(void);
void GPIO_Configuration(void);
void RCC_Configuration(void);
float Sensor_using(void);
void delay1(void);

int main() {
	// Initialize Everything Here
	rcc_init();
	ticks_init();
	
	
	///////////////////external////////////
	
	#define module_interval 25
	#define object_distance 100.0
	
	
	//void delay(void);
	
	
	float result=0;
	RCC_Configuration();
	NVIC_Configuration();
	GPIO_Configuration();
	TIM_Configuration();
	
	adc_channel_init(ADC_IO_1);// set port 1 as output
	adc_init();
	
	tft_init(0, WHITE, RED, GREEN, DARK_RED);
	tft_clear();
	tft_prints(0,0,"HELLO");
	tft_update();
	
	
	
	while (1) {
		static u32 this_ticks = 0;
		result=Sensor_using();
		
		if(result>=object_distance)
		{
			GPIO_SetBits(GPIOC,GPIO_Pin_6);
			GPIO_ResetBits(GPIOC,GPIO_Pin_5);
		}
		else if(result>object_distance && result!=0.0)
		{
			GPIO_SetBits(GPIOC,GPIO_Pin_5);
			GPIO_ResetBits(GPIOC,GPIO_Pin_6);
		}
		else
		{
			GPIO_ResetBits(GPIOC,GPIO_Pin_6);
			GPIO_ResetBits(GPIOC,GPIO_Pin_5);
		}
		
		
		
		while (get_ticks() == this_ticks);
		this_ticks = get_ticks();
		
		tft_clear();
		tft_prints(0,0,"%f",result);
		tft_update();
		
		static u32 last_led_ticks=0;
		if ((this_ticks - last_led_ticks) >= 25) {
			last_led_ticks = this_ticks;
			//Code in here will run every 25ms
			
		}
	}
}

void RCC_Configuration(void)
{
	SystemInit();
}

void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE|RCC_APB2Periph_GPIOC, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;	//echo pin
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;	//trig pin
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	
}

void TIM_Configuration(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	TIM_DeInit(TIM2);
	TIM_TimeBaseStructure.TIM_Period=5000;
	TIM_TimeBaseStructure.TIM_Prescaler=0;
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	TIM_PrescalerConfig(TIM2,359,TIM_PSCReloadMode_Immediate);
	
	TIM_ARRPreloadConfig(TIM2, DISABLE);
	
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
	TIM_Cmd(TIM2, DISABLE);
}

//return in mm
float Sensor_using(void)
{
	float distance=0;
	u16 TIM=0;
	
	GPIO_SetBits(GPIOE,GPIO_Pin_8);
	delay1();
	GPIO_ResetBits(GPIOE,GPIO_Pin_8);
	
	TIM_Cmd(TIM2, ENABLE);
	
	while(!GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_7) && overflow==0);
	TIM2->CNT=0;
	
	while(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_7) && overflow==0);
	TIM_Cmd(TIM2, DISABLE);
	
	if(overflow!=0)
	{
		overflow=0;
		return 0.0;
	}
	TIM=TIM_GetCounter(TIM2);
	distance=(float)sqrt((TIM/20.0*17)*(TIM/20.0*17)-module_interval*module_interval/4.0)+12.0;
	//12 are buffers
	return(distance);
}

void delay1(void)
{
	u32 i;
	i=100000;
	
	while(i--);
}

void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)!=RESET)
	{
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
		overflow++;
	}
}
	