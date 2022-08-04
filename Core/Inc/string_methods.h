#include <stdint.h>


#define RANGES_SIZE 35

int limConvToNum(char* ptrToLimStr);
int collisFinded(uint8_t checkVl, uint8_t* buff, uint8_t size);
int rangedStrParser(uint8_t* formatedStr, uint8_t* sequence, uint8_t* sequeSize);
