#include "pwm.h"

#include "rcc.h"
#include "stm32f10x_tim.h"

const ServoPort __SERVO1 = {PWM_CHANNEL_1,&PA15,&SERVO_TIM->CCR1};
const ServoPort __SERVO2 = {PWM_CHANNEL_4,&PB11,&SERVO_TIM->CCR4};
const ServoPort __SERVO3 = {PWM_CHANNEL_3,&PB10,&SERVO_TIM->CCR3};

const MotorPort __MOTOR1 = {{PWM_CHANNEL_1,&PC6,&MOTOR_TIM->CCR1},&PC3};
const MotorPort __MOTOR2 = {{PWM_CHANNEL_2,&PC7,&MOTOR_TIM->CCR2},&PA8};
const MotorPort __MOTOR3 = {{PWM_CHANNEL_3,&PC8,&MOTOR_TIM->CCR3},&PC9};

const PwmIOPort __PWM_IO1 = {{PWM_CHANNEL_1,&PA0,&TIM5->CCR1}, TIM5};
const PwmIOPort __PWM_IO2 = {{PWM_CHANNEL_2,&PA1,&TIM5->CCR2}, TIM5};
const PwmIOPort __PWM_IO3 = {{PWM_CHANNEL_3,&PA2,&TIM5->CCR3}, TIM5};
const PwmIOPort __PWM_IO4 = {{PWM_CHANNEL_4,&PA3,&TIM5->CCR4}, TIM5};
const PwmIOPort __PWM_IO7 = {{PWM_CHANNEL_1,&PA6,&TIM3->CCR1}, TIM3};
const PwmIOPort __PWM_IO8 = {{PWM_CHANNEL_2,&PA7,&TIM3->CCR2}, TIM3};

void pwm_init(TIM_TypeDef* timer, const PWMChannel* channel, u16 prescaler, u16 autoreload, u16 initial) {
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	tim_rcc_init(timer);
	
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseStructure.TIM_Prescaler = prescaler;
	TIM_TimeBaseStructure.TIM_Period = autoreload;
	TIM_TimeBaseInit(timer, &TIM_TimeBaseStructure);

	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
	TIM_OCInitStructure.TIM_Pulse = initial;

	switch (channel->channel) {
		case PWM_CHANNEL_1:
			TIM_OC1Init(timer, &TIM_OCInitStructure);
			TIM_OC1PreloadConfig(timer, ENABLE);
			break;

		case PWM_CHANNEL_2:
			TIM_OC2Init(timer, &TIM_OCInitStructure);
			TIM_OC2PreloadConfig(timer, ENABLE);
			break;

		case PWM_CHANNEL_3:
			TIM_OC3Init(timer, &TIM_OCInitStructure);
			TIM_OC3PreloadConfig(timer, ENABLE);
			break;

		case PWM_CHANNEL_4:
			TIM_OC4Init(timer, &TIM_OCInitStructure);
			TIM_OC4PreloadConfig(timer, ENABLE);
			break;

	}

	gpio_init(channel->pin, GPIO_Mode_AF_PP);

	TIM_ARRPreloadConfig(timer, ENABLE);
	TIM_Cmd(timer, ENABLE);	
	TIM_SetCounter(timer,0);
	TIM_CtrlPWMOutputs(timer, ENABLE);
}

static inline void pwm_control(const PWMChannel* channel, u16 duty) {
	*(channel->ccr) = duty;
}

void motor_init(const MotorPort* motor, u16 prescaler, u16 autoreload, u16 magnitude, u8 direction) {
	pwm_init(MOTOR_TIM, (const PWMChannel*) motor, prescaler, autoreload, magnitude);

	gpio_init(motor->dir, GPIO_Mode_Out_PP);
	gpio_write(motor->dir, direction);

	GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE);
}
void motor_control(const MotorPort* motor, u16 magnitude, u8 direction) {
	gpio_write(motor->dir, direction);
	pwm_control((const PWMChannel*) motor, magnitude);
}

void servo_init(const ServoPort* servo, u16 prescaler, u16 autoreload, u16 initial) {
	pwm_init(SERVO_TIM, (const PWMChannel*) servo, prescaler, autoreload, initial);

	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	GPIO_PinRemapConfig(GPIO_FullRemap_TIM2, ENABLE);
}

void servo_control(const ServoPort* servo, u16 position) {
	pwm_control((const PWMChannel*) servo, position);
}

void pwm_io_init(const PwmIOPort* io, u16 prescaler, u16 autoreload, u16 initial) {
	pwm_init(io->tim, (const PWMChannel*) io, prescaler, autoreload, initial);

}

void pwm_io_control(const PwmIOPort* io, u16 duty) {
	pwm_control((const PWMChannel*) io, duty);
}
