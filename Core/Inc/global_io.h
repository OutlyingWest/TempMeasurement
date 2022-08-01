#include "uart_io.h"
#include "tmp1075.h"
#include "vars_it.h"



// Defines for command structures
#define S_CMD_SIZE 10
#define COMMAND_SIZE 10
#define OPT_SIZE 10
#define OPT_NUM 2
#define VAL_SIZE 30

// Defines for command options
#define NONE -111
#define PRINT 0

// For strcmp in command finde function
#define MATCHED 0


// Structure of input and output parameters and settings
typedef struct
{
	uint8_t InputStartFl;
	uint8_t* BuffRX;
	uint8_t* BuffTX;
} sIO;




// Function prototypes

// Function prototypes for execute several commands 
void cswExec(void);
void pwrExec(void);
void setLvlExec(void);
void showParamExec(void);
void tmpSendExec(void);
void echoExec(void);
void errorCmdExec(void);	

// Function takes pointer to the structure(sIO) which contanes the input and output parameters and settings
void ioData(sIO *psIO);			
