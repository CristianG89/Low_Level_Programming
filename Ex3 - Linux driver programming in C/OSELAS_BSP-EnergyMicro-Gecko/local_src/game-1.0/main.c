////////////////////////////////
// EXERCISE 3 - MAIN, TDT4258 //
////////////////////////////////

#include "main.h"

// Function prototypes (Static functions shall NOT be defined in the header files, to ensure all processes have access to them...)
static bool init(void);			// Sets the screen and the gamepad to start the game
static void deinit(void);		// Free the screen and the gamepad to exit the game
static void run_game(void);		// Execute the complete game

// Constant variables
static const bool AUTO_RESTART = true;			// Set true to restart another game (after finished)
static const bool NO_TICK_SLEEP = false;		// Set true to run at full speed (for testing purposes only)
static const unsigned int TICK_PERIOD = 33333;	// Inverse of game tick/frame rate (0.033s/30Hz). In microseconds
static const unsigned int BEGIN_DELAY = 1;		// Time (in seconds) to display the begin screen, before starting the game.
static const unsigned int END_DELAY = 2;		// Time (in seconds) to display the end screen, before exiting the game.

// Sets the screen and the gamepad to start the game
static bool init() {
	printf ("Initializing game... \n");
	if (!initScreen() || !initGamepad()) {
		printf ("Initialization failed \n");
		return false;
	}
	printf ("Initialization successfully done \n");
	return true;
}

// Free the screen and the gamepad to exit the game
static void deinit() {
	printf ("Exiting game...\n");
	deinitScreen();
	deinitGamepad();
	printf ("Exit successfully done\n");
}

// Execute the complete game
static void run_game() {
	printf("Running game...\n");
	if (NO_TICK_SLEEP)
		printf("WARNING: Running without sleep!! \n");

	initGame();						// Initialization of the game interface
	beginGame();					// Game start
	sleep(BEGIN_DELAY);				// Time in seconds to keep "begin screen" displayed
	
	while (true) {					// Display is refreshed in an infinite loop
		updateGame();
		if (isGameFinished())		// Finish the program if game is finished
			break;
		if (!NO_TICK_SLEEP)			// Sleep during some time in order to save energy
			usleep(TICK_PERIOD);
	}
	
	endGame();						// Game finish
	sleep(END_DELAY);				// Time in seconds to keep "end screen" displayed
	deinitGame();					// Exit of the game interface
}

// Main function
int main(int argc, char *argv[]) {
	if (init()) {					// Run the game only if initialization was successful
		do {run_game();}
		while (AUTO_RESTART);		// While this variable true, run a new game when the previous one finishes
	}
	deinit();						// Deallocate everything to finish the game
	exit(EXIT_SUCCESS);
}