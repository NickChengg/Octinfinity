#include "ticks.h"

volatile u32 msec = 0;
volatile u32 delay_time = 0;
volatile u8 tick_tock = 0;

/**
  * @brief  Initialization of ticks timer
  * @param  None
  * @retval None
  */
void ticks_init(void) {
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannel = SysTick_IRQn; //Ignore this warning
	NVIC_Init(&NVIC_InitStructure);
	
	SysTick_Config(SystemCoreClock / 1000); 

	msec = 0;
}

/**
  * @brief  SysTick interrupt handler
  * @param  None
  * @retval None
  */
void SysTick_Handler(void){
	msec++;
	delay_time--;
	tick_tock=0;

	//Timing critical functions can be placed here
}

/**
  * @brief  Reset the ticks timer
  * @param  None
  * @retval None
  */
void ticks_reset(void) {
	msec = 0;
}

/**
  * @brief  Return no of milliseconds passed
  * @param  None
  * @retval Milliseconds passed
  */
u32 get_ticks(void) {
	return msec;
}

u32 get_subticks(void) {
  return SysTick->VAL;
}

/**
  * @brief  Wait fot the time specified by the user
  * @param  ms Amount of time in ms
  * @retval None
  */
void delay(u32 ms){
	delay_time = ms;
	while (delay_time); //the magic of interrupts will decrement this variable
}

/**
 * @brief      Helper for making the main 1ms loop, 
 * 			Just insert this at the beginning of a forever loop and the body will run every ms
 */		
void ms_loop_handler() {
	while (tick_tock); //the magic of interrupts will reset this variable every 1ms
	tick_tock = 1;
}
