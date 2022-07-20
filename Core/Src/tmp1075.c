#include <stdio.h>
#include "tmp1075.h"
#include "uart_io.h"
#include "vars_it.h"

//#define ID_CHECK

// Ptr Rx I2C Buffer 
uint8_t *ptrRxBufferI2C;
	
// Ptr Tx I2C Buffer 
uint8_t *ptrTxBufferI2C;

// ID Rx buffer
uint8_t rxBufferID[2] = {0};

// ID Tx Buffer 
uint8_t txBufferID[1] = {0};

// Alert address Rx Buffer 
uint8_t rxAlertAddrI2C[1] = {0};

// Rx I2C Buffer 
uint8_t rxTmpBufferI2C[SIZE_TMP_RX_DATA_BUF] = {0};

// Tx I2C Buffer 
uint8_t txTmpBufferI2C[SIZE_TMP_TX_DATA_BUF] = {0};

// High temperature level. Alert interrupt will generated if temperature exceed this level.
uint8_t highTempLevel = 28;

// Low temperature level. Alert interrupt will generated if temperature cross this level from the top side.
uint8_t lowTempLevel = 28;
	
// Addresses(7-bit) of tmp measurers with align to the left on 1 bit
const uint16_t tmpAddrWithAlign = TMP_1_ADDR << 1;

// Address which sends to over tmps if Alert line adge is low with align
const uint16_t alertResponseAddrWithAlign = ALERT_RESPONSE_ADDR << 1;

// Flag using to enable to get output from tmp1075 (Output enable if 1 and disable if 0)
uint8_t getOutPutFl = 0;     // Must be set for each trying read data from tmp1075


typedef struct 
{
	char AF[5];
	char BERR[7];
	char ARLO[7];
	char TIMEOUT[10];
	char Other[8];
}errorFlags;


errorFlags errorFlag = {
															"AF\r\n", 
															"BERR\r\n",
															"ARLO\r\n",
															"TIMEOUT\r\n",
															"Other\r\n",
											 };

											 
// Error messages
static char errorAlert[MESSAGE_LENGTH] = "Error of transmitting - ";
static char errorMessage[MESSAGE_LENGTH] = {0};
// static char comleteMessage[MESSAGE_LENGTH] = "Transmitting is complete\r\n";
static int errorCode = HAL_I2C_ERROR_NONE;

void aReceiveI2C(I2C_HandleTypeDef hi, uint16_t tmpAddr, uint8_t *aRxBuffer, uint16_t sizeDataBuf, uint32_t timeout, const char *comleteMessage)
{
	// This function receaving data in blocking mode
	if (HAL_I2C_Master_Receive(&hi, (uint16_t)tmpAddr, (uint8_t*)aRxBuffer, (uint16_t)sizeDataBuf, timeout)!= HAL_OK)
	{
		if (HAL_I2C_GetError(&hi) == HAL_I2C_ERROR_AF)
		{
			sprintf(errorMessage, "%s%s", errorAlert, errorFlag.AF);
			usartTx((uint8_t*)errorMessage, MESSAGE_LENGTH);
		}
		else if (errorCode == HAL_I2C_ERROR_BERR)
		{
			sprintf(errorMessage, "%s%s", errorAlert, errorFlag.BERR);
			usartTx((uint8_t*)errorMessage, MESSAGE_LENGTH);
		}
		else if (errorCode == HAL_I2C_ERROR_ARLO)
		{
			sprintf(errorMessage, "%s%s", errorAlert, errorFlag.ARLO);
			usartTx((uint8_t*)errorMessage, MESSAGE_LENGTH);
		}
		else if (errorCode == HAL_I2C_ERROR_TIMEOUT)
		{
			sprintf(errorMessage, "%s%s", errorAlert, errorFlag.TIMEOUT);
			usartTx((uint8_t*)errorMessage, MESSAGE_LENGTH);
		}
		else
		{
			sprintf(errorMessage, "%s%s", errorAlert, errorFlag.Other);
			usartTx((uint8_t*)errorMessage, MESSAGE_LENGTH);
		}
	}
	else
	{
		//usartTx((uint8_t*)comleteMessage, MESSAGE_LENGTH);
	}
}


