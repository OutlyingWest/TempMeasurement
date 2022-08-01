#include "uart_io.h"
#include "vars_it.h"




#define S_CMD_SIZE 10
#define COMMAND_SIZE 10
#define ARG_SIZE 10
#define ARG_NUM 2


// Structure of input and output parameters and settings
typedef struct
{
	uint8_t InputStartFl;
	uint8_t* BuffRX;
	uint8_t* BuffTX;
} sIO;




// Function prototypes

// Function takes pointer to the structure(sIO) which contanes the input and output parameters and settings
void ioData(sIO *psIO);			
