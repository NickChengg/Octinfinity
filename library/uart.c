#include "uart.h"

#include "rcc.h"
#include <stdio.h>
#include <stdarg.h>

UARTPort UARTPort1 = {USART1, &PA9, &PA10, 0, 0, 0};
UARTPort UARTPort3 = {USART3, &PC10, &PC11, 0, 0, GPIO_PartialRemap_USART3};

/**
 * @brief      Initialize the given UART Port
 *
 * @param      uart      The UART Port Handle
 * @param[in]  baudrate  The Baudrate
 */
void uart_init(UARTPort* uart, u32 baudrate) {
	uart_rcc_init(uart->periph);

	gpio_init(uart->tx_pin, GPIO_Mode_AF_PP);
	gpio_init(uart->rx_pin, GPIO_Mode_IN_FLOATING);

	if (IS_GPIO_REMAP(uart->remap)) GPIO_PinRemapConfig(uart->remap, ENABLE);

	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = baudrate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_Init(uart->periph, &USART_InitStructure);
	USART_Cmd(uart->periph, ENABLE);
}

/**
 * @brief      Get the IRQn_Type for the given UART/USART Peripheral to
 *             initialize NVIC
 *
 * @param      uart  The UART/USART Peripheral
 *
 * @return     The IRQn_Type
 */
static IRQn_Type uart_get_irq(USART_TypeDef* uart) {
	switch ((u32)uart) {
		default:
		case ((u32) USART1): return USART1_IRQn;
		case ((u32) USART2): return USART2_IRQn;
		case ((u32) USART3): return USART3_IRQn;
		case ((u32) UART4):  return UART4_IRQn;
		case ((u32) UART5):  return UART5_IRQn;
	}
}

/**
 * @brief      Register an RX Listener for a UART Port
 * @note       just assigns the function pointer to uart->rx, hence inline
 *
 * @param      uart      The UART Port Handle
 * @param[in]  listener  The RX Listener function pointer
 */
inline void uart_reg_listener(UARTPort* uart, UARTOnReceiveListener listener) {
	uart->rx = listener;
}

/**
 * @brief      Initialize the UART RX Interrupt and register a listener
 *
 * @param      uart      The UART Port Handle
 * @param[in]  listener  The RX Listener function pointer
 */
void uart_rx_init(UARTPort* uart, UARTOnReceiveListener listener) {
	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannel = uart_get_irq(uart->periph);
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	USART_ITConfig(uart->periph, USART_IT_RXNE, ENABLE);

	uart_reg_listener(uart, listener);
}


/**
 * @brief      Transmit a byte from the given UART Port
 *
 * @param      uart  The UART Port Handle
 * @param[in]  byte  The byte
 *
 * @note       will silently fail is UART TX is already busy (in interrupt mode)
 */
void uart_tx_byte(UARTPort* uart, u8 byte) {
	if (uart->tx) return;
	 
	while (USART_GetFlagStatus(uart->periph, USART_FLAG_TC) == RESET);
	USART_SendData(uart->periph, (u16)byte);
}

/**
 * @brief      Transmit a string from the given UART Port formatted using sprint
 * @note       Is a blocking transmission, may take time if string is long or
 *             baudrate is low Consider implementing non blocking transmission
 *             using the TX interrupts
 *
 * @param      uart       The UART Port Handle
 * @param[in]  fmt        The Format String (see sprintf)
 * @param[in]  <unnamed>  { parameter_description }
 * @param[in]  ...   The sprintf parameters
 *
 * @note       will silently fail is UART TX is already busy (in interrupt mode)
 */
void uart_tx_str(UARTPort* uart, const char* fmt, ...) {
	if (uart->tx) return;

	va_list arglist;
	char buf[255], *fp;
	
	va_start(arglist, fmt);
	vsprintf(buf, fmt, arglist);
	va_end(arglist);
	
	fp = buf;
	while (*fp)
		uart_tx_byte(uart, *fp++);
}

/**
 * @brief      Initialize the UART TX Interrupt
 *
 * @param      uart  The UART Port Handle
 */
void uart_tx_it_init(UARTPort* uart) {
	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannel = uart_get_irq(uart->periph);
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/**
 * @brief      Start non-blocking (interrupt based) TX from the given UART Port
 *
 * @param      uart     The UART Port Handle
 * @param[in]  handler  The Transmission Handler
 *
 * @return     0 if UART TX was already busy, 1 otherwise
 */
u8 uart_tx_it_start(UARTPort* uart, UARTTransmissionHandler handler) {
	if (uart->tx) return 0;

	uart->tx = handler;
	USART_ITConfig(uart->periph, USART_IT_TXE, ENABLE);	

	return 1;
}

/**
 * @brief      Stop the TX interrupts from the given UART Port
 *
 * @param      uart  The UART Port Handle
 */
void uart_tx_it_stop(UARTPort* uart) {
	USART_ITConfig(uart->periph, USART_IT_TXE, DISABLE);
	uart->tx = 0;
}

/**
 * @brief      General IRQ Handler for a UART Port
 *
 * @param      uart  The UART Port
 */
static void uart_irq_handler(UARTPort* uart) {
	if (USART_GetITStatus(uart->periph, USART_IT_RXNE) != RESET) {
		u16 byte = USART_ReceiveData(uart->periph);
		if (uart->rx) uart->rx(byte);

		USART_ClearITPendingBit(uart->periph, USART_IT_RXNE);
	} else if (USART_GetITStatus(uart->periph, USART_IT_TXE) != RESET) {
		if (uart->tx) uart->tx();
		
		USART_ClearITPendingBit(uart->periph, USART_IT_TXE);
	}
}

/**
 * @brief      Raw USART/UART IRQ Handlers
 */
void USART1_IRQHandler(void) {
	uart_irq_handler(&UARTPort1);
}
void USART3_IRQHandler(void) {
	uart_irq_handler(&UARTPort3);
}
