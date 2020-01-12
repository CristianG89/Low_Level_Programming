#ifndef INTER_H		//Statement to be sure this header file cannot be called more than once
#define INTER_H


#include <stdint.h>
#include <stdbool.h>
#include "efm32gg.h"	// List of useful memory mapped registers

// Interruptions activation
void setupNVIC();

// Main functionality. To be called with a push button activation/deactivation
void GPIO_int_loop();


#endif //INTER_H