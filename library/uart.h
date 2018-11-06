#ifndef __UART_H
#define __UART_H

#include "gpio.h"
#include "stm32f10x_usart.h"

typedef void (*UARTOnReceiveListener)(const uint8_t byte);
typedef void (*UARTTransmissionHandler)(void);

typedef struct {
	USART_TypeDef* periph;
	const GPIOPin* tx_pin;
	const GPIOPin* rx_pin;
	UARTOnReceiveListener rx;
	UARTTransmissionHandler tx;
	u32 remap;
} UARTPort;

extern UARTPort UARTPort1, UARTPort3;
#define COM1 &UARTPort1
#define COM3 &UARTPort3

/**
 * @brief      Initialize the given UART Port
 *
 * @param      uart      The UART Port Handle
 * @param[in]  baudrate  The Baudrate
 */
void uart_init(UARTPort* uart, u32 baudrate);

/**
 * @brief      Register an RX Listener for a UART Port
 * @note just assigns the function pointer to uart->rx, hence inline
 *
 * @param      uart      The UART Port Handle
 * @param[in]  listener  The RX Listener function pointer
 */
inline void uart_reg_listener(UARTPort* uart, UARTOnReceiveListener listener);

/**
 * @brief      Initialize the UART RX Interrupt and register a listener
 *
 * @param      uart      The UART Port Handle
 * @param[in]  listener  The RX Listener function pointer
 */
void uart_rx_init(UARTPort* uart, UARTOnReceiveListener listener);

/**
 * @brief      Transmit a byte from the given UART Port
 *
 * @param      uart  The UART Port Handle
 * @param[in]  byte  The byte
 * 
 * @note will silently fail is UART TX is already busy (in interrupt mode)
 */
void uart_tx_byte(UARTPort* uart, u8 byte);

/**
 * @brief      Transmit a string from the given UART Port formatted using sprint
 * @note Is a blocking transmission, may take time if string is long or baudrate is low
 * 			Consider implementing non blocking transmission using the TX interrupts
 *
 * @param      uart       The UART Port Handle
 * @param[in]  fmt        The Format String (see sprintf)
 * @param[in]  ...        The sprintf parameters
 * 
 * @note will silently fail is UART TX is already busy (in interrupt mode)
 */
void uart_tx_str(UARTPort* uart, const char* fmt, ...);

/**
 * @brief      Initialize the UART TX Interrupt
 *
 * @param      uart     The UART Port Handle
 */
void uart_tx_it_init(UARTPort* uart);

/**
 * @brief      Start non-blocking (interrupt based) TX from the given UART Port
 *
 * @param      uart     The UART Port Handle
 * @param[in]  handler  The Transmission Handler
 *
 * @return     0 if UART TX was already busy, 1 otherwise
 */
u8 uart_tx_it_start(UARTPort* uart, UARTTransmissionHandler handler);

/**
 * @brief      Stop the TX interrupts from the given UART Port
 *
 * @param      uart  The UART Port Handle
 */
void uart_tx_it_stop(UARTPort* uart);

#endif
