#include <ctype.h>
#include "uart_io.h"
#include "tmp1075.h"



// Defines for command structures
#define S_CMD_SIZE 11
#define COMMAND_SIZE 10
#define OPT_SIZE 10
#define OPT_NUM 2
#define VAL_SIZE 30

// Defines for command options
#define NONE -111
#define PRINT 0

// For tmpSendExec()
#define ENABLE_TMP_OUT 0
#define DISABLE_TMP_OUT 1

// For cswExec()
#define ENABLE_CSV_MOD 0
#define DISABLE_CSV_MOD 1

// For echoExec()
#define ENABLE_ECHO 0
#define DISABLE_ECHO 1

// For showParamExec()
#define OPT_ALL 0

// For strcmp in command finde function
#define MATCHED 0


// Structure of input and output parameters and settings
typedef struct
{
	uint8_t InputStartFl;
	uint8_t* BuffRX;
	uint8_t* BuffTX;
} sIO;


// Externs

// Flag that enable or disable send temperature measurements
// to PC from USART
extern uint8_t tmpsendFl;

// Function prototypes
void tempOutput(void);
void inputCommandWizard(void);

void curCmdStructCleaner(void);
void usartRxDataBuffCleaner(void);

// Function prototypes for execute several commands 
void cswExec(void);
void pwrExec(void);
void setLvlExec(void);
void showParamExec(void);
void tmpSendExec(void);
void echoExec(void);
void tinitExec(void);
void tdeinitExec(void);
void errorCmdExec(void);	

// Function to remove values from buffer and shift indexes in according to this
void removeValues(uint8_t* bufferToResize, uint8_t* sizeOfResBuf, uint8_t* valueBuffer, uint8_t sizeOfValBuf);

// Function takes pointer to the structure(sIO) which contanes the input and output parameters and settings
void ioData(sIO *psIO);			
