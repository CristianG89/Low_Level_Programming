////////////////////////////////
// EXERCISE 3 - GAME, TDT4258 //
////////////////////////////////

#include "game.h"

// Function prototypes (Static functions shall NOT be defined in the header files, to ensure all processes have access to them...)
static void drawGame(void);				// Draw the ball and the player
static void eraseLastDrawings();		// Erase previous ball and player
static void drawOneBrick(int i, int j);	// Draw one brick of specific colour and update screen partially
static void drawAllBricks();			// To be used once (initial state), or to be (perfomance-wise) improved.
static void updateGameState(void);		// Update the status of the game (blocks and player position)

// Constants
static const unsigned int PLAYER_SPEED = 12;
static const unsigned int PLAYER_WIDTH = 25;
static const unsigned int PLAYER_HEIGHT = 5;
static const unsigned int PLAYER_POSITIONX = 200;
static const unsigned int BRICKS_WIDTH = 64;
static const unsigned int BRICKS_HEIGHT = 10;
static const unsigned int BRICKS_Y_OFFSET = 50;
static const unsigned int BRICKS_HALF_SPACING = 1;
static const unsigned int BALL_SIZE = 5;
static const unsigned int BALL_SPEED = 3;

// Variables
static bool finished;
static bool dirtyScreen;
static Color backgroundColor;
static GamepadState gamepad;
static World world;


// Draw the ball and the player
static void drawGame() {
	drawRect(world.ball_y, world.ball_x, BALL_SIZE, BALL_SIZE, WHITE);
	updatePartialScreen(world.ball_y, world.ball_x, BALL_SIZE, BALL_SIZE);
	drawRect(world.player_y, PLAYER_POSITIONX, PLAYER_WIDTH, PLAYER_HEIGHT, WHITE);
	updatePartialScreen(world.player_y, PLAYER_POSITIONX, PLAYER_WIDTH, PLAYER_HEIGHT);
}

// Erase previous ball and player
static void eraseLastDrawings() {
	// Clear full screen if dirty
	if (dirtyScreen) {
		drawFullScreen(backgroundColor);
		drawAllBricks();
		updateFullScreen();
		dirtyScreen = false;
	}

	// Erase previous ball and player
	drawRect(world.lastball_y, world.lastball_x, BALL_SIZE, BALL_SIZE, backgroundColor);
	updatePartialScreen(world.lastball_y, world.lastball_x, BALL_SIZE, BALL_SIZE);
	drawRect(world.lastplayer_y, PLAYER_POSITIONX, PLAYER_WIDTH, PLAYER_HEIGHT, backgroundColor);
	updatePartialScreen(world.lastplayer_y, PLAYER_POSITIONX, PLAYER_WIDTH, PLAYER_HEIGHT);
}

// Draw one brick of specific colour and update screen partially
static void drawOneBrick(int i, int j) {
	Color colorToUse;
	switch (world.bricks[i][j]) {
		case 0:
			colorToUse = backgroundColor;
			break;
		case 1:
			colorToUse = WHITE;
			break;
		case 2:
			colorToUse = PINK;
			break;
		case 3:
			colorToUse = CYAN;
			break;
		case 4:
			colorToUse = RED;
			break;
		case 5:
			colorToUse = GREEN;
			break;
		case 6:
			colorToUse = ORANGE;
			break;
	}
	unsigned int x = j * BRICKS_WIDTH + BRICKS_HALF_SPACING;
	unsigned int y = i * BRICKS_HEIGHT + BRICKS_Y_OFFSET + BRICKS_HALF_SPACING;
	unsigned int width = BRICKS_WIDTH - BRICKS_HALF_SPACING;
	unsigned int height = BRICKS_HEIGHT - BRICKS_HALF_SPACING;
	drawRect(x, y, width, height, colorToUse);
	updatePartialScreen(x, y, width, height);
}

// To be used once (initial state), or to be (perfomance-wise) improved.
static void drawAllBricks() {
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 5; j++)
			drawOneBrick(i, j);
	}
}

