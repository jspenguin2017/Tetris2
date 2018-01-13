/*
	Author: Hai Yang Xu
	Student ID: <redacted>
	Author: John Beckingham
	Student ID: <redacted>

	Tetris 2 constants and configurations.
*/

#pragma once

// Whether debug mode is activated
//#define DEBUG
// Whether on-boot tests are activated
//#define RUN_TESTS
// Whether cheat mode is activated
//#define CHEAT

// Display pixel counts
#define LCD_HEIGHT 320
#define LCD_WIDTH  240

// Display pins
#define TFT_DC 9
#define TFT_CS 10

// Joystick pins, the axis definition here are backward from assignment 2, but
// wiring is still the same
#define JOY_HORIZ A1
#define JOY_VERT  A0
#define JOY_BTN   2

// Joystick constants
#define JOY_CENTER   512
#define JOY_DEADZONE 128

// Game "pixel"
#define BLOCK_SIZE    10
#define SCREEN_HEIGHT (LCD_HEIGHT / BLOCK_SIZE)
#define SCREEN_WIDTH  (LCD_WIDTH  / BLOCK_SIZE)
