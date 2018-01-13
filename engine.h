/*
	Author: Hai Yang Xu
	Student ID: <redacted>
	Author: John Beckingham
	Student ID: <redacted>

	Tetris 2 game engine.
*/

#pragma once

// Reset and get ready for one-player mode
void initOnePlayer(Adafruit_ILI9341 &tft);
// Tick for one-player mode
bool onePlayerTick(
	bool up, bool down, bool left, bool right, bool btn,
	Adafruit_ILI9341 &tft
);

// Reset and get ready for two-player mode
void initTwoPlayer(Adafruit_ILI9341 &tft);
// Tick for two-player mode
bool twoPlayerTick(
	bool up, bool down, bool left, bool right, bool btn,
	Adafruit_ILI9341 &tft
);

// Get the current game score, it is only reliable after the game-ending tick
// finishes, only for one-player mode
uint32_t getCurrentScore();
// Get whether this player lost, only for two-player mode, only reliable after
// game ended like getCurrentScore()
bool getIfLost();
