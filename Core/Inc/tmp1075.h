#include "stm32f3xx_hal.h"
#include "stm32f3xx_ll_gpio.h"

// Defines

// Maximum transmitting wait time for HAL I2C functions in blocking mode
// in initialization time
#define INIT_TIMEOUT 700

// Maximum wait time for HAL I2C functions in blocking mode 
// on the runtime 
#define RUNTIME_TIMEOUT 100

// Length of error message 
#define MESSAGE_LENGTH 40
#define MESSAGE_LONG_LENGTH 100
// Addresses(7-bit) of tmp measurers
#define TMP_1_ADDR 0x40 // 0x48

// Address which sends to over tmps if Alert line adge is low
#define ALERT_RESPONSE_ADDR 0x0C

// Switch Read/Write mode
#define TMP_READ 1
#define TMP_WRITE 0

// Size of data buffers
#define SIZE_TMP_RX_DATA_BUF 2
#define SIZE_TMP_TX_DATA_BUF 2


// Defines to internal needs
#define CLEAR_SIGN_BIT 0x7F

// Addresses of tmp1075 registers
#define TEMP_REG_ADDR 0x00
#define CFGR_REG_ADDR 0x01
#define LLIM_REG_ADDR 0x02
#define HLIM_REG_ADDR 0x03
#define DIED_REG_ADDR 0x0F

// Period of get temperature value to show
#define GET_TEMP_TIME 1000 

// Settings for CFGR register
#define SET_ALERT_IT_MOD 0x02

// Default minimum and maximum temperature edges for alerts
#define DEFAULT_TLOW 26
#define DEFAULT_THIGH 26

#define NUMBER_OF_TMP_SENSORS 32


#define ON 1
#define OFF 0
#define NO_PRINT 0

// Externs

// Pointer to the HAL pointet to I2C init structure
extern I2C_HandleTypeDef hi2c1;

// Variables

// Rx I2C Buffer 
extern uint8_t rxBufferI2C[];

// Tx I2C Buffer 
extern uint8_t txBufferI2C[];

// Size of buffer of connected tmps
extern const uint8_t kTmpBufSize;

// Buffer of connected tmps
extern uint8_t connectedTmpNums[];

// Address which sends to over tmps if Alert line adge is low
extern const uint16_t alertResponseAddrWithAlign;

// Function prototypes
void aReceiveI2C(I2C_HandleTypeDef hi, uint16_t tmpAddr, uint8_t *aTxBuffer, uint16_t sizeDataBuf, uint32_t timeout, const char *comleteMessage);
void aTransmitI2C(I2C_HandleTypeDef hi, uint16_t tmpAddr, uint8_t *aTxBuffer, uint16_t sizeDataBuf, uint32_t timeout, const char *comleteMessage);
void initIndividualTmpAlertIT(uint8_t nTmpr);
void initIndividualTmpAlertLimits(uint8_t nTmpr);
void initAlertSelectedTmps(uint8_t *connectedTmpNums, uint8_t sizeTmpNumsBuff);
void tugglePinTest(void);
void handlerAlertIT(uint8_t *tmpNums, uint8_t sizeTmpNumsBuff);
void showAllTmpParameters(void);
void showIndividualTmpParameters(uint8_t nTmpr, uint8_t headerOn);
void setAllDefaultTmpParameters(uint8_t lowTempLevel,
                             uint8_t highTempLevel,
                             uint8_t isPrint);
void setIndividualTmpParameters(uint8_t nTmpr,
						                    uint8_t lowTempLevel,
						                    uint8_t highTempLevel,
						                    uint8_t isPrint);

// Get the temperature from tmp1075 and displays it on terminal 
void getSelectedTemperatures(uint8_t *tmpNums, uint8_t sizeTmpNumsBuff);
