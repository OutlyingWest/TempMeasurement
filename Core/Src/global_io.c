#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "vars_it.h"
#include "global_io.h"
#include "string_methods.h"



// Flag for enable or disable sending of temperature
uint8_t tmpsendFl = 1;

// Flag for enable or disable echo mod for commands
uint8_t isEchoFl = 1;

// Structure of commands
struct Commands
{
	uint8_t code;
	char command[COMMAND_SIZE];
	char option[OPT_NUM][OPT_SIZE];
}cmd[S_CMD_SIZE] = {0, "csv",      "-e",      "-d",
										1, "pwr",      "--on",    "--off",
										2, "setlvl",   "-p",      "None",
										3, "shwparam", "--all",   "None",
										4, "tsend",    "-e",      "-d",
										5, "echo",     "--on",    "--off",
										6, "setrng",   "-p",      "None",
										7, "tchck",    "--all",   "None",
			              8, "tinit",    "--all",   "None",
										9, "tdeinit",  "--all",   "None",
									 10, "disp",     "--all",   "None",};

										
struct currentCmd
{
	int8_t code;
	int8_t optNum;
	char value[VAL_SIZE];
}curCmd = {NONE, NONE, "None"};

										

										
// Find commands in input strings
// Fill the currentCmd structure in format:
// Example: code | optNum  |   value
//             2 |    0(-p)| 1,27,28
void inputCommandFinder(uint8_t *rxUsartBuff, uint8_t sizeBuff)
{
	char sep[4] = " \r\n";
	char *rxPtr = (char*)rxUsartBuff;
	uint8_t matchCmdFl = 1;
	char messageErrCmd[MESSAGE_LENGTH] = "Error: undefined command\r\n";
	
	rxPtr = strtok ((char*)rxUsartBuff, sep);

	// Find command
	for (int cd = 0; rxPtr != NULL && cd < S_CMD_SIZE; cd++)
	{
		if (MATCHED == strcmp(rxPtr, cmd[cd].command))
		{
			matchCmdFl = MATCHED;
			curCmd.code = cmd[cd].code;
			rxPtr = strtok(NULL, sep);
			
			// Find option
			for (int op = 0; rxPtr != NULL && op < OPT_NUM; op++)
			{
				if (MATCHED == strcmp(rxPtr, cmd[cd].option[op]))
				{
					curCmd.optNum = op;
					rxPtr = strtok(NULL, sep);
					break;
				}
			}
			
			// Record value
			if (rxPtr)
			{
				strcpy(curCmd.value, rxPtr);
			}
			break;
		}
	}
	if (matchCmdFl != MATCHED)
	{
		usartTx((uint8_t*)messageErrCmd, MESSAGE_LENGTH);
	}
//	else if (matchOptFl != MATCHED)
//	{
//		usartTx((uint8_t*)messageErrOpt, MESSAGE_LENGTH);
//	}
}
	
// Execute finded commands
// with accordance of currentCmd structure
void inputCommandExecuter()
{
	if (curCmd.code == cmd[0].code)
	{
		cswExec();
	}
	else if (curCmd.code == cmd[1].code)
	{
		pwrExec();
	}
	else if (curCmd.code == cmd[2].code)
	{
		setLvlExec();
	}
	else if (curCmd.code == cmd[3].code)
	{
		showParamExec();
	}
	else if (curCmd.code == cmd[4].code)
	{
		tmpSendExec();
	}
	else if (curCmd.code == cmd[5].code)
	{
		echoExec();
	}
	else if (curCmd.code == cmd[8].code)
	{
		tinitExec();
	}
	else if (curCmd.code == cmd[9].code)
	{
		tdeinitExec();
	}
	else
	{
		errorCmdExec();
	}
}
	

