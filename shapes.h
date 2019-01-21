/*



*/

#pragma once

// Binary L to R --> Top to Bottom
const uint16_t S_BLOCK_1[ROTATION_COUNT] = { 0b0001001100100000,
											 0b0000011000110000,
											 0b0001001100100000,
											 0b0110001100000000 };
const uint16_t S_BLOCK_2[ROTATION_COUNT] = { 0b0010001100010000,
											 0b0000001101100000,
											 0b0100011000100000,
											 0b0011011000000000 };
const uint16_t L_BLOCK_1[ROTATION_COUNT] = { 0b0011001000100000,
											 0b0000011100010000,
											 0b0010001001100000,
											 0b0100011100000000 };
const uint16_t L_BLOCK_2[ROTATION_COUNT] = { 0b0010001000110000,
											 0b0000011101000000,
											 0b0110001000100000,
											 0b0001011100000000 };
const uint16_t I_BLOCK_1[ROTATION_COUNT] = { 0b0010001000100010,
											 0b0000111100000000,
											 0b0010001000100010,
											 0b0000111100000000 };
const uint16_t T_BLOCK_1[ROTATION_COUNT] = { 0b0010001100100000,
											 0b0000011100100000,
											 0b0010011000100000,
											 0b0010011100000000 };
const uint16_t C_BLOCK_1[ROTATION_COUNT] = { 0b0000001100110000,
											 0b0000001100110000,
											 0b0000001100110000,
											 0b0000001100110000};

const uint16_t *SHAPES[SHAPE_COUNT + 1] = { NULL,
										S_BLOCK_1,
										S_BLOCK_2,
										L_BLOCK_1,
										L_BLOCK_2,
										I_BLOCK_1,
										T_BLOCK_1,
										C_BLOCK_1 };

struct Shape {
	uint8_t rot : 2;
	uint8_t id : 3;
	uint8_t x : 5;
	uint8_t y : 6; 
};