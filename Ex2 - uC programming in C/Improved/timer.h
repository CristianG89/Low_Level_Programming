#ifndef TIMER_H		// Statement to be sure this header file cannot be called more than once
#define TIMER_H


#include <stdint.h>
#include <stdbool.h>
#include "efm32gg.h"	// List of useful memory mapped registers

// TIMER1 configuration and initialization (NOTE: Timer counter registers are 16 bits)
void setupTimer1();

// Start TIMER1 to count
void startTimer1(uint16_t period);

// Stop TIMER1
void stopTimer1();


#endif //TIMER_H