// Handle overall commands
void inputCommandWizard()
{
	if (unblockUsartHandlerFl)
	{
		// Echo ON
		handlerUsartRxIT(isEchoFl);
		
		// TESTING
		char messageRx[MESSAGE_LENGTH] = {0};
		sprintf(messageRx, "Cmd in Wiz %s\r\n", (char*)sUART3it.rxData);
		usartTx((uint8_t*)messageRx, MESSAGE_LENGTH);
		// ~TESTING
		
		inputCommandFinder(sUART3it.rxData, USART_BUFFER_SIZE);
		inputCommandExecuter();
		curCmdStructCleaner();
		usartRxDataBuffCleaner();
		unblockUsartHandlerFl = 0;
	}
}

	
//
void cswExec()
{
	char messageOpt[MESSAGE_LENGTH] = "Error: undefined option\r\n";
	
	if (curCmd.optNum == ENABLE_CSV_MOD)
	{
		csvMod = ON;
	}
	else if (curCmd.optNum == DISABLE_CSV_MOD)
	{
		csvMod = OFF;
	}
	else
	{
		usartTx((uint8_t*)messageOpt, MESSAGE_LENGTH);
	}
}


//
void pwrExec()
{}

	
//
void setLvlExec()
{
	uint8_t isPrint = 0;
	char messageOpt[MESSAGE_LENGTH] = "Error: undefined option\r\n";
	char messageValue[MESSAGE_LENGTH] = "Error: incorrect value\r\n";
	char sep[2] = ",";
	char *valuePtr;
	uint8_t valueBuf[3] = {0};
	int8_t convResult = 0;
	uint8_t nTmp = 0;
	uint8_t lowTempLevel = 0;
	uint8_t highTempLevel = 0;

	
	if (curCmd.optNum == PRINT)
	{
		isPrint = ON;
	}
	else if(curCmd.optNum == NONE)
	{
		isPrint = OFF;
	}
	else
	{
		usartTx((uint8_t*)messageOpt, MESSAGE_LENGTH);
	}
	valuePtr = strtok((char*)curCmd.value, sep);
	
	// Parse value
	for (int nv = 0; valuePtr != NULL && nv < 3; nv++)
	{
		if (MATCHED == strcmp(valuePtr, "0"))
		{
			convResult = 0;
			valueBuf[nv] = convResult;
		}
		else if (0 != (convResult = atoi(valuePtr)))
		{
			valueBuf[nv] = convResult;
		}
		else
		{
			usartTx((uint8_t*)messageValue, MESSAGE_LENGTH);
		}
		valuePtr = strtok(NULL, sep);
	}
	// Fill arguments
	nTmp = valueBuf[0];
	lowTempLevel = valueBuf[1];
	highTempLevel = valueBuf[2];

	
	// Set limits in tmp structure
	setIndividualTmpParameters(nTmp,
						                 lowTempLevel,
						                 highTempLevel,
						                 isPrint);
	
	// Init limits - send data to tmp sensor
	initIndividualTmpAlertLimits(nTmp);
}
	

//
void showParamExec()
{
	char messageOpt[MESSAGE_LENGTH] = "Error: undefined option\r\n";
	char messageValue[MESSAGE_LENGTH] = "Error: incorrect value\r\n";
	char messageValueRange[MESSAGE_LENGTH] = "Error: out of range\r\n";
	uint8_t headerFl = 1;


	if (csvMod)
		headerFl = 0;

	if (curCmd.optNum == OPT_ALL)
	{
		showAllTmpParameters(headerFl);
	}
	else if (curCmd.optNum == NONE)
	{
		uint8_t tmpNumbers[NUMBER_OF_TMP_SENSORS] = { 0 };
		uint8_t sequeTmpSize = 0;
		uint8_t errFl = 0;

		
		errFl = rangedStrParser((uint8_t*)curCmd.value, tmpNumbers, &sequeTmpSize);
		if (errFl)
		{
			usartTx((uint8_t*)messageValue, MESSAGE_LENGTH);
		}
		else
		{
			// Check out of range
			for (uint8_t tnum = 0; tnum < sequeTmpSize; tnum++)
			{
				if (tmpNumbers[tnum] > NUMBER_OF_TMP_SENSORS)
				{
					errFl = 1;
				}
			}
			// If errors not finded - show selected parameters
			if (sequeTmpSize <= NUMBER_OF_TMP_SENSORS && !errFl)
			{	
				showSelectedTmpParameters(tmpNumbers, sequeTmpSize, headerFl);
			}
			else
			{
				usartTx((uint8_t*)messageValueRange, MESSAGE_LENGTH);
			}
		}
	}
	else
	{
		usartTx((uint8_t*)messageOpt, MESSAGE_LENGTH);
	}
}
	

