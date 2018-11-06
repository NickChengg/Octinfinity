#include "ppi.h"

static inline void ppi_dport_init(const PPIPort* ppi) {
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = ppi->dpins;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = ppi->mode == PPITxMode ? GPIO_Mode_Out_PP : GPIO_Mode_IN_FLOATING;
	GPIO_Init(ppi->dport, &GPIO_InitStructure);
}

static inline void ppi_clk_init(const PPIPort* ppi) {
	gpio_init(ppi->clk, GPIO_Mode_Out_PP);
}

void ppi_init(const PPIPort* ppi) {
	ppi_clk_init(ppi);

	ppi_dport_init(ppi);
}

IRQn_Type tim_get_irq(TIM_TypeDef* tim) {
	switch ((u32) tim) {
		case ((u32) TIM1): return TIM1_IRQn;
		case ((u32) TIM2): return TIM2_IRQn;
		case ((u32) TIM3): return TIM3_IRQn;
		case ((u32) TIM4): return TIM4_IRQn;
		case ((u32) TIM5): return TIM5_IRQn;
		case ((u32) TIM8): return TIM8_IRQn;
	}
}

void appi_clk_init(const APPIPort* appi) {
	gpio_init(ppi->clk, GPIO_Mode_AF_PP);

	tim_rcc_init(timer);
	
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_TimeBaseStructure.TIM_CounterMode = appi->base.clk_mode==PPIClockRising ? TIM_CounterMode_CenterAligned1 : TIM_CounterMode_CenterAligned2;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseStructure.TIM_Prescaler = appi->tim_psc;
	TIM_TimeBaseStructure.TIM_Period = 2;
	TIM_TimeBaseInit(timer, &TIM_TimeBaseStructure);

	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;

	switch (appi->tim_ch) {
		case 1:
			TIM_OC1Init(timer, &TIM_OCInitStructure);
			TIM_OC1PreloadConfig(timer, ENABLE);
			break;

		case 2:
			TIM_OC2Init(timer, &TIM_OCInitStructure);
			TIM_OC2PreloadConfig(timer, ENABLE);
			break;

		case 3:
			TIM_OC3Init(timer, &TIM_OCInitStructure);
			TIM_OC3PreloadConfig(timer, ENABLE);
			break;

		case 4:
			TIM_OC4Init(timer, &TIM_OCInitStructure);
			TIM_OC4PreloadConfig(timer, ENABLE);
			break;

	}

	TIM_ARRPreloadConfig(timer, ENABLE);
	TIM_Cmd(timer, ENABLE);	
	TIM_SetCounter(timer,0);
	TIM_CtrlPWMOutputs(timer, ENABLE);

	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = tim_get_irq(appi->tim);
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	TIM_ITConfig(appi->tim, TIM_IT_Update);
}

void appi_init(const APPIPort* appi) {
	appi_clk_init(appi);

	ppi_dport_init((const PPIPort*) appi);
}

void ppi_tx(const PPIPort* ppi, u16 tx) {
	if (ppi->mode != PPITxMode) return;

	gpio_write(ppi->clk, ppi->clk_mode==PPIClockRising);
	
	ppi->dport->ODR = tx & ppi->dpins;
	
	gpio_write(ppi->clk, ppi->clk_mode==PPIClockFalling);
}

u16 ppi_rx(const PPIPort* ppi) {
	if (ppi->mode != PPIRxMode) return;

	gpio_write(ppi->clk, ppi->clk_mode==PPIClockRising);
	
	u16 rx = ppi->dport->IDR & ppi->dpins;
	
	gpio_write(ppi->clk, ppi->clk_mode==PPIClockFalling);
}
