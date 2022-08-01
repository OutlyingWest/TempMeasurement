#include <string.h>
#include "global_io.h"




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
										4, "tmpsend",  "-e",      "-d",
										5, "echo",     "--on",    "--off",
										6, "ntmps",    "--check", "--set",};

										
struct currentCmd
{
	int8_t code;
	int8_t optNum;
	char value[VAL_SIZE];
}curCmd = {NONE, NONE, "None"};

										

										
// Find commands in input strings
// Fill the currentCmd structure
void inputCommandFinder(uint8_t *rxUsartBuff, uint8_t sizeBuff)
{
	char sep[2] = " ";
	char *rxPtr = (char*)rxUsartBuff;
	
	rxPtr = strtok ((char*)rxUsartBuff, sep);

	// Find command
	for (int cd = 0; rxPtr != NULL && cd < S_CMD_SIZE; cd++)
	{
		if (MATCHED == strcmp(rxPtr, cmd[cd].command))
		{
			curCmd.code = cmd[cd].code;
			rxPtr = strtok(NULL, sep);
			
			// Find option
			for (int op = 0; rxPtr != NULL && op < S_CMD_SIZE; op++)
			{
				if (MATCHED == strcmp(rxPtr, cmd[cd].option[op]))
				{
					curCmd.optNum = op;
					rxPtr = strtok(NULL, sep);
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
	else if (curCmd.code == cmd[6].code)
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
	if (curCmd.optNum == PRINT)
	{
		
	}
	else if(curCmd.optNum == NONE)
	{
		
	}
	else
	{
		// Incorrect command
	}
}
	

//
void showParamExec()
{}
	

//
void tmpSendExec()
{}
	

//
void echoExec()
{}
	

//
void errorCmdExec()
{}
	
	
// Should count the time in order to avoid the long suspension in reading keyboard
// Maybe should use the interrupts
void ioData(sIO *psIO)
{

	if (psIO->InputStartFl)
	{
		
	}
}
