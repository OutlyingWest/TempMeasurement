#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "vars_it.h"
#include "global_io.h"




// Flag for enable or disable sending of temperature
uint8_t tmpsendFl = 1;

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
										6, "ntmps",    "--check", "--set",};

										
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
	char messageErrCmd[MESSAGE_LENGTH] = "Err: undefined command\r\n";
	
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
		handlerUsartRxIT(ON);
		
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
{}


//
void pwrExec()
{}

	
//
void setLvlExec()
{
	uint8_t isPrint = 0;
	char messageOpt[MESSAGE_LENGTH] = "Err: undefined option\r\n";
	char messageValue[MESSAGE_LENGTH] = "Err: incorrect value\r\n";
	char sep[2] = ",";
	char *valuePtr;
	uint8_t valueBuf[3] = {0};
	int8_t convResult = 0;
	uint8_t nTmp = 0;
	uint8_t lowTempLevel = 0;
	uint8_t highTempLevel = 0;
	
	// TESTING
	char messageOption[MESSAGE_LENGTH] = {0};
	sprintf(messageOption, "Opt in setlvl Exec %d\r\n", (int)curCmd.optNum);
	usartTx((uint8_t*)messageOption, MESSAGE_LENGTH);
	// ~TESTING
	
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
	valuePtr = strtok ((char*)curCmd.value, sep);
	
	// Parse value
	for (int nv = 0; valuePtr != NULL && nv < 3; nv++)
	{
		if (MATCHED ==strcmp(valuePtr, "0"))
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
{}
	

//
void tmpSendExec()
{
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
		// Error
	}
}
	

//
void echoExec()
{}
	

//
void errorCmdExec()
{}

	
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
		getSelectedTemperatures(connectedTmpNums, kTmpBufSize);
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
