#include <math.h>

#include "efm32gg.h"	// List of useful memory mapped registers
#include "sound.h"
#include "timer.h"
#include "gpio.h"


// Sound wave generation by the DAC, and controlled by the Timer
void playAudio(Sound *wave, uint16_t period, int8_t volume) {
	startDAC0();		// DAC and Timer activated ONLY when they will be used
	startTimer1(period);
	
	uint32_t i = 0;
	while (i < (wave->length-1)) {		// For loop gives problems...
		if (*TIMER1_CNT >= period) {	
			*DAC0_CH0DATA = *DAC0_CH1DATA = (volume/10) * wave->data[i];	// Volume/10 to ensure natural numbers as result
			i++;
		}
	}
	stopTimer1();		// Once sound reproduced, DAC and Timer deactivated again
	stopDAC0();
}