//
void tmpSendExec()
{
	char messageOpt[MESSAGE_LENGTH] = "Error: undefined option\r\n";
	
	if (curCmd.optNum == ENABLE_TMP_OUT)
	{
		tmpsendFl = 1;
	}
	else if (curCmd.optNum == DISABLE_TMP_OUT)
	{
		tmpsendFl = 0;
	}
	else
	{
		usartTx((uint8_t*)messageOpt, MESSAGE_LENGTH);
	}
}
	

//
void echoExec()
{
	char messageOpt[MESSAGE_LENGTH] = "Error: undefined option\r\n";
	
	if (curCmd.optNum == ENABLE_ECHO)
	{
		isEchoFl = 1;
	}
	else if (curCmd.optNum == DISABLE_ECHO)
	{
		isEchoFl = 0;
	}
	else
	{
		usartTx((uint8_t*)messageOpt, MESSAGE_LENGTH);
	}
}


void tinitExec()
{
	char messageOpt[MESSAGE_LENGTH] = "Error: undefined option\r\n";
	char messageValue[MESSAGE_LENGTH] = "Error: incorrect value\r\n";
	char messageValueRange[MESSAGE_LENGTH] = "Error: out of range\r\n";
	uint8_t allTmpNumsBuff[NUMBER_OF_TMP_SENSORS] = {0};


	if (curCmd.optNum == OPT_ALL)
	{
	  for (uint8_t ntmp = 0; ntmp < NUMBER_OF_TMP_SENSORS; ntmp++)
		{
			allTmpNumsBuff[ntmp] = ntmp;
		}

		// Init Alerts in tmp1075
		initAlertSelectedTmps(allTmpNumsBuff, NUMBER_OF_TMP_SENSORS);
	
		// Set number of faults for crossing the high level limit of temperature
		initNumFaultsSelectedTmps(allTmpNumsBuff, NUMBER_OF_TMP_SENSORS, 4);
	}
	else if (curCmd.optNum == NONE)
	{
		uint8_t tmpNumbers[NUMBER_OF_TMP_SENSORS] = { 0 };
		uint8_t sequeTmpSize = 0;
		uint8_t errFl = 0;

		
		errFl = rangedStrParser((uint8_t*)curCmd.value, tmpNumbers, &sequeTmpSize);
		if (errFl)
		{
			usartTx((uint8_t*)messageValue, MESSAGE_LENGTH);
		}
		else
		{
			// Check out of range
			for (uint8_t tnum = 0; tnum < sequeTmpSize; tnum++)
			{
				if (tmpNumbers[tnum] > NUMBER_OF_TMP_SENSORS)
				{
					errFl = 1;
				}
			}
			// If errors not finded - show selected parameters
			if (sequeTmpSize <= NUMBER_OF_TMP_SENSORS && !errFl)
			{	
				// Init Alerts in tmp1075
				initAlertSelectedTmps(tmpNumbers, sequeTmpSize);
	
				// Set number of faults for crossing the high level limit of temperature
				initNumFaultsSelectedTmps(tmpNumbers, sequeTmpSize, 4);
				
				// Fill the global tmp nums buffer
				uint8_t newTmpSize = tmpBufSize + sequeTmpSize;
				
				// Numbers readed from command
				uint8_t ntmpReaded = 0;
				
				// Numbers which already in tmp nums buffer
				uint8_t ntmpConnected = 0;
				
				for (ntmpConnected = tmpBufSize; ntmpConnected < newTmpSize; ntmpConnected++, ntmpReaded++)
				{
					connectedTmpNumsBuff[ntmpConnected] = tmpNumbers[ntmpReaded];
				}
				
				// Redefenition of global tmp nums variables 
				connectedTmpNums = connectedTmpNumsBuff;
				tmpBufSize = newTmpSize;
				
				if (tmpBufSize)
				{
					// Enable output
					tmpsendFl = 1;
				}
				
			}
			else
			{
				usartTx((uint8_t*)messageValueRange, MESSAGE_LENGTH);
			}
		}
	}
	else
	{
		usartTx((uint8_t*)messageOpt, MESSAGE_LENGTH);
	}
}


