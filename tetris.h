#pragma once

#include <gfxfont.h>
#include <Adafruit_SPITFT_Macros.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_GFX.h>
#include <RGBmatrixPanel.h>

// 64x32 Matrix connections to Arduino
#define CLK		11
#define OE		9
#define LAT		10
#define A		A0
#define B		A1
#define C		A2
#define D		A3

// Game related information about board
#define MAX_SHAPES		164
#define PLAYABLE_HEIGHT 46
#define BITS			16
#define FLOOR_INDEX		40
#define LEFT_LIMIT		1
#define RIGHT_LIMIT		28

#define START_X			16
#define START_Y			4
#define START_ROT		0

#define BIT_X			i%4 + currentShape.x
#define BIT_Y			floor(i / 4) + currentShape.y
#define SHAPE_COLOR		COLORS[currentShape.id]

#define ROTATION_COUNT	4
#define SHAPE_COUNT	    7

#define JX				 8	// Joystick X-axis
#define JY				 9  // Joystick Y-axis
#define JS				52  // Josytick Switch

#define INT8_MAX       0b11111111
#define MOVE_DOWN_FQ   4	// Once ever 6 loops

// Colors
#define RED matrix.Color333(4, 0, 0)
#define BLUE matrix.Color333(0, 0, 4)
#define GREEN matrix.Color333(0, 4, 0)
#define PURPLE matrix.Color333(3, 0, 3)
#define ORANGE matrix.Color333(3, 3, 0)
#define CYAN matrix.Color333(0, 3, 3)
#define PINK matrix.Color333(3, 1, 2)
#define WHITE matrix.Color333(2, 2, 2)
#define BLACK matrix.Color333(0, 0, 0)

RGBmatrixPanel matrix(A, B, C, D, CLK, LAT, OE, true, 64);
const uint16_t COLORS[SHAPE_COUNT + 1] = {BLACK, RED, GREEN, BLUE, PURPLE, ORANGE, CYAN, PINK };

struct shapePixel {
	uint8_t col : 4;
};
