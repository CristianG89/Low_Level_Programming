////////////////////////////////////
// EXERCISE 3 - HARDWARE, TDT4258 //
////////////////////////////////////

#include <signal.h>
#include <linux/fb.h>
#include <linux/fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "hardware.h"

// Function prototypes (Static functions shall NOT be defined in the header files, to ensure all processes have access to them...)
static void readGamepad(void);							// Read gamepad signal status
static void gamepadSignalHandler(int);					// Read the call back signal

// Constants
static const char *SCREEN_FILE = "/dev/fb0";			// Drivers, to be opened as files (as in all Linux distributions...)
static const char *GAMEPAD_FILE = "/dev/gamepad";

const unsigned int SCREEN_WIDTH = 320;
const unsigned int SCREEN_HEIGHT = 240;
const unsigned int SCREEN_PIXEL_SIZE = 2;
const unsigned int SCREEN_BUFFER_SIZE = 320 * 240 * 2;
const Color BLACK = {.raw = (uint16_t) 0x0000 };
const Color WHITE = {.raw = (uint16_t) 0xFFFF };
const Color RED = {.raw = (uint16_t) 0xa967 };
const Color GREEN = {.raw = (uint16_t) 0x8568 };
const Color BLUE = {.raw = (uint16_t) 0x857a };
const Color CYAN = {.raw = (uint16_t) 0x871a };
const Color PINK = {.raw = (uint16_t) 0x839a };
const Color ORANGE = {.raw = (uint16_t) 0xc306 };

// Variables
int screenFd = -1;
int gamepadFd = -1;
uint16_t *screenBuffer;
GamepadState gamepadValues;
GamepadState gamepadChanges;
void (*gamepadSignalCallback)() = NULL;

// Access to the screen (driver) and allocate memory in it
bool initScreen() {
	// Open as writable device
	screenFd = open (SCREEN_FILE, O_RDWR);
	if (screenFd == -1) {
		printf ("Failed to open file %s.\n", SCREEN_FILE);
		return false;
	}

	// Extract screen info
	struct fb_var_screeninfo info;
	if (ioctl (screenFd, FBIOGET_VSCREENINFO, &info)) {
		printf ("Failed to extract screen info.\n");
		return false;
	}
	printf ("Supported screen: width=%d height=%d bpp=%d\n", SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_PIXEL_SIZE * 8);
	printf ("Actual screen: width=%d height=%d bpp=%d\n", info.xres, info.yres, info.bits_per_pixel);

	// Memory map it
	screenBuffer = (uint16_t *) mmap (NULL, SCREEN_BUFFER_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, screenFd, 0);
	if (screenBuffer == MAP_FAILED) {
		screenBuffer = NULL;
		printf ("Failed to memory map screen.\n");
		return false;
	}

	return true;
}

// Close the screen (driver) and deallocate its memory
void deinitScreen() {
	if (screenFd != -1) {
		if (screenBuffer)
			munmap (screenBuffer, SCREEN_BUFFER_SIZE);
		close(screenFd);
	}
}

// Open the gamepad (driver) and make its signal accesible
bool initGamepad() {
	// Open as readable device
	gamepadFd = open(GAMEPAD_FILE, O_RDONLY);
	if (gamepadFd == -1) {
		printf ("Failed to open file %s.\n", GAMEPAD_FILE);
		return false;
	}

	// Enable signals
	if (signal (SIGIO, &gamepadSignalHandler) == SIG_ERR) {
		printf ("Failed to set signal handler.\n");
		return false;
	}
	if (fcntl (gamepadFd, F_SETOWN, getpid ()) == -1) {
		printf ("Failed to set file owner.\n");
		return false;
	}
	long flags = fcntl (gamepadFd, F_GETFL);
	if (fcntl (gamepadFd, F_SETFL, flags | FASYNC) == -1) {
		printf ("Failed to set FASYNC flag.\n");
		return false;
	}

	return true;
}

// Close the gamepad (driver)
void deinitGamepad() {
	if (gamepadFd)
		close(gamepadFd);
}

// Read gamepad signal status
static void readGamepad() {
	GamepadState newState;
	read(gamepadFd, &(newState.raw), 1);
	gamepadChanges.raw = newState.raw ^ gamepadValues.raw;
	gamepadValues.raw = newState.raw;
}

// Read the call back signal
static void gamepadSignalHandler(int sigNo) {
	readGamepad();
	if (gamepadSignalCallback)
		(*gamepadSignalCallback)();
}

// Update the call back signal
void setGamepadSignalCallback(void (*callback)()) {
	gamepadSignalCallback = callback;
}

// Get the gamepad values
GamepadState getGamepadValues() {
	return gamepadValues;
}

// Get gamepad changes and clear them
GamepadState getGamepadChangesAndClear() {
	GamepadState currentChanges = gamepadChanges;
	gamepadChanges.raw = 0;
	return currentChanges;
}

// Fill the screen completely with one colour
inline void drawFullScreen(Color color) {
	drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, color);
}

// Draw a rectangle in one specific colour
void drawRect (unsigned int x1, unsigned int y1, unsigned int width, unsigned int height, Color color) {
	if (width == 0 || height == 0)
		return;		// Zero area

	unsigned int x2 = x1 + width - 1;
	unsigned int y2 = y1 + height - 1;
	if (x1 >= SCREEN_WIDTH || x2 > SCREEN_WIDTH || y1 >= SCREEN_HEIGHT || y2 > SCREEN_HEIGHT)
		return;		// Illegal area

	for (int y = y1; y < y2; y++) {
		int i = y * SCREEN_WIDTH + x1;
		for (int x = x1; x < x2; x++) {
			screenBuffer[i] = color.raw;
			i++;
		}
	}
}

// Update screen partially with new data
void updatePartialScreen (unsigned int x, unsigned int y, unsigned int width, unsigned int height) {
	struct fb_copyarea area = {x, y, width, height, 0, 0};
	ioctl(screenFd, 0x4680, &area);
}

// Update screen completely with new data
inline void updateFullScreen() {
	updatePartialScreen(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
}