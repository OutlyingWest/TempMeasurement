#include <stdint.h>

#define USART_BUFFER_SIZE 70

typedef struct
{
	uint8_t rxData[USART_BUFFER_SIZE];
	uint8_t txData[USART_BUFFER_SIZE];
}sUARTit;
