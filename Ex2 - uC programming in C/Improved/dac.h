#ifndef DAC_H	//Statement to be sure this header file cannot be called more than once
#define DAC_H


#include <stdint.h>
#include <stdbool.h>
#include "efm32gg.h"	// List of useful memory mapped registers

// DAC0 configuration (NOTE: Core clock runs at 14 MHz by default)
void setupDAC0();

// DAC0 activation
void startDAC0();

// DAC0 deactivation
void stopDAC0();


#endif //DAC_H