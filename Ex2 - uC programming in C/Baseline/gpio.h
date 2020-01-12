#ifndef GPIO_H	//Statement to be sure this header file cannot be called more than once
#define GPIO_H


#include <stdint.h>
#include <stdbool.h>
#include "efm32gg.h"	// List of useful memory mapped registers

// Function to set up GPIO mode
void setupGPIO();


#endif //GPIO_H