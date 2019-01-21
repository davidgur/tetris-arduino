/*
	tetris.ino
	David Gurevich
	Last Updated: January 19, 2019

	Copyright (C) 2019 David Gurevich

	Permission is hereby granted, free of charge, to any person obtaining a copy of
	this software and associated documentation files (the "Software"), to deal in
	the Software without restriction, including without limitation the rights to
	use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
	of the Software, and to permit persons to whom the Software is furnished to do
	so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/


#include "tetris.h"
#include "shapes.h"
#include "splash.h"

#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))

const char* scoretxt = "SCORE:";
uint8_t SCORE = 0;

Shape collisionTestShape;
Shape currentShape;
uint8_t moveDownCounter = 0;

uint16_t walls[PLAYABLE_HEIGHT];
uint16_t currentShapeMap[PLAYABLE_HEIGHT];
uint16_t collisions[PLAYABLE_HEIGHT];
uint16_t floorObstacle[PLAYABLE_HEIGHT];
uint8_t shapeObstacles[PLAYABLE_HEIGHT][BITS];

void initializeObstacles() {
	for (uint16_t i = 0; i < PLAYABLE_HEIGHT; i++) {
		walls[i] = 0b1000000000000111;
	}
	floorObstacle[40] = 0b1111111111111111;
}
void redrawScreen() {
	// DRAW FRAME
	matrix.drawLine(10, 5, 10, 45, WHITE);
	matrix.drawLine(10, 45, 31 - 9, 45, WHITE);
	matrix.drawLine(31 - 8, 5, 31 - 8, 45, WHITE);

	matrix.setCursor(1, 52);
	matrix.setTextColor(WHITE);
	matrix.print("S:");
	matrix.println(SCORE);
}

void splashScreen() {
	matrix.drawBitmap(0, 0, splashBitmap, 32, 64, BLACK, WHITE);
	matrix.swapBuffers(false);
}

void gameOverScreen() {
	matrix.drawBitmap(0, 0, gameOverBitmap, 32, 64, BLACK, WHITE);
	matrix.swapBuffers(false);
}

bool wallCollisionDetected() {
	uint16_t collisionsSum = 0;
	for (uint16_t i = 0; i < PLAYABLE_HEIGHT; i++) {
		if ((currentShapeMap[i] & walls[i]) > 0)
			collisionsSum++;
	}
	return collisionsSum > 0;
}

bool floorCollisionDetected() {
	uint16_t collisionsSum = 0;
	for (uint16_t i = 0; i < PLAYABLE_HEIGHT; i++) {
		if ((currentShapeMap[i] & floorObstacle[i]) > 0)
			collisionsSum++;
	}

	return collisionsSum > 0;
}

bool hitCeiling() {
	for (uint8_t i = 0; i < BITS; i++) {
		if (shapeObstacles[4][i] != 0)
			return true;
	}
	return false;
}

void updateShapeMap() {
	memset(currentShapeMap, 0, sizeof currentShapeMap);
	for (uint16_t i = 0; i < BITS; i++) {
		if (CHECK_BIT(SHAPES[currentShape.id][currentShape.rot], i)) {
			uint16_t mapY = floor(i / 4) + currentShape.y - 5;
			uint16_t mapX = (i % 4) + currentShape.x - 8;
			currentShapeMap[mapY] |= 1 << mapX;
		}
	}
}

void updateFloorObstacles() {
	memset(floorObstacle, 0, sizeof floorObstacle);
	floorObstacle[40] = 0b1111111111111111;
	for (uint8_t i = 0; i < 40; i++) {
		for (uint8_t j = 0; j < BITS; j++) {
			floorObstacle[i] |= (shapeObstacles[i][j] != 0) << j;
		}
	}
}

void addShapeToObstacles() {
	for (uint16_t i = 0; i < PLAYABLE_HEIGHT; i++) {
		for (uint16_t j = 0; j < BITS; j++) {
			if (CHECK_BIT(currentShapeMap[i], j)) {
				shapeObstacles[i][j] = currentShape.id;
			}
		}
	}
	updateFloorObstacles();
}

void moveDown() {
	currentShape.y++;
	updateShapeMap();
	if (floorCollisionDetected()) {
		currentShape.y--;
		updateShapeMap();
		addShapeToObstacles();

		currentShape.id = random(1, SHAPE_COUNT + 1);
		currentShape.x = 16;
		currentShape.y = 4;
	}
}

void moveRight() {
	currentShape.x++;
	updateShapeMap();
	if (floorCollisionDetected() || wallCollisionDetected()) {
		currentShape.x--;
		updateShapeMap();
	}
}

void moveLeft() {
	currentShape.x--;
	updateShapeMap();
	if (floorCollisionDetected() || wallCollisionDetected()) {
		currentShape.x++;
		updateShapeMap();
	}
}

void rotate() {
	uint8_t oldRotation = currentShape.rot;
	currentShape.rot = (currentShape.rot + 1) % ROTATION_COUNT;
	updateShapeMap();

	if (floorCollisionDetected() || wallCollisionDetected()) {
		currentShape.rot = oldRotation;
		updateShapeMap();
	}
}

void drop() {
	while (!floorCollisionDetected())
		moveDown();
}


void joystickMovement() {
	uint16_t joystickX = analogRead(JX);
	uint16_t joystickY = analogRead(JY);
	uint16_t  joystickS = digitalRead(JS);

	if (joystickX > 1000)
		moveRight();
	else if (joystickX < 100)
		moveLeft();

	delay(100);

	if (joystickY > 1000 && currentShape.y < PLAYABLE_HEIGHT)
		moveDown();
	else if (joystickY < 100)
		rotate();

	if (joystickS == 0)
		drop();
}

void performLineElimination() {
	bool clearedLines[40];
	bool clearLines;
	uint8_t lowestLine;

	for (uint8_t i = 40; i > 0; i--) {
		if (floorObstacle[i] == 32760) {	// Full Line
			for (uint8_t j = i; j > 1; j--) {
				memcpy(shapeObstacles[j], shapeObstacles[j - 1], sizeof shapeObstacles[j]);
			}
			updateFloorObstacles();
			SCORE += 5;
			matrix.drawFastHLine(0, 0, 1, WHITE);
			matrix.drawFastHLine(63, 63, 1, WHITE);
			matrix.drawFastVLine(0, 0, 63, WHITE);
			matrix.drawFastVLine(31, 0, 63, WHITE);
		}
	}
}

void reboot() {
	asm volatile ("  jmp 0");
}

void setup() {
	matrix.begin();
	matrix.setRotation(3);

	matrix.setTextSize(0.5);
	matrix.setTextColor(WHITE);
	matrix.setTextWrap(false);

	pinMode(JS, INPUT);
	digitalWrite(JS, HIGH);

	Serial.begin(9600);
	randomSeed(analogRead(A6));

	currentShape.id = random(2, 8);
	currentShape.rot = random(0, 4);
	currentShape.x = 16;
	currentShape.y = 4;

	initializeObstacles();
	splashScreen();
	delay(4000);
}

void loop() {

	matrix.fillScreen(0);
	redrawScreen();

	moveDownCounter++;
	if (moveDownCounter % MOVE_DOWN_FQ == 0)
		moveDown();

	updateShapeMap();

	for (uint16_t i = 0; i < BITS; i++) {
		if (CHECK_BIT(SHAPES[currentShape.id][currentShape.rot], i))
			matrix.drawPixel(BIT_X, BIT_Y, SHAPE_COLOR);
	}

	for (uint16_t i = 0; i < PLAYABLE_HEIGHT; i++) {
		for (uint16_t j = 0; j < BITS; j++) {
			if (shapeObstacles[i][j] != 0) {
				matrix.drawPixel(j + 8, i + 5, COLORS[shapeObstacles[i][j]]);
			}
		}
	}
	joystickMovement();
	performLineElimination();
	if (hitCeiling()) {
		gameOverScreen();
		delay(3000);
		reboot();

	}

	matrix.swapBuffers(false);
}
