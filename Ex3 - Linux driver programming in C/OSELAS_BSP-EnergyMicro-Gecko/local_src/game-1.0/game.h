#ifndef GAME_H			//Statement to be sure this header file cannot be called more than once
#define GAME_H


#include "main.h"

// Structure
typedef struct {
	bool collideWithBrick;
	int bricks[6][6];
	int player_y;
	int player_sy;
	int ball_x;
	int ball_y;
	int ball_sx;
	int ball_sy;
	int lastball_x;
	int lastball_y;
	int lastplayer_y;
} World;

// Function prototypes (Only non-static functions are defined in the header files)
void initGame(void);					// Initialization of the game interface
void deinitGame(void);					// Exiting the game interface
void beginGame(void);					// Game start
void endGame(void);						// Game finish
bool isGameFinished(void);				// Check if game is already finished
void updateGame(void);					// Read the gamepad again and refresh the screen


#endif // GAME_H