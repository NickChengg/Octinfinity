#include "adc.h"

#include "rcc.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"

static u16 ADC_READINGS[16] = {0};
u8 NO_OF_ADC_PORTS = 0;
static ADCPort* adc_ports[16] = {0};

ADCPort ADC_CH1 = {GPIO1, 15, ADC_Channel_0};
ADCPort ADC_CH2 = {GPIO2, 15, ADC_Channel_1};
ADCPort ADC_CH3 = {GPIO3, 15, ADC_Channel_2};
ADCPort ADC_CH4 = {GPIO4, 15, ADC_Channel_3};
ADCPort ADC_CH5 = {GPIO5, 15, ADC_Channel_4};
ADCPort ADC_CH6 = {GPIO6, 15, ADC_Channel_5};
ADCPort ADC_CH7 = {GPIO7, 15, ADC_Channel_6};
ADCPort ADC_CH8 = {GPIO8, 15, ADC_Channel_7};

ADCPort ADC_TEMP = {0, 0, ADC_Channel_TempSensor};
ADCPort ADC_VREF = {0, 0, ADC_Channel_Vrefint};

#define ADC_USING_DMA
//#undef ADC_USING_INT

static void adc_dma_init(void);
static void adc_it_init(void);
/**
 * @brief      Initialize an ADC Channel
 *
 * @param      adc_port  The ADC Port
 */
void adc_channel_init(ADCPort* adc_port) {
	adc_port->index = NO_OF_ADC_PORTS++;
	if(adc_port->pin) gpio_init(adc_port->pin, GPIO_Mode_AIN);
	//ADC_RegularChannelConfig(ADC1, adc_port->channel, adc_port->index+1, ADC_SampleTime_239Cycles5);
	// (SampleTime[239.5] + Tconv[12.5])/ADCCLK[72MHz/6] = 21us -> 47.62kHz (/NO_OF_ADC_PORTS)
	adc_ports[adc_port->index] = adc_port;
}


/**
 * @brief      Initialize the Whole ADC
 * @note       IMPORTANT: must be called after all the channels are initialized
 */
void adc_init() {
	adc_rcc_init(ADC1);

	ADC_InitTypeDef ADC_InitStructure;
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = NO_OF_ADC_PORTS;
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);

	adc_dma_init();
	adc_it_init();

// (SampleTime[239.5] + Tconv[12.5])/ADCCLK[72MHz/6] = 21us -> 47.62kHz (/NO_OF_ADC_PORTS)
	for (u8 i=0; i<NO_OF_ADC_PORTS; i++) {
		ADC_RegularChannelConfig(ADC1, adc_ports[i]->channel, adc_ports[i]->index+1, ADC_SampleTime_239Cycles5);
	}

	ADC_Init(ADC1, &ADC_InitStructure);
	ADC_DMACmd(ADC1, ENABLE);
	ADC_Cmd(ADC1, ENABLE);

	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1));
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));
	
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);

}

/**
 * @brief      Gets the latest ADC reading corresponding to the given ADC Port
 *
 * @param      adc   The ADC Port
 *
 * @return     The ADC Reading
 */
u16 get_adc(ADCPort* adc) {
	return ADC_READINGS[adc->index];
}

//Advanced: DMA and Interrupts

/**
 * @brief      Initialize the DMA Channel for the ADC 
 * 			to automatically read the measurement into the ADC_READINGS buffer
 */	
static void adc_dma_init(void) {
	#ifdef ADC_USING_DMA

	dma_rcc_init(DMA1);

	DMA_InitTypeDef DMA_InitStructure;
	DMA_DeInit(DMA1_Channel1);
	
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = NO_OF_ADC_PORTS;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&ADC_READINGS[0];
	
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);
	
	DMA_Cmd(DMA1_Channel1,ENABLE);
	//ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);

	#endif
}

/**
 * @brief      Initialize the ADC Conversion Interrupt
 * 				Will cause an interrupt to be called after each sequence of conversions
 * 				One sequence means each channels is measured once
 */
static void adc_it_init(void) {
	#if ADC_USING_INT

	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

	#ifdef ADC_USING_DMA
		NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
		NVIC_Init(&NVIC_InitStructure);
		DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);
	#else
		NVIC_InitStructure.NVIC_IRQChannel = ADC1_2_IRQn;
		NVIC_Init(&NVIC_InitStructure);
		ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);
	#endif

	#endif
}

static u8 adc_converted_channel = 0;
#define ADC_READING (ADC_READINGS[adc_converted_channel])

void ADC1_2_IRQHandler(void) {
	if (ADC_GetITStatus(ADC1, ADC_IT_EOC)) {
		ADC_READING = ADC_GetConversionValue(ADC1); // This is basically what the DMA does lol

		adc_converted_channel++;
		ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
	}
}

void DMA1_Channel1_IRQHandler(void) {
	if (DMA_GetITStatus(DMA1_IT_TC1)) {
		adc_converted_channel++;
		DMA_ClearITPendingBit(DMA1_IT_TC1);
	}
}
