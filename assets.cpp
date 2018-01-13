/*
	Author: Hai Yang Xu
	Student ID: <redacted>
	Author: John Beckingham
	Student ID: <redacted>

	Tetris 2 assets.
*/


#include <Arduino.h>
#include <Adafruit_ILI9341.h>

#include "constants.h"
#include "assets.h"


// Block constants, b_<index>_<rotation>, from
// http://www.colinfahey.com/tetris/tetris_diagram_pieces_orientations_new.jpg
const bool b_0_0[4 * 4] = {
	0, 0, 0, 0,
	0, 1, 1, 0,
	0, 1, 1, 0,
	0, 0, 0, 0
};


const bool b_1_0[4 * 4] = {
	0, 0, 0, 0,
	1, 1, 1, 1,
	0, 0, 0, 0,
	0, 0, 0, 0
};
const bool b_1_1[4 * 4] = {
	0, 1, 0, 0,
	0, 1, 0, 0,
	0, 1, 0, 0,
	0, 1, 0, 0
};


const bool b_2_0[4 * 4] = {
	0, 0, 0, 0,
	0, 0, 1, 1,
	0, 1, 1, 0,
	0, 0, 0, 0
};
const bool b_2_1[4 * 4] = {
	0, 0, 1, 0,
	0, 0, 1, 1,
	0, 0, 0, 1,
	0, 0, 0, 0
};


const bool b_3_0[4 * 4] = {
	0, 0, 0, 0,
	0, 1, 1, 0,
	0, 0, 1, 1,
	0, 0, 0, 0
};
const bool b_3_1[4 * 4] = {
	0, 0, 0, 1,
	0, 0, 1, 1,
	0, 0, 1, 0,
	0, 0, 0, 0
};


const bool b_4_0[4 * 4] = {
	0, 0, 0, 0,
	0, 1, 1, 1,
	0, 1, 0, 0,
	0, 0, 0, 0
};
const bool b_4_1[4 * 4] = {
	0, 0, 1, 0,
	0, 0, 1, 0,
	0, 0, 1, 1,
	0, 0, 0, 0
};
const bool b_4_2[4 * 4] = {
	0, 0, 0, 1,
	0, 1, 1, 1,
	0, 0, 0, 0,
	0, 0, 0, 0
};
const bool b_4_3[4 * 4] = {
	0, 1, 1, 0,
	0, 0, 1, 0,
	0, 0, 1, 0,
	0, 0, 0, 0
};


const bool b_5_0[4 * 4] = {
	0, 0, 0, 0,
	0, 1, 1, 1,
	0, 0, 0, 1,
	0, 0, 0, 0
};
const bool b_5_1[4 * 4] = {
	0, 0, 1, 1,
	0, 0, 1, 0,
	0, 0, 1, 0,
	0, 0, 0, 0
};
const bool b_5_2[4 * 4] = {
	0, 1, 0, 0,
	0, 1, 1, 1,
	0, 0, 0, 0,
	0, 0, 0, 0
};
const bool b_5_3[4 * 4] = {
	0, 0, 1, 0,
	0, 0, 1, 0,
	0, 1, 1, 0,
	0, 0, 0, 0
};


const bool b_6_0[4 * 4] = {
	0, 0, 0, 0,
	0, 1, 1, 1,
	0, 0, 1, 0,
	0, 0, 0, 0
};
const bool b_6_1[4 * 4] = {
	0, 0, 1, 0,
	0, 0, 1, 1,
	0, 0, 1, 0,
	0, 0, 0, 0
};
const bool b_6_2[4 * 4] = {
	0, 0, 1, 0,
	0, 1, 1, 1,
	0, 0, 0, 0,
	0, 0, 0, 0
};
const bool b_6_3[4 * 4] = {
	0, 0, 1, 0,
	0, 1, 1, 0,
	0, 0, 1, 0,
	0, 0, 0, 0
};


// Size on memory of each block
const int SIZE = 4 * 4 * sizeof(bool);

// Current block
uint8_t currBlock = 0;
uint8_t currRotation = 0;


// Get a specific block and rotation
void getBlock(bool target[], uint8_t index, uint8_t rotation){
	// Save state
	currBlock = index;
	currRotation = rotation;

	// Copy over the block
	switch (index % 7) {
		case 0:
			memcpy(target, b_0_0, SIZE);
			break;

		case 1:
			if (rotation % 2) {
				memcpy(target, b_1_1, SIZE);
			} else {
				memcpy(target, b_1_0, SIZE);
			}
			break;

		case 2:
			if (rotation % 2) {
				memcpy(target, b_2_1, SIZE);
			} else {
				memcpy(target, b_2_0, SIZE);
			}
			break;

		case 3:
			if (rotation % 2) {
				memcpy(target, b_3_1, SIZE);
			} else {
				memcpy(target, b_3_0, SIZE);
			}
			break;

		case 4:
			switch(rotation % 4) {
				case 0:
					memcpy(target, b_4_0, SIZE);
					break;
				case 1:
					memcpy(target, b_4_1, SIZE);
					break;
				case 2:
					memcpy(target, b_4_2, SIZE);
					break;
				case 3:
					memcpy(target, b_4_3, SIZE);
					break;
			}
			break;

		case 5:
			switch(rotation % 4) {
				case 0:
					memcpy(target, b_5_0, SIZE);
					break;
				case 1:
					memcpy(target, b_5_1, SIZE);
					break;
				case 2:
					memcpy(target, b_5_2, SIZE);
					break;
				case 3:
					memcpy(target, b_5_3, SIZE);
					break;
			}
			break;

		case 6:
			switch(rotation % 4) {
				case 0:
					memcpy(target, b_6_0, SIZE);
					break;
				case 1:
					memcpy(target, b_6_1, SIZE);
					break;
				case 2:
					memcpy(target, b_6_2, SIZE);
					break;
				case 3:
					memcpy(target, b_6_3, SIZE);
					break;
			}
			break;
	}
}

// Get a random block with random rotation
void getRandomBlock(bool target[]) {
	#ifdef CHEAT
		getBlock(target, 1U, 0U); // 4 in a row
	#else
		getBlock(target, (uint8_t)random(0, 8), (uint8_t)random(0, 4));
	#endif
}

// Rotate current block to the left or right
void getLeftRotation(bool target[]) {
	currRotation--;
	getBlock(target, currBlock, currRotation);
}
void getRightRotation(bool target[]) {
	currRotation++;
	getBlock(target, currBlock, currRotation);
}

// Rotate current block but not retrieving it
void setLeftRotation() {
	currRotation--;
}
void setRightRotation() {
	currRotation++;
}


// Color constants
#define COLORS_LEN 6
const uint16_t colors[COLORS_LEN] = {
	ILI9341_BLUE, ILI9341_RED, ILI9341_GREEN,
	ILI9341_CYAN, ILI9341_MAGENTA, ILI9341_YELLOW
};


// Get a random color
void getRandomColor(uint16_t &target) {
	target = colors[random(0, COLORS_LEN)];
}
