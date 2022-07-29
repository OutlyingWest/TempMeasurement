#include <vars_it_typedef.h>

// Increases every millisecond in interrupt handler of tim6 
extern uint32_t tim6Tick;

// Last time ticks value of tim6
extern uint32_t tim6LastTimeTmp;

// Last time ticks value of tim6 for USART
extern uint32_t tim6LastTimeUSART;

// If this flag is set, then an AlertHandleIT function in mainloop will executed
extern volatile uint8_t interruptAlertOccuredFl;

// If this flag is set, then a handlerUsartRxIT function in mainloop will executed
extern volatile uint8_t txUnblockUsartHandlerFl;

// Structure with Rx Tx USART buffers for IT
extern sUARTit sUART3it;
