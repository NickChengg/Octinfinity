#include "spi.h"
#include "rcc.h"

SPIPort TFT_SPI = {
	SPI2, &PB13, &PB14, &PB15, 0, 0, 0
};

IRQn_Type spi_get_irq(SPI_TypeDef* spi) {
	switch ((u32)spi) {
		default:
		case ((u32)SPI1): return SPI1_IRQn;
		case ((u32)SPI2): return SPI2_IRQn;
		// case ((u32)SPI3): return SPI3_IRQn;
	}
}

void spi_init(SPIPort* spi, u16 speed, u16 direction, u16 mode, u16 datasize) {
	//TODO: RCC Init
	SPI_InitTypeDef SPI_InitStructure;

	spi_rcc_init(spi->periph);

	gpio_init(spi->sclk, GPIO_Mode_AF_PP);

	if (direction & (SPI_Direction_2Lines_FullDuplex | SPI_Direction_1Line_Rx)) 
		gpio_init(mode == SPI_Mode_Master ? spi->miso : spi->mosi, GPIO_Mode_AF_PP);
	if (direction & (SPI_Direction_2Lines_FullDuplex | SPI_Direction_1Line_Tx)) 
		gpio_init(mode == SPI_Mode_Master ? spi->mosi : spi->miso, GPIO_Mode_AF_PP);

	if (IS_GPIO_REMAP(spi->remap)) GPIO_PinRemapConfig(spi->remap, ENABLE);

	SPI_InitStructure.SPI_Direction = direction;
	SPI_InitStructure.SPI_Mode = mode;
	SPI_InitStructure.SPI_DataSize = datasize;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = speed;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB; 
	SPI_Init(spi->periph, &SPI_InitStructure);
	SPI_Cmd(spi->periph, ENABLE);
	SPI_CalculateCRC(spi->periph, DISABLE);		// Disable the CRC checking
	SPI_SSOutputCmd(spi->periph, DISABLE);
}

u16 spi_transfer(SPIPort* spi, u16 data) {
	if (spi->tx || spi->rx) return 0;

	while (SPI_I2S_GetFlagStatus(spi->periph, SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(spi->periph, data);

	while (SPI_I2S_GetFlagStatus(spi->periph, SPI_I2S_FLAG_RXNE) == RESET);
	return SPI_I2S_ReceiveData(spi->periph);
}

void spi_tx(SPIPort* spi, u16 data) {
	while (SPI_I2S_GetFlagStatus(spi->periph, SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(spi->periph, data);
}

u16 spi_rx(SPIPort* spi) {
	while (SPI_I2S_GetFlagStatus(spi->periph, SPI_I2S_FLAG_RXNE) == RESET);
	return SPI_I2S_ReceiveData(spi->periph);
}

void spi_it_init(SPIPort* spi) {
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = spi_get_irq(spi->periph);
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

u8 spi_it_start(SPIPort* spi, SPIOnReceiveListener listener, SPITransmissionHandler handler) {
	if (spi->tx || spi->rx) return 0;

	spi->tx = handler;
	spi->rx = listener;
	SPI_I2S_ITConfig(spi->periph, SPI_I2S_FLAG_TXE|SPI_I2S_FLAG_RXNE, ENABLE);

	return 1;
}
void spi_it_stop(SPIPort* spi) {
	SPI_I2S_ITConfig(spi->periph, SPI_I2S_FLAG_TXE|SPI_I2S_FLAG_RXNE, DISABLE);
	spi->tx = 0;
	spi->rx = 0;
}
u8 spi_it_tx_start(SPIPort* spi, SPITransmissionHandler handler) {
	if (spi->tx) return 0;

	spi->tx = handler;
	SPI_I2S_ITConfig(spi->periph, SPI_I2S_FLAG_TXE, ENABLE);

	return 0;
}
void spi_it_tx_stop(SPIPort* spi) {
	SPI_I2S_ITConfig(spi->periph, SPI_I2S_FLAG_TXE, DISABLE);
	spi->tx = 0;
}
u8 spi_it_rx_start(SPIPort* spi, SPIOnReceiveListener listener) {
	if (spi->rx) return 0;

	spi->rx = listener;
	SPI_I2S_ITConfig(spi->periph, SPI_I2S_FLAG_RXNE, ENABLE);

	return 1;
}
void spi_it_rx_stop(SPIPort* spi) {
	SPI_I2S_ITConfig(spi->periph, SPI_I2S_FLAG_RXNE, DISABLE);
	spi->rx = 0;
}

static void spi_irq_handler(SPIPort* spi) {
	if (SPI_I2S_GetITStatus(spi->periph, SPI_I2S_IT_RXNE) != RESET) {
		u16 data = SPI_I2S_ReceiveData(spi->periph);
		if (spi->rx) spi->rx(data);

		SPI_I2S_ClearITPendingBit(spi->periph, SPI_I2S_IT_RXNE);
	} else if (SPI_I2S_GetITStatus(spi->periph, SPI_I2S_IT_TXE) != RESET) {
		if (spi->tx) spi->tx(spi->periph);
		
		SPI_I2S_ClearITPendingBit(spi->periph, SPI_I2S_IT_TXE);
	}
}

void SPI2_IRQHandler(void) {
	spi_irq_handler(&TFT_SPI);
}