// Update the status of the game (blocks and player position)
static void updateGameState() {
	world.lastball_x = world.ball_x;
	world.lastball_y = world.ball_y;
	world.lastplayer_y = world.player_y;

	// Control player with LEFT and RIGHT
	if (gamepad.buttons.SW5_left && !gamepad.buttons.SW7_right)
		world.player_sy = -PLAYER_SPEED;
	else if (!gamepad.buttons.SW5_left && gamepad.buttons.SW7_right)
		world.player_sy = PLAYER_SPEED;
	else
		world.player_sy = 0;

	// Control player with TOP to stop game
	if (gamepad.buttons.SW1_up)
		finished = true;

	// Update player position
	world.player_y += world.player_sy;
	if (world.player_y < 0)
		world.player_y = 0;
	else if (world.player_y + PLAYER_WIDTH > SCREEN_WIDTH)
		world.player_y = SCREEN_WIDTH - PLAYER_WIDTH;

	// Update Ball position
	world.ball_x += world.ball_sx;
	world.ball_y += world.ball_sy;

	// Collisions ----- Ball-Walls
	if (world.ball_x < 0) {
		world.ball_x = 0;
		world.ball_sx = -world.ball_sx;
	} else if (world.ball_x + BALL_SIZE > SCREEN_HEIGHT)
		finished = true;		// Game is finished!

	if (world.ball_y < 0) {
		world.ball_y = 0;
		world.ball_sy = -world.ball_sy;
	} else if (world.ball_y + BALL_SIZE > SCREEN_WIDTH) {
		world.ball_y = SCREEN_WIDTH - BALL_SIZE;
		world.ball_sy = -world.ball_sy;
	}

	// Collisions ----- Ball-Player
	int ballBottom = world.ball_x + BALL_SIZE;
	int ballRight = world.ball_y + BALL_SIZE;
	int playerRight = world.player_y + PLAYER_WIDTH;
	int playerBottom = PLAYER_POSITIONX + PLAYER_HEIGHT;

	if ((ballBottom > PLAYER_POSITIONX) && (playerBottom > world.ball_x)) {
		if (!(ballRight < world.player_y) && !(playerRight < world.ball_y)) {
			// Inverse speed if ball is going wrong direction (avoids bugs)
			if (world.ball_sx > 0)
				world.ball_sx = -world.ball_sx;
		}
	}

	// Collisions ----- Ball-Bricks
	int brickBottom;
	int brickTop;
	int brickLeft;
	int brickRight;

	bool previouslyCollided = world.collideWithBrick;
	world.collideWithBrick = false;
	int i = 0;
	int j = 0;
	for (i = 0; i < 6; i++) {
		for (j = 0; j < 5; j++) {
			if (world.bricks[i][j] > 0) {
				brickTop = 50 + i * BRICKS_HEIGHT;
				brickBottom = brickTop + BRICKS_HEIGHT;
				brickLeft = j * BRICKS_WIDTH;
				brickRight = brickLeft + BRICKS_WIDTH;
				if ((ballBottom > brickTop) && (world.ball_x < brickBottom)) {
					if (!(ballRight < brickLeft) && !(world.ball_y > brickRight)) {
						world.bricks[i][j] -= 1;	// the brick looses a life.
						// Change direction if it is first tick colliding with any brick (avoid bugs)
						if (!previouslyCollided) {
							world.ball_x = world.lastball_x;
							world.ball_sx = -world.ball_sx;
						}
						world.collideWithBrick = true;
						break;
					}
				}
			}
		}
		if (world.collideWithBrick)
			break;
	}

	if (world.collideWithBrick) {
		drawOneBrick(i, j);

		// Checking if all bricks are down each time there is a collision with a brick:
		bool allBricksDown = true;
		for (i = 0; i < 6; i++) {
			for (j = 0; j < 5; j++) {
				if (world.bricks[i][j] > 0)
					allBricksDown = false;
			}
		}
		finished = allBricksDown;	// If yes, then it is a victory!
	}
}

// Initialization of the game interface
void initGame() {
	backgroundColor = BLACK;
	finished = false;
	dirtyScreen = true;

	// Initializing bricks : The value inside the array gives the "number of lives left" of a brick.
	// When equal to 0, the brick is down. The 2 dimensions of the array gives us rows and columns.
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 5; j++)
			world.bricks[i][j] = 6 - i;
	}

	// Initial game values
	world.player_y = 60;					// y-Position of the player
	world.player_sy = 0;					// y-Speed of the player
	world.ball_x = SCREEN_HEIGHT / 2;		// x-Position of the ball
	world.ball_y = SCREEN_WIDTH / 2;		// y-Position of the ball
	world.ball_sx = BALL_SPEED;				// x-Speed of the ball
	world.ball_sy = BALL_SPEED;				// y-Speed of the ball
	world.lastball_x = world.ball_x;		// previous x-Position of the ball
	world.lastball_y = world.ball_y;		// previous y-Position of the ball
	world.lastplayer_y = world.player_y;	// previous y-Position of the player
	world.collideWithBrick = false;			// boolean indicating if there was a previous collision
}

// Exiting the game interface
void deinitGame() {
	// No need to write in here
}

// Game start
void beginGame() {
	drawFullScreen(backgroundColor);
	updateFullScreen();
}

// Game finish
void endGame() {
	// Show end screen (score, "GAME OVER", ...)
	drawFullScreen(backgroundColor);
	// Placeholder for GAME OVER
	drawFullScreen(BLUE);
	drawRect(25, 25, SCREEN_WIDTH - 50, SCREEN_HEIGHT - 50, BLACK);
	updateFullScreen();
}

// Check if game is already finished
bool isGameFinished() {
	return finished;
}

// Read the gamepad again and refresh the screen
void updateGame() {
	gamepad = getGamepadValues();
	eraseLastDrawings();
	drawGame();
	updateGameState();
}