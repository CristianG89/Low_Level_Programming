#ifndef MAIN_H	//Statement to be sure this header file cannot be called more than once
#define MAIN_H


// External (global) variables. They are defined outside any function, so they are accesible globally and their values can be modified by any function too.
extern Sound *current_Sound;
extern Sound Melody, Tone1, Tone2, Tone3;

extern uint16_t period;
extern int8_t volume;
extern uint32_t smpl;
extern uint32_t pre_buttons;



#endif //MAIN_H