void tdeinitExec()
{
	char messageOpt[MESSAGE_LENGTH] = "Error: undefined option\r\n";
	char messageValue[MESSAGE_LENGTH] = "Error: incorrect value\r\n";
	char messageValueRange[MESSAGE_LENGTH] = "Error: out of range\r\n";


	if (curCmd.optNum == OPT_ALL)
	{
		// Set deinit code for all tmp sensors 
	  for (uint8_t ntmp = 0; ntmp < NUMBER_OF_TMP_SENSORS; ntmp++)
		{
			connectedTmpNumsBuff[ntmp] = TMP_DEINIT_CODE;
		}
		
		// Disable output
		tmpsendFl = 0;
		
		// Number of tmp sensors output set to 0
		tmpBufSize = 0;
		
	}
	else if (curCmd.optNum == NONE)
	{
		uint8_t tmpNumbers[NUMBER_OF_TMP_SENSORS] = { 0 };
		uint8_t sequeTmpSize = 0;
		uint8_t errFl = 0;

		
		errFl = rangedStrParser((uint8_t*)curCmd.value, tmpNumbers, &sequeTmpSize);
		if (errFl)
		{
			usartTx((uint8_t*)messageValue, MESSAGE_LENGTH);
		}
		else
		{
			// Check out of range
			for (uint8_t tnum = 0; tnum < sequeTmpSize; tnum++)
			{
				if (tmpNumbers[tnum] > NUMBER_OF_TMP_SENSORS)
				{
					errFl = 1;
				}
			}
			// If errors not finded - show selected parameters
			if (sequeTmpSize <= NUMBER_OF_TMP_SENSORS && !errFl)
			{	
				// Redefenition of global tmp nums variables:
				// - Remove numbers of tmp sensors from global tmp buffer
				// - Decrease size of global tmp buffer in according to this
				removeValues(connectedTmpNums, &tmpBufSize, tmpNumbers, sequeTmpSize);
				
				if (!tmpBufSize)
				{
					// Disable output
					tmpsendFl = 0;
				}
			}
			else
			{
				usartTx((uint8_t*)messageValueRange, MESSAGE_LENGTH);
			}
		}
	}
	else
	{
		usartTx((uint8_t*)messageOpt, MESSAGE_LENGTH);
	}
}


//
void errorCmdExec()
{}

	

void removeValues(uint8_t* bufferToResize, uint8_t* sizeOfResBuf, uint8_t* valueBuffer, uint8_t sizeOfValBuf)
{
	// New index for element in bufferToResize
	uint8_t iNew = 0;
	uint8_t sameValueFindedFl = 0;

	for (uint8_t iRes = 0; iRes < *sizeOfResBuf; iRes++)
	{
		sameValueFindedFl = 0;
		for (uint8_t iVal = 0; iVal < sizeOfValBuf; iVal++)
		{
			if (bufferToResize[iRes] == valueBuffer[iVal])
			{
				sameValueFindedFl = 1;
				break;
			}
		}
		if (!sameValueFindedFl)
		{
			bufferToResize[iNew++] = bufferToResize[iRes];
		}
	}
	*sizeOfResBuf = iNew;
}
	
void curCmdStructCleaner()
{
	curCmd.code = NONE;
	curCmd.optNum = NONE;
	for (uint8_t nv = 0; nv < VAL_SIZE; curCmd.value[nv++] = 0){}
}


void usartRxDataBuffCleaner()
{
	for (uint8_t nd = 0; nd < USART_BUFFER_SIZE; sUART3it.rxData[nd++] = '\0'){}
}


void tempOutput()
{
	if (tmpsendFl)
	{
		getSelectedTemperatures(connectedTmpNums, tmpBufSize);
	}
}

	
// Should count the time in order to avoid the long suspension in reading keyboard
// Maybe should use the interrupts
void ioData(sIO *psIO)
{

	if (psIO->InputStartFl)
	{
		
	}
}
