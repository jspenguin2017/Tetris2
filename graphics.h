/*
	Author: Hai Yang Xu
	Student ID: <redacted>
	Author: John Beckingham
	Student ID: <redacted>

	Tetris 2 graphics library.
*/

#pragma once

// Clear game map
void mapClear();
// Check for collision with game map
bool mapCheckCollision(int x, int y, bool block[]);
// Redraw the entire game map
void mapRedraw(Adafruit_ILI9341 &tft);
// Remove completed rows from game map then return the number of rows removed
uint32_t mapRemoveFullRows(Adafruit_ILI9341 &tft);
// Move the whole map up, return whether the operation succeeded
bool mapMoveUp(int howMany);

// Draw a block
void drawBlock(
	int x, int y,
	bool block[], uint16_t color,
	Adafruit_ILI9341 &tft
);
// Undraw current block
void undrawBlock(
	int x, int y,
	bool block[],
	Adafruit_ILI9341 &tft
);
// Move current block, x and y will be automatically updated
void moveBlock(
	int &x, int &y,
	int dx, int dy,
	bool block[], uint16_t color,
	Adafruit_ILI9341 &tft
);
// Commit current block onto the game map
void commitBlock(
	int x, int y,
	bool block[], uint16_t color
);

// Graphical tests for moveBlock(...)
void testMoveBlock(Adafruit_ILI9341 &tft);
