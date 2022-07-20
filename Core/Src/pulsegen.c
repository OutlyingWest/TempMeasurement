#include "pulsegen.h"


// Structure of 1st timer's used parameters  
timParam tim1Param = {0,0,0};

void tim1SetCmnParameters()
{
	// Set of TIM 1 inint. parameters in structure
	tim1Param.CompareLvl = 44; //44
	tim1Param.PrescVal = 0;
	tim1Param.PrdVal = 114;   //114
}


void tim1InitCmnParameters()
{
	LL_TIM_SetPrescaler(TIM1, tim1Param.PrescVal);
	LL_TIM_SetAutoReload(TIM1, tim1Param.PrdVal);
	LL_TIM_OC_SetCompareCH1(TIM1, tim1Param.CompareLvl);
}


