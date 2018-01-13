/*
	Author: Hai Yang Xu
	Student ID: <redacted>
	Author: John Beckingham
	Student ID: <redacted>

	Tetris 2 graphics library.
*/

#include <Arduino.h>
#include <Adafruit_ILI9341.h>

#include "constants.h"
#include "assets.h"
#include "graphics.h"

// Special colors
// Stone is non-removable rows from opponent, rgb(153, 153, 153)
#define STONE        0x9CD3U
#define OUT_OF_BOUND ILI9341_WHITE
#define BACKGROUND   ILI9341_BLACK

// Current game background map
uint16_t backgroundMap[SCREEN_WIDTH * SCREEN_HEIGHT];

// Check if a point (x, y) is within bounds of the game map
bool isWithinBounds(int x, int y) {
	if (x < 0 || x >= SCREEN_WIDTH) {
		return false;
	}
	if (y < 0 || y >= SCREEN_HEIGHT) {
		return false;
	}

	return true;
}

// Draw a 10 by 10 "pixel"
void drawPixel(
	int x, int y,
	uint16_t color, Adafruit_ILI9341 &tft
) {
	if (isWithinBounds(x, y)) {
		tft.fillRect(
			x * BLOCK_SIZE, y * BLOCK_SIZE,
			BLOCK_SIZE, BLOCK_SIZE,
			color
		);
	}
}
// Draw a "pixel" only when it is part of a block
// i is column index and j is row index
void drawPixelIf(
	int x, int y,
	int i, int j,
	bool block[], uint16_t color,
	Adafruit_ILI9341 &tft
) {
	if (block[i + j * 4]) {
		drawPixel(x + i, y + j, color, tft);
	}
}

// Safely get the value of game map at point (x, y)
// Returns a collision placeholder if the point is outside of the bounds of
// the game map
uint16_t mapSafeGetValue(int x, int y) {
	if (isWithinBounds(x, y)) {
		return backgroundMap[x + y * SCREEN_WIDTH];
	} else {
		// This will make collision detection easier
		if (y < 0 && x >= 0 && x < SCREEN_WIDTH) {
			return BACKGROUND;
		} else {
			return OUT_OF_BOUND;
		}
	}
}
// Safely set the value of background map at point (x, y)
// Returns true if the the operation succeeded, false otherwise
bool mapSafeSetValue(int x, int y, uint16_t color) {
	if (isWithinBounds(x, y)) {
		backgroundMap[x + y * SCREEN_WIDTH] = color;
		return true;
	} else {
		return false;
	}
}

// Clear the game background map
void mapClear() {
	for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
		backgroundMap[i] = BACKGROUND;
	}
}
// Check if a block moved to the point (x, y) will overlap part of the current
// game map
bool mapCheckCollision(int x, int y, bool block[]) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (block[i + j * 4] && mapSafeGetValue(x + i, y + j) != BACKGROUND) {
				return true;
			}
		}
	}
	return false;
}
// Redraw whole map
void mapRedraw(Adafruit_ILI9341 &tft) {
	for (int x = 0; x < SCREEN_WIDTH; x++) {
		for (int y = 0; y < SCREEN_HEIGHT; y++) {
			drawPixel(x, y, backgroundMap[x + y * SCREEN_WIDTH], tft);
		}
	}
}
// Remove full rows from the map and return the number of rows removed
// Will destroy blocks that are not committed
uint32_t mapRemoveFullRows(Adafruit_ILI9341 &tft) {
	uint32_t removed = 0;

	for (int y = SCREEN_HEIGHT - 1; y >= 0; y--) {
		// Check if current row is full
		bool isFull = true;
		for (int x = 0; x < SCREEN_WIDTH; x++) {
			if (
				backgroundMap[x + y * SCREEN_WIDTH] == BACKGROUND ||
				backgroundMap[x + y * SCREEN_WIDTH] == STONE
			) {
				isFull = false;
				break;
			}
		}

		if (isFull) {
			// Shift game map down one row
			if (y > 0) {
				memmove(
					backgroundMap + SCREEN_WIDTH, backgroundMap,
					y * SCREEN_WIDTH * sizeof(uint16_t)
				);
			}
			// Fill the first row with background
			for (int x = 0; x < SCREEN_WIDTH; x++) {
				backgroundMap[x] = BACKGROUND;
			}
			// Update counter and rollback y so the new row at current position
			// can be checked
			removed++;
			y++;
		}
	}

	// Redraw displayed map if needed
	if (removed > 0) {
		mapRedraw(tft);
	}

	return removed;
}
// Move the map up (if possible), returns whether it's possible
bool mapMoveUp(int howMany) {
	int toCheck = howMany * SCREEN_WIDTH;
	// Check if top rows are empty
	for (int i = 0; i < toCheck; i++) {
		if (backgroundMap[i] != BACKGROUND) {
			return false;
		}
	}

	// Move rows up
	memmove(
		backgroundMap, backgroundMap + toCheck,
		(SCREEN_WIDTH * SCREEN_HEIGHT - toCheck) * sizeof(uint16_t)
	);

	// Fill bottom rows with stone
	for (
		int i = SCREEN_WIDTH * SCREEN_HEIGHT - toCheck;
		i < SCREEN_WIDTH * SCREEN_HEIGHT;
		i++
	) {
		backgroundMap[i] = STONE;
	}

	return true;
}

// Draw a block to a specific position
void drawBlock(
	int x, int y,
	bool block[], uint16_t color,
	Adafruit_ILI9341 &tft
) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			drawPixelIf(x, y ,i, j, block, color, tft);
		}
	}
}
// Undraw current block
void undrawBlock(
	int x, int y,
	bool block[],
	Adafruit_ILI9341 &tft
) {
	drawBlock(x, y, block, BACKGROUND, tft);
}
// Move current block
void moveBlock(
	int &x, int &y,
	int dx, int dy,
	bool block[], uint16_t color,
	Adafruit_ILI9341 &tft
) {
	// Undraw the block and draw it again at the new position
	undrawBlock(x, y, block, tft);
	drawBlock(x + dx, y + dy, block, color, tft);

	// Update x and y
	x = x + dx;
	y = y + dy;
}
// Commit current block onto game map
void commitBlock(
	int x, int y,
	bool block[], uint16_t color
) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (block[i + j * 4]) {
				mapSafeSetValue(x + i, y + j, color);
			}
		}
	}
}

// Test move block
void testMoveBlock(Adafruit_ILI9341 &tft) {
	// Generate a random block
	bool b1[4 * 4];
	uint16_t color;
	getRandomBlock(b1);
	getRandomColor(color);

	// Put the block onto the display
	int x = SCREEN_WIDTH / 2 - 2;
	int y = SCREEN_WIDTH / 2 - 2;
	drawBlock(x, y, b1, color, tft);
	delay(1000);

	// Right
	moveBlock(x, y, 1, 0, b1, color, tft);
	delay(1000);
	moveBlock(x, y, 2, 0, b1, color, tft);
	delay(1000);
	// Left
	moveBlock(x, y, -1, 0, b1, color, tft);
	delay(1000);
	moveBlock(x, y, -2, 0, b1, color, tft);
	delay(1000);
	// Down
	moveBlock(x, y, 0, 1, b1, color, tft);
	delay(1000);
	moveBlock(x, y, 0, 2, b1, color, tft);
	delay(1000);
	// Up
	moveBlock(x, y, 0, -1, b1, color, tft);
	delay(1000);
	moveBlock(x, y, 0, -2, b1, color, tft);
	delay(1000);
}
