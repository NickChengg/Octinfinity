#ifndef __SPI_H
#define __SPI_H

#include "gpio.h"

#include "stm32f10x_spi.h"

typedef void (*SPIOnReceiveListener)(const uint8_t byte);
typedef void (*SPITransmissionHandler)(SPI_TypeDef* spi);

typedef struct {
	SPI_TypeDef* periph;
	const GPIOPin* sclk;
	const GPIOPin* miso;
	const GPIOPin* mosi;
	u32 remap;
	SPIOnReceiveListener rx;
	SPITransmissionHandler tx;
} SPIPort;

extern SPIPort TFT_SPI;

/**
 * @brief      Initialize a given SPI Port with the given parameters
 *
 * @param[in]  spi        The SPI Port
 * @param[in]  speed      The Baudrate Prescaler for the SPI
 * @param[in]  direction  TX/RX?/Full Duplex
 * @param[in]  mode       Master/Slave
 * @param[in]  datasize   8/16bit
 *
 * @note       see stm32f103xx_spi.h/.c for more information about the below
 */
void spi_init(SPIPort* spi, u16 speed, u16 direction, u16 mode, u16 datasize);

/**
 * @brief      Perform a single SPI Transfer (Transmit+Receive)
 *
 * @param[in]  spi   The SPI Port
 * @param[in]  data  The data to transmit
 *
 * @return     The Data Received
 */
u16 spi_transfer(SPIPort* spi, u16 data);

/**
 * @brief      Perform a single SPI Transmission 
 *
 * @param      spi   The SPI Port
 * @param[in]  data  The data to transmit
 */
void spi_tx(SPIPort* spi, u16 data);

/**
 * @brief      Perform a single SPI Reception
 *
 * @param      spi   The SPI Port
 *
 * @return     The data received
 * 
 * @note not very useful, will block indefinitely if it doesnt receive anything
 */
u16 spi_rx(SPIPort* spi);

/**
 * @brief      Initialize the interrupt handler for the given SPI Port
 *
 * @param[in]  spi   The SPI Port
 */
void spi_it_init(SPIPort* spi);

/**
 * @brief      Start a sequence simultaneous transmission and reception through
 *             interrupts
 *
 * @param[in]  spi       The SPI Port
 * @param[in]  listener  The RX Listener function pointer
 * @param[in]  handler   The TX Handler function pointer
 *
 * @return     0 if SPI TX/RX was already busy, 1 otherwise
 */
u8 spi_it_start(SPIPort* spi, SPIOnReceiveListener listener, SPITransmissionHandler handler);

/**
 * @brief      Stop the SPI Interrupt Sequence
 *
 * @param[in]  spi   The SPI Port
 */
void spi_it_stop(SPIPort* spi);

/**
 * @brief      Start a sequence of transmissions through interrupts
 *
 * @param[in]  spi      The SPI Port
 * @param[in]  handler  The TX Handler function pointer
 *
 * @return     0 if SPI TX was already busy, 1 otherwise
 */
u8 spi_it_tx_start(SPIPort* spi, SPITransmissionHandler handler);

/**
 * @brief      Stop a sequence of transmissions through interrupts
 *
 * @param[in]  spi   The SPI Port
 */
void spi_it_tx_stop(SPIPort* spi);

/**
 * @brief      Start a sequence of receptions through interrupts
 * @note       this function will not initiate a transfer, use this in Slave
 *             mode or when a transfer has been initiated separately
 *
 * @param[in]  spi       The SPI Port
 * @param[in]  listener  The RX Listener function pointer
 *
 * @return     0 if SPI RX was already busy, 1 otherwise
 */
u8 spi_it_rx_start(SPIPort* spi, SPIOnReceiveListener listener);

/**
 * @brief      Stop the sequence of receptions through interrupts
 *
 * @param[in]  spi   The SPI Port
 */
void spi_it_rx_stop(SPIPort* spi);

#endif
