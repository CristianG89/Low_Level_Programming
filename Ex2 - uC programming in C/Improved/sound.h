#ifndef SOUND_H	//Statement to be sure this header file cannot be called more than once
#define SOUND_H


#include <stdint.h>
#include "efm32gg.h"	// List of useful memory mapped registers

typedef struct {
	uint16_t *data;
	uint16_t length;
} Sound;

void playAudio(Sound *wave);


#endif //SOUND_H