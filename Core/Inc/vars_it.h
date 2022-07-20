#include <stdint.h>

// Increases every millisecond in interrupt handler of tim6 
extern uint32_t tim6Tick;

// Last time ticks value of tim6
extern uint32_t tim6LastTime;

// If this flag is set, then an AlertHandleIT function in mainloop will executed
extern volatile uint8_t interruptAlertOccuredFl;
