/*
 * tv_remote.h
 *
 *  Created on: Jan 31, 2019
 *      Author: William
 */

#ifndef TV_REMOTE_H_
#define TV_REMOTE_H_

#include <stdint.h>

// TV Code
#define TV_CODE         1017

#define TV_BUTTON_0     0b100101100101011010011010
#define TV_BUTTON_1     0b100101111110011010000001
#define TV_BUTTON_2     0b100101100001011010011110
#define TV_BUTTON_3     0b100101010001011010101110
#define TV_BUTTON_4     0b100101110001011010001110
#define TV_BUTTON_5     0b100101001001011010110110
#define TV_BUTTON_6     0b100101101001011010010110
#define TV_BUTTON_7     0b100101011001011010100110
#define TV_BUTTON_8     0b100101111001011010000110
#define TV_BUTTON_9     0b100101000101011010111010
#define TV_BUTTON_LAST  0b100101011100011010100011
#define TV_BUTTON_MUTE  0b100101110011011010001100

#define TV_BIT_MASK_START   0x00800000

// Pulse Width Lengths
#define TV_PULSE_START_MIN  4000
#define TV_PULSE_START_MAX  5000
#define TV_PULSE_1_MIN  3000
#define TV_PULSE_1_MAX  4000
#define TV_PULSE_0_MIN  1000
#define TV_PULSE_0_MAX  2000

// State machine
#define TV_STATE_START  4
#define TV_STATE_ONE    3
#define TV_STATE_ZERO   1
#define TV_STATE_REPEAT 36


///// Functions /////
int8_t button_decode(uint32_t code);
void button_ready_clear(void);
void button_ready_set(void);
void button_code_set(uint32_t code);
uint32_t button_code_get();
char button_char_get(uint32_t code);
void button_char_pos_inc(uint32_t code);

#endif /* TV_REMOTE_H_ */
