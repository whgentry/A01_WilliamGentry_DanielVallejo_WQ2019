/*
 * tv_remote.c
 *
 *  Created on: Feb 6, 2019
 *      Author: William
 */
#include "tv_remote.h"

// Global Variables
static uint32_t button_code = 0;
static uint32_t button_code_prev = 0;
static int8_t button_num = 0;
static int8_t button_ready = 0;

// Multi Tap Character arrays
char button_char_2[4] = "abc";
char button_char_3[4] = "def";
char button_char_4[4] = "ghi";
char button_char_5[4] = "jkl";
char button_char_6[4] = "mno";
char button_char_7[5] = "pqrs";
char button_char_8[4] = "tuv";
char button_char_9[5] = "wxyz";

void button_ready_clear(void)
{
    button_ready = 0;
}

void button_ready_set(void)
{
    button_ready = 1;
}

void button_code_set(uint32_t code)
{
    button_code = code;
    button_ready_set();
}

uint32_t button_code_get()
{
    if (button_ready == 1) {
        button_ready_clear();
        return button_code;
    } else {
        return 0;
    }
}

char button_char_get(uint32_t code)
{
    static uint8_t char_pos = 0;
    static uint32_t code_prev = 0;
    static char char_caps = 0;

    // increment char pos if same button is pressed
    if (code == code_prev) {
        button_char_pos_inc(code, &char_pos);
    } else {
        char_pos = 0;
    }
    code_prev = code;

    switch(code)
    {
    case TV_BUTTON_0:
        return ' ';
    case TV_BUTTON_1:
        if (char_caps) {
            char_caps = 0;
        } else {
            char_caps = 32;
        }
        return 0xFF;
    case TV_BUTTON_2:
        return (button_char_2[char_pos] - char_caps);
    case TV_BUTTON_3:
        return (button_char_3[char_pos] - char_caps);
    case TV_BUTTON_4:
        return (button_char_4[char_pos] - char_caps);
    case TV_BUTTON_5:
        return (button_char_5[char_pos] - char_caps);
    case TV_BUTTON_6:
        return (button_char_6[char_pos] - char_caps);
    case TV_BUTTON_7:
        return (button_char_7[char_pos] - char_caps);
    case TV_BUTTON_8:
        return (button_char_8[char_pos] - char_caps);
    case TV_BUTTON_9:
        return (button_char_9[char_pos] - char_caps);
    case TV_BUTTON_LAST:
        return 0x7F;
    case TV_BUTTON_MUTE:
        return '\0';
    default:
        return 0xFF;
    }
}

void button_char_pos_inc(uint32_t code, uint8_t *pos)
{
    switch(code)
    {
    case TV_BUTTON_2:
    case TV_BUTTON_3:
    case TV_BUTTON_4:
    case TV_BUTTON_5:
    case TV_BUTTON_6:
    case TV_BUTTON_8:
        *pos = ++(*pos) % 3;
        break;
    case TV_BUTTON_7:
    case TV_BUTTON_9:
        *pos = ++(*pos) % 4;
        break;
    case TV_BUTTON_LAST:
    case TV_BUTTON_MUTE:
    case TV_BUTTON_0:
    case TV_BUTTON_1:
    default:
        return;
    }
}

int8_t button_decode(uint32_t code)
{
    switch(code)
    {
    case TV_BUTTON_0:
        return 0;
    case TV_BUTTON_1:
        return 1;
    case TV_BUTTON_2:
        return 2;
    case TV_BUTTON_3:
        return 3;
    case TV_BUTTON_4:
        return 4;
    case TV_BUTTON_5:
        return 5;
    case TV_BUTTON_6:
        return 6;
    case TV_BUTTON_7:
        return 7;
    case TV_BUTTON_8:
        return 8;
    case TV_BUTTON_9:
        return 9;
    case TV_BUTTON_LAST:
        return 10;
    case TV_BUTTON_MUTE:
        return 11;
    default:
        return -1;
    }
}

