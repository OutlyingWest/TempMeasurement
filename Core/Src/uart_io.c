#include "uart_io.h"
#include "vars_it.h"

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


void handlerUsartRxIT()
{
	if (rxUnblockUSARTFl)
	{
		static uint8_t nByte = 0;
		
		tim6LastTimeUSART = tim6Tick;
		while (tim6Tick - tim6LastTimeUSART < WAIT_USART_RX_TIME &&
					 nByte < USART_BUFFER_SIZE)
		{
			// If byte is received to read register - write it to the rx buffer 
			if (LL_USART_IsActiveFlag_RXNE(USART3))
			{
				sUART3it.rxData[nByte] = LL_USART_ReceiveData8(USART3);
			}
			//If message fully sended - stop fill the rx buffer
			if (sUART3it.rxData[nByte] == '\r' || sUART3it.rxData[nByte] == '\n')
			{
				//TODO: Invent the algorithm for handling Interrupt included vars below
				//messageIsSendedFl = 1;
				//nByte = 0;
				break;
			}
			nByte++;
		}
		// Reset handler unblock flag 
		rxUnblockUSARTFl = 0;
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

