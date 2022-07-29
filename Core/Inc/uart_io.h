#include "stm32f3xx_ll_usart.h"

#define UART_BUFFER_SIZE 70

// UART variables
extern char rxDataUART[10];
extern char txDataUART[10];
extern uint16_t aBufferSizeUART;

// Refactor the UART vars
typedef struct
{
	char RxData[UART_BUFFER_SIZE];
	char TxData[UART_BUFFER_SIZE];
}sUART;

//extern sUART;
extern sUART sUART3;


// Function prototypes
void usartRx(uint8_t* rxdt,		// Pointer to receve data buffer
							uint8_t* txdt,		// Pointer to transmit data buffer
							uint16_t sz);			// Size of both TX and RX data buffers


void  usartTx(uint8_t* txdt,		// Pointer to transmit data buffer
							 uint16_t sz);		// Size of TX data buffer


// This function working without stop the main loop
void usartRxCont(uint8_t* rxdt,
								 uint8_t* txdt,
								 uint16_t sz);

void handlerUsartRxIT(void);
