#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"	// List of useful memory mapped registers
#include "main.h"		// File with the "extern" variables (they are defined in another file, but can be used here too)
#include "sound.h"
#include "timer.h"
#include "dac.h"
#include "interrupt_handlers.h"

// Constant values
#define	IRQ_GPIO_EVEN	1
#define	IRQ_GPIO_ODD	11
#define	IRQ_TIMER1		12


// NOTE: "__attribute__ ((interrupt))" is an indication to the compiler that the function should be generated for an exception handler.

// TIMER1 interrupt handler 
void __attribute__ ((interrupt)) TIMER1_IRQHandler() {
	*TIMER1_IFC = *TIMER1_IF;	// Once TIMER1 interruption executed, all input flags are read and cleared to avoid infinite loop

	if (smpl >= (current_Sound->length)) {
		stopDAC0();		// While sleeping, DAC disabled
		stopTimer1 ();
		*SCR = 0x6;
	} else {
		*DAC0_CH0DATA = *DAC0_CH1DATA = volume * current_Sound->data[smpl];
		smpl++;
	}
}

// GPIO even pin interrupt handler
void __attribute__ ((interrupt)) GPIO_EVEN_IRQHandler() {
	GPIO_int_loop();			// Same function to execute with both even and odd GPIO interruption activatio
}

// GPIO odd pin interrupt handler
void __attribute__ ((interrupt)) GPIO_ODD_IRQHandler() {
	GPIO_int_loop();			// Same function to execute with both even and odd GPIO interruption activatio
}

// Main functionality. To be called with a push button activation/deactivation
void GPIO_int_loop() {
	*GPIO_IFC = *GPIO_IF;		// Once GPIO interruption executed, all input flags are read and cleared to avoid infinite loop
	*GPIO_PA_DOUT = *GPIO_PC_DIN << 8;	// Pins 0-7 in PORTC are the inputs, and pins 8-15 in PORTA are the output, so data must be shifted
		
	uint32_t buttons = *GPIO_PC_DIN & 0x000000FF;
	
	if (pre_buttons != buttons) {
		switch (buttons) {
			case 0xFE:	//0x01 in active low logic (SW1)
				period += 50;
				if (period > PERIOD_MAX) period = PERIOD_MAX;
				break;
			case 0xFD:	//0x02 in active low logic (SW2)
				volume += 10;
				if (volume > VOLUME_MAX) volume = VOLUME_MAX;
				break;
			case 0xFB:	//0x04 in active low logic (SW3)
				period -= 50;
				if (period < PERIOD_MIN) period = PERIOD_MIN;
				break;
			case 0xF7:	//0x08 in active low logic (SW4)
				volume -= 10;
				if (volume < VOLUME_MIN) volume = VOLUME_MIN;
				break;
			case 0xEF:	//0x10 in active low logic (SW5)
				playAudio(&Tone1, period, volume);
				break;
			case 0xDF:	//0x20 in active low logic (SW6)
				playAudio(&Tone2, period, volume);
				break;
			case 0xBF:	//0x40 in active low logic (SW7)
				playAudio(&Tone3, period, volume);
				break;
			case 0x7F:	//0x80 in active low logic (SW8)
				playAudio(&Melody, period, volume);
				break;
			default:	// (At least) two buttons pushed together will do nothing...
		}
	}
	pre_buttons = buttons;	// Current GPIO input value loaded into previous value
}

// Interruptions activations (SHOULD WE USE ALL OF THEM, OR ONLY THE TIMER1???)
void setupNVIC() {
	*ISER0 |= (1 << IRQ_GPIO_EVEN);		// Enable GPIO even pins interruption (bit 1)
	*ISER0 |= (1 << IRQ_GPIO_ODD);		// Enable GPIO odd pins interruption (bit 11)
	*ISER0 |= (1 << IRQ_TIMER1);		// Enable Timer 1 interruption (bit 12)
}