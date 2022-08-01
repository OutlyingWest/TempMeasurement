#include <string.h>
#include "global_io.h"




// Structure of commands
struct Commands
{
	uint8_t code;
	char command[COMMAND_SIZE];
	char arg[ARG_NUM][ARG_SIZE];
}cmd[S_CMD_SIZE] = {0, "a", "-b", "-c",
										1, "d", "-e", "-f"};

// Find commands in input strings
int inputCommandFinder(uint8_t *rxUsartBuff, uint8_t sizeBuff)
{
	char sep[5] = " ";
	char *rxPtr = (char*)rxUsartBuff;
	
	rxPtr = strtok ((char*)rxUsartBuff, sep);

	// Find command
	for (int cd = 0; rxPtr != NULL && cd < S_CMD_SIZE; cd++)
	{
		if (strcmp(rxPtr, cmd[cd].command))
		{
			//TODO: Some behaveour
			;
			rxPtr = strtok(NULL, sep);
			// Find arguments
			for (int ar = 0; rxPtr != NULL && ar < S_CMD_SIZE; ar++)
			{
				if (strcmp(rxPtr, cmd[cd].arg[ar]))
				{
					//TODO: Some behaveour
					;
				}
				rxPtr = strtok(NULL, sep);
			}
		}
		else
		{
			rxPtr = strtok(NULL, sep);
		}
	}
}
	
// Execute finded commands
void inputCommandExecuter()
{}
	

// Handle overall commands
void inputCommandWizard()
{}

//Cmd cmd = {"qwer", "rty", "-qw" , "-er"};


//void cmdInit()
//{
//	cmd.command
//}




// Should count the time in order to avoid the long suspension in reading keyboard
// Maybe should use the interrupts
void ioData(sIO *psIO)
{

	if (psIO->InputStartFl)
	{
		
	}
}
