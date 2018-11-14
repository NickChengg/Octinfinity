#include "gpio.h"

// Look in the header file for the functions

//EXTI0_IRQHandler
//EXTI1_IRQHandler
//EXTI2_IRQHandler
//EXTI3_IRQHandler
//EXTI4_IRQHandler
__weak void EXTI5_IRQHandler(void) {}
__weak void EXTI6_IRQHandler(void) {}
__weak void EXTI7_IRQHandler(void) {}
__weak void EXTI8_IRQHandler(void) {}
__weak void EXTI9_IRQHandler(void) {}
__weak void EXTI10_IRQHandler(void) {}
__weak void EXTI11_IRQHandler(void) {}
__weak void EXTI12_IRQHandler(void) {}
__weak void EXTI13_IRQHandler(void) {}
__weak void EXTI14_IRQHandler(void) {}
__weak void EXTI15_IRQHandler(void) {}

void gpio_exti_init(const GPIOPin* gpio, EXTITrigger_TypeDef trigger) {
	IRQn_Type exti_grp;
	switch (gpio->pin) {
		case GPIO_Pin_0: exti_grp = EXTI0_IRQn; break;
		case GPIO_Pin_1: exti_grp = EXTI0_IRQn; break;
		case GPIO_Pin_2: exti_grp = EXTI0_IRQn; break;
		case GPIO_Pin_3: exti_grp = EXTI0_IRQn; break;
		case GPIO_Pin_4: exti_grp = EXTI0_IRQn; break;
		case GPIO_Pin_5: 
		case GPIO_Pin_6: 
		case GPIO_Pin_7: 
		case GPIO_Pin_8: 
		case GPIO_Pin_9: exti_grp = EXTI9_5_IRQn; break;
		case GPIO_Pin_10: 
		case GPIO_Pin_11: 
		case GPIO_Pin_12: 
		case GPIO_Pin_13: 
		case GPIO_Pin_14: 
		case GPIO_Pin_15: exti_grp = EXTI15_10_IRQn; break;
	};
	u8 port_source;
	switch ((u32) gpio->port) {
		case GPIOA_BASE: port_source = GPIO_PortSourceGPIOA; break;
		case GPIOB_BASE: port_source = GPIO_PortSourceGPIOB; break;
		case GPIOC_BASE: port_source = GPIO_PortSourceGPIOC; break;
		case GPIOD_BASE: port_source = GPIO_PortSourceGPIOD; break;
	};
	u8 pin_source = 0;
	u16 pin = gpio->pin;
	do {
		if (pin & 0x01) break;
		pin >>=1;
		pin_source++;
	} while (1);

	//	NVIC
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	NVIC_InitStructure.NVIC_IRQChannel = exti_grp;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	//	EXTI
	GPIO_EXTILineConfig(port_source, pin_source);
	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_InitStructure.EXTI_Line = gpio->pin;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = trigger; 
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
}

void EXTI9_5_IRQHandler(void) {
	if ((EXTI->PR & EXTI_Line5) != RESET)
	{
		EXTI5_IRQHandler();
		EXTI_ClearITPendingBit(EXTI_Line5);
	}
	if ((EXTI->PR & EXTI_Line6) != RESET)
	{
		EXTI6_IRQHandler();
		EXTI_ClearITPendingBit(EXTI_Line6);
	}
	if ((EXTI->PR & EXTI_Line7) != RESET)
	{
		EXTI7_IRQHandler();
		EXTI_ClearITPendingBit(EXTI_Line7);
	}
	if ((EXTI->PR & EXTI_Line8) != RESET)
	{
		EXTI8_IRQHandler();
		EXTI_ClearITPendingBit(EXTI_Line8);
	}
	if ((EXTI->PR & EXTI_Line9) != RESET)
	{
		EXTI9_IRQHandler();
		EXTI_ClearITPendingBit(EXTI_Line9);
	}
	if ((EXTI->PR & EXTI_Line10) != RESET)
	{
		EXTI10_IRQHandler();
		EXTI_ClearITPendingBit(EXTI_Line10);
	}
	if ((EXTI->PR & EXTI_Line11) != RESET)
	{
		EXTI11_IRQHandler();
		EXTI_ClearITPendingBit(EXTI_Line11);
	}
}

#include "leds.h"
void EXTI15_10_IRQHandler(void)
{
	if ((EXTI->PR & EXTI_Line12) != RESET)
	{
		EXTI12_IRQHandler();
		EXTI_ClearITPendingBit(EXTI_Line12);
	}
	if ((EXTI->PR & EXTI_Line13) != RESET)
	{
		EXTI13_IRQHandler();
		EXTI_ClearITPendingBit(EXTI_Line13);
	}
	if ((EXTI->PR & EXTI_Line14) != RESET)
	{
		EXTI14_IRQHandler();
		EXTI_ClearITPendingBit(EXTI_Line14);
	}
	if ((EXTI->PR & EXTI_Line15) != RESET)
	{
		EXTI15_IRQHandler();
		EXTI_ClearITPendingBit(EXTI_Line15);
	}
}
