#ifndef __PWM_H
#define __PWM_H

/*	
 *	PWM Library for 2018-19 Internal Mainboard
 *	
 *	Preconfigured timers are:
 *	TIM3 as a 3 channel motor controller
 *	TIM2 as a 3 channel servo controller
 *	
 *	However any timer in the MCU can be used to generated PWM in many forms and for many purposes.
 *	(and many other functions)
 *	
 * PS. Always Remember your Prescaler and AutoReload Value formulas
 *
 *  Author  : Anshuman Medhi
 *  Contact : amedhi@connect.ust.hk 
 *						68925193
 *
 */

#include "gpio.h"

#define MOTOR_TIM TIM8
#define SERVO_TIM TIM2

typedef enum {
	PWM_CHANNEL_1,
	PWM_CHANNEL_2,
	PWM_CHANNEL_3,
	PWM_CHANNEL_4,
} PWMChannelID;

typedef struct {
	PWMChannelID channel;
	const GPIOPin* pin;
	volatile uint16_t* ccr; // Pointer to the Capture Compare Register of the corresponding
	                        // Timer OC Channel (In general *TIMx->CCRy)
} PWMChannel;

typedef PWMChannel ServoPort;

typedef struct {
	PWMChannel pwm;
	TIM_TypeDef* tim;
} PwmIOPort;

typedef struct {
	PWMChannel pwm;
	const GPIOPin* dir;
} MotorPort;

//** Feel free to extend the library **//

/**
 * @brief      Initialize a custom PWM Timer
 *
 * @param      timer       The timer
 * @param[in]  channel     The channel
 * @param[in]  prescaler   The prescaler
 * @param[in]  autoreload  The autoreload
 * @param[in]  initial     The initial duty
 */
void pwm_init(TIM_TypeDef* timer, const PWMChannel* channel, u16 prescaler, u16 autoreload, u16 initial);

/**
 * @brief      Initialize a Motor Port
 *
 * @param[in]  motor       The Motor Port
 * @param[in]  prescaler   The prescaler
 * @param[in]  autoreload  The autoreload
 * @param[in]  magnitude   The magnitude
 * @param[in]  direction   The direction
 */
void motor_init(const MotorPort* motor, u16 prescaler, u16 autoreload, u16 magnitude, u8 direction);

/**
 * @brief      Control a Motor Port
 *
 * @param[in]  motor      The Motor Port
 * @param[in]  magnitude  The magnitude
 * @param[in]  direction  The direction
 */
void motor_control(const MotorPort* motor, u16 magnitude, u8 direction);
// HINT: What about a motor_control() that takes speed as a signed integer?

/**
 * @brief      Initialize a Servo Port
 *
 * @param[in]  servo       The Servo Port
 * @param[in]  prescaler   The prescaler
 * @param[in]  autoreload  The autoreload
 * @param[in]  initial     The initial position
 */
void servo_init(const ServoPort* servo, u16 prescaler, u16 autoreload, u16 initial);

/**
 * @brief      Control a Servo Port
 *
 * @param[in]  servo     The Servo Port
 * @param[in]  position  The target position
 */
void servo_control(const ServoPort* servo, u16 position);
// HINT: What about a servo_control() that takes the real angle

/**
 * @brief      Initialize a GPIO Port as a PWM output
 *
 * @param[in]  io          The GPIO Port
 * @param[in]  prescaler   The prescaler
 * @param[in]  autoreload  The autoreload
 * @param[in]  initial     The initial output compare
 */
void pwm_io_init(const PwmIOPort* io, u16 prescaler, u16 autoreload, u16 initial);

/**
 * @brief      Control the PWM of a gpio port
 *
 * @param[in]  io    The GPIO Port
 * @param[in]  duty  The duty cycle / output compare
 */
void pwm_io_control(const PwmIOPort* io, u16 duty);

extern const ServoPort __SERVO1, __SERVO2, __SERVO3;
extern const MotorPort __MOTOR1, __MOTOR2, __MOTOR3;
extern const PwmIOPort __PWM_IO1, __PWM_IO2, __PWM_IO3, __PWM_IO4, __PWM_IO8, __PWM_IO7;
#define SERVO1 &__SERVO1
#define SERVO2 &__SERVO2
#define SERVO3 &__SERVO3
#define MOTOR1 &__MOTOR1
#define MOTOR2 &__MOTOR2
#define MOTOR3 &__MOTOR3
#define PWMIO1 &__PWM_IO1
#define PWMIO2 &__PWM_IO2
#define PWMIO3 &__PWM_IO3
#define PWMIO4 &__PWM_IO4
#define PWMIO7 &__PWM_IO7
#define PWMIO8 &__PWM_IO8

#endif
