/*
	Author: Hai Yang Xu
	Student ID: <redacted>
	Author: John Beckingham
	Student ID: <redacted>

	Tetris 2 game engine.
*/

#include <Arduino.h>
#include <Adafruit_ILI9341.h>

#include "constants.h"
#include "assets.h"
#include "engine.h"
#include "graphics.h"

// Game mode
enum GameMode { ONE_PLAYER, TWO_PLAYER };
GameMode gameMode;

// Block drop period in milliseconds
int msPerDrop;

// Current score, only used in one-player mode
uint32_t score;
// Get current score
uint32_t getCurrentScore() {
	return score;
}
// Whether this player lost, only used in two-player mode
bool lost = false;
// Get if this player lost
bool getIfLost() {
	return lost;
}

// Current block
bool currentBlock[4 * 4];
uint16_t currentColor;

uint32_t lastTime;
int x;
int y;

// Input control
#define INPUT_HOLD 200
uint32_t lastLeftTime;
uint32_t lastRightTime;
uint32_t lastUpTime;
uint32_t lastDownTime;

// Reset the state of the current block, and generate a new block
void generateNewBlock() {
	getRandomBlock(currentBlock);
	getRandomColor(currentColor);

	lastTime = millis();
	x = SCREEN_WIDTH / 2 - 2;
	y = -4;
}

// Check for non-removable rows sent from opponent then generate new block
void generateNewBlockMiddleware(Adafruit_ILI9341 &tft) {
	// Check for non-removable rows
	if (gameMode == TWO_PLAYER) {
		while (Serial3.available() > 0) {
			char temp = Serial3.read();
			// One block can clear up to 4 lines
			bool moved = false;
			if (temp == '1') {
				moved = true;
				if (!mapMoveUp(1)) {
					lost = true;
				}
			} else if (temp == '2') {
				moved = true;
				if (!mapMoveUp(2)) {
					lost = true;
				}
			} else if (temp == '3') {
				moved = true;
				if (!mapMoveUp(3)) {
					lost = true;
				}
			} else if (temp == '4') {
				moved = true;
				if (!mapMoveUp(4)) {
					lost = true;
				}
			}

			// Redraw map if needed
			if (moved && !lost) {
				mapRedraw(tft);
			}
		}
	}

	// Generate new block
	generateNewBlock();
}
// Check for full rows and send non-removable rows to opponent if needed
void mapRemoveFullRowsMiddleware(Adafruit_ILI9341 &tft) {
	// Check for full rows
	uint32_t rowsRemoved = mapRemoveFullRows(tft);
	if (rowsRemoved == 0UL) {
		return;
	}

	// Update score
	if (gameMode == ONE_PLAYER) {
		score = score + rowsRemoved;
	}

	// Send non-removable rows
	if (gameMode == TWO_PLAYER) {
		Serial3.print((int)rowsRemoved);
	}
}

// Initialize one-player mode
void initOnePlayer(Adafruit_ILI9341 &tft) {
	// Set game mode
	gameMode = ONE_PLAYER;

	// Clear screen
	tft.fillScreen(ILI9341_BLACK);
	mapClear();

	// Reset basic state
	msPerDrop = 500;
	score = 0UL;

	// Reset block state
	generateNewBlock();

	// Reset input state
	uint32_t lastLeftTime = 0UL;
	uint32_t lastRightTime = 0UL;
	uint32_t lastUpTime = 0UL;
	uint32_t lastDownTime = 0UL;
}
// Tick for one-player mode, returns whether the game ended
bool onePlayerTick(
	bool up, bool down, bool left, bool right, bool btn,
	Adafruit_ILI9341 &tft
) {
	uint32_t now = millis();

	// Process block drop
	if (lastTime + msPerDrop < now) {
		lastTime = now;

		if (mapCheckCollision(x, y + 1, currentBlock)) {
			// Can't move further
			if (y < 0) {
				// Out of space, GG
				return true;
			} else {
				// Commit this block
				commitBlock(x, y, currentBlock, currentColor);
				// Check for row completion then generate a new block
				mapRemoveFullRowsMiddleware(tft);
				generateNewBlockMiddleware(tft);
			}
		} else {
			// Drop one "pixel"
			moveBlock(x, y, 0, 1, currentBlock, currentColor, tft);
		}
	}

	// Apply input control
	if (left) {
		if (lastLeftTime + INPUT_HOLD > now) {
			// Too soon for another tick
			left = false;
		} else {
			// Save the new timestamp and let this tick though
			lastLeftTime = now;
		}
	}
	if (right) {
		if (lastRightTime + INPUT_HOLD > now) {
			right = false;
		} else {
			lastRightTime = now;
		}
	}
	if (up) {
		if (lastUpTime + INPUT_HOLD * 2 > now) {
			up = false;
		} else {
			lastUpTime = now;
		}
	}
	if (down) {
		if (lastDownTime + INPUT_HOLD * 2 > now) {
			down = false;
		} else {
			lastDownTime = now;
		}
	}

	// Handle player input
	if (left) { // Move left
		if (!mapCheckCollision(x - 1, y, currentBlock)) {
			moveBlock(x, y, -1, 0, currentBlock, currentColor, tft);
		}
	} else if (right) { // Move right
		if (!mapCheckCollision(x + 1, y, currentBlock)) {
			moveBlock(x, y, 1, 0, currentBlock, currentColor, tft);
		}
	}

	if (up) { // Rotate right (clockwise)
		// Get the rotated block
		bool tempBlock[4 * 4];
		getRightRotation(tempBlock);

		if (mapCheckCollision(x, y, tempBlock)) {
			// Can't fit, rotate back
			setLeftRotation();
		} else {
			// Undraw current block
			undrawBlock(x, y, currentBlock, tft);
			// Save the rotated block then draw it
			memcpy(currentBlock, tempBlock, 4 * 4 * sizeof(bool));
			drawBlock(x, y, currentBlock, currentColor, tft);
		}
	} else if (down) { // Rotate left (counterclockwise)
		bool tempBlock[4 * 4];
		getLeftRotation(tempBlock);

		if (mapCheckCollision(x, y, tempBlock)) {
			setRightRotation();
		} else {
			undrawBlock(x, y, currentBlock, tft);
			memcpy(currentBlock, tempBlock, 4 * 4 * sizeof(bool));
			drawBlock(x, y, currentBlock, currentColor, tft);
		}
	}

	if (btn) { // Joystick button, drop faster
		msPerDrop = 50;
	} else {
		msPerDrop = 500;
	}

	// Game has not ended
	return false;
}

// Initialize two-player mode
void initTwoPlayer(Adafruit_ILI9341 &tft) {
	// Call "super"
	initOnePlayer(tft);

	// Set game mode
	gameMode = TWO_PLAYER;
	// Reset lost flag
	lost = false;
}
// Tick for two-player mode, returns whether the game ended
bool twoPlayerTick(
	bool up, bool down, bool left, bool right, bool btn,
	Adafruit_ILI9341 &tft
) {
	// Check for game end
	if (lost) {
		return true;
	}

	// Check for opponent lose
	if (Serial3.available() > 0) {
		if (Serial3.peek() == 'E') {
			Serial3.read();
			return true;
		}
	}

	// Call "super"
	if (onePlayerTick(up, down, left, right, btn, tft)) {
		// Tell opponent that this player lost
		Serial3.print('E');
		lost = true;
		return true;
	} else {
		return false;
	}
}
