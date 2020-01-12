#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"	// List of useful memory mapped registers

// Function to set up GPIO mode
void setupGPIO() {
	// Enable clock for GPIO pins
	*CMU_HFPERCLKEN0 |= CMU2_HFPERCLKEN0_GPIO;	// OR gate for setting 1 the bit 13 (CMU_HFPERCLKEN0_GPIO)
	
	// Configuration of the output pins (PORTA) - and its drive strength
	*GPIO_PA_CTRL = 0x2;			// Set high drive strength in PORTA
	*GPIO_PA_MODEH = 0x55555555;	// Pins 8-15 of PORTA set as output pins, mode 5 (Push-pull with alt. drive strength mode)

	// Configuration of the input pins (PORTC)
	*GPIO_PC_MODEL = 0x33333333;	// Pins 0-7 of PORTC set as input pins, mode 3 (requires writing in GPIO_PC_DOUT for final configuration)
	*GPIO_PC_DOUT = 0xFF;			// Enables pull-up resistor (to avoid missreadings) and filter (for noise tolerance) to the previous input pins
}