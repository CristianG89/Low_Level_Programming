////////////////////////////////////
// EXERCISE 2 - BASELINE, TDT4258 //
////////////////////////////////////

#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"	// List of useful memory-mapped registers
#include "timer.h"
#include "dac.h"
#include "gpio.h"
#include "sound.h"
#include "sound_samples.h"


int main(void) {
	uint16_t period = CPU_CLOCK/SAMPLING_RATE;		// Period = 14000kHz/44.1kHz = 317.46 (317) clock cycles
	int8_t volume = VOLUME_DEF;
	uint32_t buttons = 0;
	uint32_t pre_buttons = 0;
	
	// Sounds (Melody and tones) definition and initialization
	Sound Melody, Tone1, Tone2, Tone3;
	
	Melody.data = data_Melody;
	Melody.length = LENGTH_MELODY;

	Tone1.data = data_Tone1;
	Tone1.length = LENGTH_TONE1;

	Tone2.data = data_Tone2;
	Tone2.length = LENGTH_TONE2;

	Tone3.data = data_Tone3;
	Tone3.length = LENGTH_TONE3;
	
	// Peripheral setup functions
	setupTimer1();
	setupDAC0();
	setupGPIO();
	
	// Melody play at the start-up
	playAudio (&Melody, period, volume);

	while (1) {
		*GPIO_PA_DOUT = *GPIO_PC_DIN << 8;	// Pins 0-7 in PORTC are the inputs, and pins 8-15 in PORTA are the output, so data must be shifted
		buttons = *GPIO_PC_DIN & 0x000000FF;
		
		if (pre_buttons != buttons) {	// Only in case the current GPIO input value differs from the previous one, do an action
			switch (*GPIO_PC_DIN) {
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
		pre_buttons = buttons;		// Last GPIO input value updated
	}
	return 0;
}