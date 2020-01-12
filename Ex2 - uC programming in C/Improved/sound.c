#include <math.h>

#include "efm32gg.h"	// List of useful memory mapped registers
#include "sound.h"
#include "timer.h"
#include "dac.h"
#include "gpio.h"
#include "main.h"


// Selected audio loaded into a generic sound struct to be reproduced by the timer interruption
void playAudio (Sound * wave) {
	current_Sound = wave;
	
	*SCR = 0x0;		// System waken up to reproduce the audio
	startDAC0();	// To reproduce audio, DAC enabled again
	startTimer1(period);

	smpl = 0;		// Song should start from position 0 again if another audio requested
}