void aTransmitI2C(I2C_HandleTypeDef hi, uint16_t tmpAddr, uint8_t *aTxBuffer, uint16_t sizeDataBuf, uint32_t timeout, const char *comleteMessage)
{
	// This function transmitting data in blocking mode
	if (HAL_I2C_Master_Transmit(&hi, (uint16_t)tmpAddr,(uint8_t*)aTxBuffer, (uint16_t)sizeDataBuf, timeout) != HAL_OK)
	{
		if ((errorCode = HAL_I2C_GetError(&hi)) == HAL_I2C_ERROR_AF)
		{
			sprintf(errorMessage, "%s%s", errorAlert, errorFlag.AF);
			usartTx((uint8_t*)errorMessage, MESSAGE_LENGTH);
		}
		else if (errorCode == HAL_I2C_ERROR_BERR)
		{
			sprintf(errorMessage, "%s%s", errorAlert, errorFlag.BERR);
			usartTx((uint8_t*)errorMessage, MESSAGE_LENGTH);
		}
		else if (errorCode == HAL_I2C_ERROR_ARLO)
		{
			sprintf(errorMessage, "%s%s", errorAlert, errorFlag.ARLO);
			usartTx((uint8_t*)errorMessage, MESSAGE_LENGTH);
		}
		else if (errorCode == HAL_I2C_ERROR_TIMEOUT)
		{
			sprintf(errorMessage, "%s%s", errorAlert, errorFlag.TIMEOUT);
			usartTx((uint8_t*)errorMessage, MESSAGE_LENGTH);
		}
		else
		{
			sprintf(errorMessage, "%s%s", errorAlert, errorFlag.Other);
			usartTx((uint8_t*)errorMessage, MESSAGE_LENGTH);
		}
	}
	else
	{
		usartTx((uint8_t*)comleteMessage, MESSAGE_LENGTH);
	}
	
}


void initAlertLimits(uint8_t lowLimit, uint8_t highLimit)
{
	uint8_t txBuf[2] = {0};    // Tx I2C Buffer 
	char messageL[MESSAGE_LENGTH] = {0};
	char messageH[MESSAGE_LENGTH] = {0};

	//setRegAddr((uint8_t)LLIM_REG_ADDR);
	txBuf[0] = LLIM_REG_ADDR;
	
	// Set low limit level
	txBuf[1] = lowLimit;
	

	sprintf(messageL, "Low limit set (%d'C).\r\n", lowLimit); 
	aTransmitI2C(hi2c1, (uint16_t)tmpAddrWithAlign, (uint8_t*)txBuf, (uint16_t)SIZE_TMP_TX_DATA_BUF, TRANSMIT_TIMEOUT, messageL);
	
	//setRegAddr((uint8_t)HLIM_REG_ADDR);
	txBuf[0] = HLIM_REG_ADDR;
	
	// Set high limit level
	txBuf[1] = highLimit;
	
	sprintf(messageH, "High limit set (%d'C).\r\n\n", highLimit);
	aTransmitI2C(hi2c1, (uint16_t)tmpAddrWithAlign, (uint8_t*)txBuf, (uint16_t)SIZE_TMP_TX_DATA_BUF, TRANSMIT_TIMEOUT, messageH);
}


void alertTmpInitIT(void)
{
	uint8_t txBuf[2] = {0};    // Tx I2C Buffer 

		
	// Set the address of the configurate register.
	txBuf[0] = CFGR_REG_ADDR;
	
	// Set alert in interrupt mod
	txBuf[1] |= SET_ALERT_IT_MOD;
	
	const char message[] = "IT init completed.\r\n";
	aTransmitI2C(hi2c1, (uint16_t)tmpAddrWithAlign, (uint8_t*)txBuf, (uint16_t)SIZE_TMP_TX_DATA_BUF, TRANSMIT_TIMEOUT, message);
}



float convTemp(uint8_t* rxTmpBufferI2C)
{
	// Function does the conversion of received data from tmp1075 to a decimal form
	// and filling the temperature structure feilds  by them

	/*uint8_t highByte = rxBufferI2C[0];
	uint8_t lowByte = rxBufferI2C[1];*/
	uint16_t tempUint16 = (rxTmpBufferI2C[0] << 4) + (rxTmpBufferI2C[1] >> 4);
	//int32_t tempValInt32 = 0;
	float tempFlt32 = 0;
	float intPart = 0;
	float fractPart = 0;
	int8_t sign = 0;

	sign = tempUint16 >> 11;
	// If sign == 1 True, then temperature is negative, else it's positive 
	if (sign)
	{
		// Converse result to the direct code
		tempUint16 = ~(tempUint16 - 1) & 0x0FFF;
		intPart = (float)(tempUint16 >> 4);
		
		// Converse fractional part of number (in uint16_t format yet) to the true fractional form (in float32)
		fractPart = (float)(tempUint16 & 0x000F) / 16;
		tempFlt32 = -1 * (intPart + fractPart);
	}
	else
	{
		intPart = (float)(tempUint16 >> 4);
		fractPart = (float)(tempUint16 & 0x000F) / 16;
		tempFlt32 = intPart + fractPart;
	}
	
	return tempFlt32;
}


