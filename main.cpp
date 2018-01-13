/*
	Author: Hai Yang Xu
	Student ID: <redacted>
	Author: John Beckingham
	Student ID: <redacted>

	Tetris 2 bootstrapper.
*/

#include <Arduino.h>
#include <Adafruit_ILI9341.h>

#include "constants.h"
#include "assets.h"
#include "engine.h"
#include "graphics.h"

// Set up display controller instance
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

// Implemented later
void setup();
void loop();

// The entry point of the program
int main() {
	setup();
	while (true) {
		loop();
	}
	return 0;
}

// Draw a simple splash screen
void splashScreen() {
	// Clear the screen
	tft.fillScreen(ILI9341_BLACK);

	// Draw one random block on each of the quarters of the screen
	bool tempBlock[4 * 4];
	uint16_t tempColor;

	getRandomBlock(tempBlock);
	getRandomColor(tempColor);
	drawBlock(
		random(0, SCREEN_WIDTH / 2), random(0, SCREEN_HEIGHT / 2),
		tempBlock, tempColor, tft
	);

	getRandomBlock(tempBlock);
	getRandomColor(tempColor);
	drawBlock(
		random(0, SCREEN_WIDTH / 2), random(SCREEN_HEIGHT / 2, SCREEN_HEIGHT),
		tempBlock, tempColor, tft
	);

	getRandomBlock(tempBlock);
	getRandomColor(tempColor);
	drawBlock(
		random(SCREEN_WIDTH / 2, SCREEN_WIDTH), random(0, SCREEN_HEIGHT / 2),
		tempBlock, tempColor, tft
	);

	getRandomBlock(tempBlock);
	getRandomColor(tempColor);
	drawBlock(
		random(SCREEN_WIDTH / 2, SCREEN_WIDTH),
		random(SCREEN_HEIGHT / 2, SCREEN_HEIGHT),
		tempBlock, tempColor, tft
	);

	// Draw prompt text
	tft.setTextSize(2);
	tft.setTextColor(ILI9341_WHITE);

	tft.setCursor(10, 200);
	tft.print("RIGHT - One-Player");
	tft.setCursor(10, 250);
	tft.print("LEFT  - Two-Player");
}

// Initialization
void setup() {
	init();

	// Serial is only used for debug logs (if there are any)
	#ifdef DEBUG
		Serial.begin(9600);
	#endif

	// Serial3 is for two-player mode
	Serial3.begin(9600);

	// Initialize display
	tft.begin();
	tft.setRotation(0);

	// Initialize joystick button
	pinMode(JOY_BTN, INPUT_PULLUP);

	// Seed random number generator, the seed is pinned in debug mode to
	// facilitate debugging
	#ifdef DEBUG
		randomSeed(123);
	#else
		randomSeed(analogRead(15));
	#endif

	// Run tests if needed
	#if defined(DEBUG) && defined(RUN_TESTS)
		testMoveBlock(tft);
	#endif

	// Draw splash screen for the first time
	splashScreen();
}

// Main loop
void loop() {
	// System state
	enum State {
		SPLASH_SCREEN,
		ONE_PLAYER, ONE_PLAYER_END,
		TWO_PLAYER_HANDSHAKE, TWO_PLAYER, TWO_PLAYER_END
	};
	static State state = SPLASH_SCREEN;

	// Process joystick input
	int joyX = analogRead(JOY_HORIZ);
	int joyY = analogRead(JOY_VERT);
	bool up, down, left, right, btn;

	if (joyY < (JOY_CENTER - JOY_DEADZONE)) { // Y axis
		up = true;  down = false;
	} else if (joyY > (JOY_CENTER + JOY_DEADZONE)) {
		up = false; down = true;
	} else {
		up = false; down = false;
	}

	if (joyX < (JOY_CENTER - JOY_DEADZONE)) { // X axis
		left = true;  right = false;
	} else if (joyX > (JOY_CENTER + JOY_DEADZONE)) {
		left = false; right = true;
	} else {
		left = false; right = false;
	}

	btn = digitalRead(JOY_BTN) == LOW; // Button

	if (state == SPLASH_SCREEN) {
		if (right) {
			// One-player mode
			initOnePlayer(tft);
			state = ONE_PLAYER;
		}
		if (left) {
			// Two-player mode
			state = TWO_PLAYER_HANDSHAKE;
		}
	} else if (state == ONE_PLAYER) {
		if (onePlayerTick(up, down, left, right, btn, tft)) {
			// Return true means game ended, draw end screen
			tft.setTextSize(2);
			tft.setTextColor(ILI9341_WHITE);
			tft.setCursor(10, 150);
			tft.print("Game Over, Score:");
			tft.setCursor(10, 200);
			tft.print(getCurrentScore());

			// Wait a bit to prevent misclick
			delay(2500);
			tft.setCursor(10, 250);
			tft.print("Press Joystick...");
			// Move to end game wait mode
			state = ONE_PLAYER_END;
		}
	} else if (state == ONE_PLAYER_END || state == TWO_PLAYER_END) {
		if (btn) { // Press joystick to return to main screen
			splashScreen();
			state = SPLASH_SCREEN;
		}
	} else if (state == TWO_PLAYER_HANDSHAKE) {
		// Show wait screen
		tft.fillScreen(ILI9341_BLACK);
		tft.setTextSize(2);
		tft.setTextColor(ILI9341_WHITE);
		tft.setCursor(10, 150);
		tft.print("Connecting...");

		// Wait for the other player to enter two-player mode
		int otherReady = 0; // The number of continuous 'R' we received
		// Send 'R's
		Serial3.print("RRR");
		// Wait for 3 'R's
		while (otherReady < 3) {
			while (Serial3.available() == 0) {
				delay(1);
			}
			if (Serial3.read() == 'R') {
				otherReady++;
			} else {
				otherReady = 0;
			}
		}

		// Game start
		initTwoPlayer(tft);
		state = TWO_PLAYER;
	} else if (state == TWO_PLAYER) {
		// Pretty much the same as one-player mode, just slightly different
		// end game screen
		if (twoPlayerTick(up, down, left, right, btn, tft)) {
			tft.setTextSize(2);
			tft.setTextColor(ILI9341_WHITE);
			tft.setCursor(10, 150);
			if (getIfLost()) {
				tft.print("You LOST");
			} else {
				tft.print("You WON!");
			}

			delay(2500);
			tft.setCursor(10, 250);
			tft.print("Press Joystick...");

			state = TWO_PLAYER_END;
		}
	}
}
