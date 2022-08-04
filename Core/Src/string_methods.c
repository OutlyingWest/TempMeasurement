#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "string_methods.h"


int limConvToNum(char* ptrToLimStr)
{
	uint8_t lim = 0;
	uint8_t convResult = 0;

	if (!strcmp(ptrToLimStr, "0"))
	{
		lim = 0;
	}
	else if (0 != (convResult = atoi(ptrToLimStr)))
	{
		lim = convResult;
	}
	else
	{
		lim = 255;  // Impossible value
	}

	return lim;
}


int collisFinded(uint8_t checkVl, uint8_t* buff, uint8_t size)
{
	for (uint8_t i = 0; i < size; i++)
	{
		if (checkVl == buff[i])
			return 1;
	}
	return 0;
}


// Function accepts 2 args.
// arg 1: string with number ranges separated ","
//		  For example formatedStr: "0-5,7-9,10,11-20"
// 
// arg 2: sequence formed by ranges in arg 1
// 
// return: int value - 0 if not errors occured
//					  -1 if errors occured
int rangedStrParser(uint8_t* formatedStr, uint8_t* sequence, uint8_t* sequeSize)
{
	char* partsPtrs[RANGES_SIZE];  // pointer to separated by "," parts of value
	char sepComma[2] = ",";
	char sepDash[2] = "-";
	char* partPtr;
	char* rangePtr;
	int returnValue = 0;

	struct Limits
	{
		uint8_t lowRangeLim;
		uint8_t highRangeLim;
	}lim[RANGES_SIZE] = { 0 };
	uint8_t partNumber;

	// Searching for parts of value separated ","
	partPtr = strtok((char*)formatedStr, sepComma);
	for (partNumber = 0; partPtr != NULL && partNumber < RANGES_SIZE; partNumber++)
	{
		partsPtrs[partNumber] = partPtr;
		partPtr = strtok(NULL, sepComma);
	}

	// Searching for parts of value separated "-"
	for (uint8_t partNum = 0; partNum < partNumber; partNum++)
	{
		partPtr = partsPtrs[partNum];
		rangePtr = strtok(partPtr, sepDash);

		// Check for find errors of input format
		if (rangePtr == NULL)
		{
			returnValue = -1;
			break;
		}

		// Get low limit for single range
		lim[partNum].lowRangeLim = limConvToNum(rangePtr);

		// Get high limit for single range 
		rangePtr = strtok(NULL, sepDash);

		// Check of existing high limit of range
		if (rangePtr != NULL)
		{
			lim[partNum].highRangeLim = limConvToNum(rangePtr);
		}
	}

	// Calculation of tmp sensors sequence size
	for (uint8_t partNum = 0; partNum < partNumber; partNum++)
	{
		if (lim[partNum].highRangeLim == 0)
		{
			(*sequeSize)++;
		}
		else
		{
			if (lim[partNum].lowRangeLim <= lim[partNum].highRangeLim)
			{
				*sequeSize += lim[partNum].highRangeLim - lim[partNum].lowRangeLim + 1;
			}
			else
			{
				returnValue = -1;
			}
		}
	}

	// Creating numeric sequence by ranges
	uint8_t tmpNum = 0;
	uint8_t cntBuf = 0;
	for (uint8_t partNum = 0; cntBuf < *sequeSize; partNum++)
	{
		if (lim[partNum].highRangeLim != 0)
		{
			for (tmpNum = lim[partNum].lowRangeLim; tmpNum <= lim[partNum].highRangeLim; tmpNum++)
			{
				if (!collisFinded(tmpNum, sequence, cntBuf))
				{
					sequence[cntBuf++] = tmpNum;
				}
				else
				{
					(*sequeSize)--;
				}

			}
		}
		else if (lim[partNum].highRangeLim == 0)
		{
			if (!collisFinded(lim[partNum].lowRangeLim, sequence, cntBuf))
			{
				sequence[cntBuf++] = lim[partNum].lowRangeLim;
			}
			else
			{
				(*sequeSize)--;
			}
		}
		else
		{
			// Impossible branch
			returnValue = -1;
		}
	}
	return returnValue;
}