void getTemperature()
{	
	// If Alert interrupt is occured, temperature receiving is blocked 
	if (!interruptAlertOccuredFl)
	{
		if ((tim6Tick - tim6LastTime) > GET_TEMP_TIME)
		{
			tim6LastTime = tim6Tick;
			char tempMessage[MESSAGE_LENGTH] = { 0 };

			// Set the address of the temperature register. Must be set in first transmitted byte (in txBufferI2C[0])
			txTmpBufferI2C[0] = TEMP_REG_ADDR;

			// Enable output
			getOutPutFl = 1;
			ptrRxBufferI2C = rxTmpBufferI2C;
			ptrTxBufferI2C = txTmpBufferI2C;
			// Transmit the reqest for receive temperature
			HAL_I2C_Master_Transmit_IT(&hi2c1, (uint16_t)tmpAddrWithAlign, (uint8_t*)ptrTxBufferI2C, (uint16_t)SIZE_TMP_TX_DATA_BUF);

			// Perform the conversion and filling the temperature structure feilds
			// and display on terminal
			sprintf(tempMessage, "Temp: %.2f'C\r", convTemp(ptrRxBufferI2C));
			usartTx((uint8_t*)tempMessage, MESSAGE_LENGTH);

			#ifdef ID_CHECK
			// Enable output
			getOutPutFl = 1;
				
			// Set the address of the ID register.
			txBufferID[0] = DIED_REG_ADDR;
			
			ptrRxBufferI2C = rxBufferID;
			ptrTxBufferI2C = txBufferID;
			// Transmit the reqest for receive ID
			HAL_I2C_Master_Transmit_IT(&hi2c1, (uint16_t)tmpAddrWithAlign, (uint8_t*)ptrTxBufferI2C, (uint16_t)SIZE_TMP_TX_DATA_BUF);

			// Display on terminal
			char messageID[MESSAGE_LENGTH] = { 0 };
			sprintf(messageID, "\n %x,%x\r\n",rxBufferID[0], rxBufferID[1]);
			usartTx((uint8_t*)messageID, MESSAGE_LENGTH);
			#endif // ID_CHECK
		}
	}
}


void handlerAlertIT()
{	
	//  If Alert interrupt is occured, handler is unblocked
	if (interruptAlertOccuredFl)
	{
		NVIC_DisableIRQ(I2C1_ER_IRQn);
		NVIC_DisableIRQ(I2C1_EV_IRQn);
		char message[MESSAGE_LENGTH] = {0};
		static uint8_t eventFl = 0;
		static uint8_t firstStartFl = 1; 
		rxAlertAddrI2C[0] = 0;
		
		// TODO: add the init for more tmps and solve the problem with current tmp address - 0. But other works!!!
		 HAL_Delay(1000);
		//HAL_I2C_Master_Receive_IT(&hi2c1, (uint16_t)alertResponseAddrWithAlign, (uint8_t*)rxAlertAddrI2C, (uint16_t)1);
		HAL_I2C_Master_Receive(&hi2c1, (uint16_t)alertResponseAddrWithAlign, (uint8_t*)rxAlertAddrI2C, (uint16_t)1, 1000);

		if (!firstStartFl)
		{
			eventFl ^= 1;
			sprintf(message, "\r\n\nAlert addr: %x\r\n", rxAlertAddrI2C[0]);
			usartTx((uint8_t*)message, MESSAGE_LENGTH);
		}
		
		if (firstStartFl)
		{
			firstStartFl = 0; 
		}
		else if (eventFl)
		{
			sprintf(message, "Max. temperature(%d'C) exceeded\r\n\n", highTempLevel);
			usartTx((uint8_t*)message, MESSAGE_LENGTH);
		}
		else if(!eventFl)
		{
			sprintf(message, "Temperature(<%d) - returned to normal\r\n\n", lowTempLevel);
			usartTx((uint8_t*)message, MESSAGE_LENGTH);
		}
		
		tugglePinTest();
		
		// Clear the alert interrupt flag 
		interruptAlertOccuredFl = 0;
		
		NVIC_EnableIRQ(I2C1_ER_IRQn);
		NVIC_EnableIRQ(I2C1_EV_IRQn);
	}
}



// After transmitting of address tmp1075 and address of register for reading in case, 
// the function of receive is called
void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	if (getOutPutFl)
	{
		getOutPutFl = 0;
		HAL_I2C_Master_Receive_IT(hi2c, (uint16_t)tmpAddrWithAlign, (uint8_t*)ptrRxBufferI2C, (uint16_t)SIZE_TMP_RX_DATA_BUF);
	}
}

void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c) {}


void tugglePinTest()
{
	if( LL_GPIO_IsInputPinSet(GPIOE, LL_GPIO_PIN_8)) 
	{
		LL_GPIO_ResetOutputPin(GPIOE, LL_GPIO_PIN_8);
	}
	else 
	{
		LL_GPIO_SetOutputPin(GPIOE, LL_GPIO_PIN_8);
	}
}



