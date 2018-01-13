/*
	Author: Hai Yang Xu
	Student ID: <redacted>
	Author: John Beckingham
	Student ID: <redacted>

	Tetris 2 assets.
*/

#pragma once

// Get a random block with random rotation and set it as the current block
void getRandomBlock(bool target[]);

// Rotate current block to the left (counterclockwise) or right (clockwise)
void getLeftRotation(bool target[]);
void getRightRotation(bool target[]);

// Rotate current block, but not retrieving the rotated block, useful when
// rotated block does not fit and needs to go back
void setLeftRotation();
void setRightRotation();

// Get a random color
void getRandomColor(uint16_t &target);
