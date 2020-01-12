#ifndef HARDWARE_H			//Statement to be sure this header file cannot be called more than once
#define HARDWARE_H


#include "main.h"

// Union variables
typedef union {
	struct {
		unsigned int SW1_up:1;		// Button SW1 on gamepad
		unsigned int SW2_na:1;		// Button SW2 on gamepad
		unsigned int SW3_na:1;		// Button SW3 on gamepad
		unsigned int SW4_na:1;		// Button SW4 on gamepad
		unsigned int SW5_left:1;	// Button SW5 on gamepad
		unsigned int SW6_na:1;		// Button SW6 on gamepad
		unsigned int SW7_right:1;	// Button SW7 on gamepad
		unsigned int SW8_na:1;		// Button SW8 on gamepad
	} buttons;
	uint8_t raw;
} GamepadState;

typedef union {
	struct {
		unsigned int red:5;
		unsigned int green:6;
		unsigned int blue:5;
	} rgb;
	uint16_t raw;
} Color;

// Constants
extern const unsigned int SCREEN_WIDTH;
extern const unsigned int SCREEN_HEIGHT;
extern const unsigned int SCREEN_PIXEL_SIZE;
extern const unsigned int SCREEN_BUFFER_SIZE;
extern const Color BLACK;
extern const Color WHITE;
extern const Color RED;
extern const Color GREEN;
extern const Color BLUE;
extern const Color PINK;
extern const Color CYAN;
extern const Color ORANGE;

// Prototype functions
bool initScreen(void);		// Access to the screen (driver) and allocate memory in it
void deinitScreen(void);	// Close the screen (driver) and deallocate its memory
bool initGamepad(void);		// Open the gamepad (driver) and make its signal accesible
void deinitGamepad(void);	// Close the gamepad (driver)

// Function prototypes (Only non-static functions are defined in the header files)
void setGamepadSignalCallback(void (*)());		// Update the call back signal

GamepadState getGamepadValues(void);			// Get the gamepad values
GamepadState getGamepadChangesAndClear(void);	// Get gamepad changes and clear them

void drawFullScreen(Color);						// Fill the screen completely with one colour
void drawRect(unsigned int, unsigned int, unsigned int, unsigned int, Color);		// Draw a rectangle in one specific colour
void updatePartialScreen(unsigned int, unsigned int, unsigned int, unsigned int);	// Update screen partially with new data
void updateFullScreen(void);					// Update screen completely with new data

// Functions that cannot be included from the library properly
int open(const char *, int, ...);
int fcntl(int, int, ...);


#endif // PLATFORM_H