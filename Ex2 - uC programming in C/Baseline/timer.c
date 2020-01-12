#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"	// List of useful memory mapped registers

// TIMER1 configuration and initialization (NOTE: Timer counter registers are 16 bits)
void setupTimer1() {
	*CMU_HFPERCLKEN0 |= CMU2_HFPERCLKEN0_TIMER1;	// Enable clock of Timer 1 (bit 6) to make it working
}

void startTimer1(uint32_t period) {
	*TIMER1_TOP = period;		// Defined period (number of timer ticks between interrupts)
	*TIMER1_CMD = 0x1;			// Start TIMER1 to count
}

void stopTimer1() {
	*TIMER1_CMD = 0x0;			// Stop TIMER1
}