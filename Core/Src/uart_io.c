#include <string.h>
#include <stdio.h>
#include "uart_io.h"
#include "vars_it.h"
// Temporary
#include "tmp1075.h"

#define WAIT_USART_RX_TIME 10

// Creating the instance of UART structure 
sUART sUART3 = {0};


// UART variables
char rxDataUART[10] = {0};
char txDataUART[10] = {0};

uint8_t rxDataItUART[70] = {0};
uint8_t txDataItUART[10] = {0};
uint16_t aBufferSizeUART = 10;




void usartRx(uint8_t* rxdt, uint8_t* txdt, uint16_t sz)
{
		uint16_t ind = 0;
		uint8_t* prx = rxdt;
		uint8_t* ptx = txdt;
		uint16_t siz = sz - 1;
		char endBuf[2] = {'\r','\n'};
		uint16_t endInd = 0;
	
		// Prevent cleaning receave buffer
		for (ind = 0; ind < siz; ind++)
			*(rxdt + ind) = 0;
	
		ind = 0;
		// Character by character reading from the UART input
		while (++ind < siz)
		{
				while (!LL_USART_IsActiveFlag_RXNE(USART3)) {}
				*prx = LL_USART_ReceiveData8(USART3);									//On this step "P" is coming (1)
					
				if (*prx == '\r' || *prx == '\n')
				{
						ptx++;
						break;
				}
				
				*ptx = *prx;
				while (!LL_USART_IsActiveFlag_TXE(USART3)) {}
				LL_USART_TransmitData8(USART3,*(uint8_t*)ptx);
				ptx++;
				prx++;
		}
		
		// The transmit line to be added the end line characters
		--ptx;
		while (endInd < 2)
		{
				*ptx = endBuf[endInd];
				while (!LL_USART_IsActiveFlag_TXE(USART3)) {}
				LL_USART_TransmitData8(USART3,*(uint8_t*)ptx);
				ptx++;
				endInd++;
		}
		
		// Cleaning the transmit buffer
		while (ind)
		{						
				*(txdt + ind) = 0;
				--ind;
		}
}


void handlerUsartRxIT(uint8_t echo)
{
	if (echo && txUnblockUsartHandlerFl)
	{
		sprintf((char*)sUART3it.rxData, "%s%s", sUART3it.rxData, "\r\n");
		usartTx(sUART3it.rxData, USART_BUFFER_SIZE);
		// Reset handler unblock flag 
		txUnblockUsartHandlerFl = 0;
	}
}



void  usartTx(uint8_t* txdt, uint16_t sz)
{
    uint16_t ind = 0;
		uint16_t siz = sz - 1;
    while (*(txdt + ind) != '\0' && ind < siz)
    {
        while (!LL_USART_IsActiveFlag_TXE(USART3));
        LL_USART_TransmitData8(USART3, *(uint8_t*)(txdt + ind));
        ++ind;
    }
		while (ind)
		{
				--ind;
				*(txdt + ind) = 0;
		}
}

