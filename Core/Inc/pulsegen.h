//#include "stm32f3xx_hal_tim.h"
#include <stdint.h>
#include "stm32f3xx_ll_tim.h"
#include "stm32f3xx_hal.h"

// Global initialisation of TIM 1 structure. Defined in main.c
extern TIM_HandleTypeDef htim1;

// Structure of timer's used parameters  
typedef struct 
{
	uint32_t CompareLvl;
	uint32_t PrescVal;
	uint32_t PrdVal;
} timParam;

// Initialisation of  TIM 1 structure for some commonly used parameters. Defined in pulsegen.c 
extern timParam tim1Param;

// Timer set parameters function
void tim1SetCmnParameters(void);

// Timer initialization set parameters function
void tim1InitCmnParameters(void);
