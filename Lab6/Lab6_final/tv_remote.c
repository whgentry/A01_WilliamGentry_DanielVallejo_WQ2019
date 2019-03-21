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
static uint8_t button_char_pos = 0;

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
    static uint32_t code_prev = 0;
    static char char_caps = 0;

    // increment char pos if same button is pressed
    if (code == code_prev) {
        button_char_pos_inc(code);
    } else {
        button_char_pos = 0;
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
        return (button_char_2[button_char_pos] - char_caps);
    case TV_BUTTON_3:
        return (button_char_3[button_char_pos] - char_caps);
    case TV_BUTTON_4:
        return (button_char_4[button_char_pos] - char_caps);
    case TV_BUTTON_5:
        return (button_char_5[button_char_pos] - char_caps);
    case TV_BUTTON_6:
        return (button_char_6[button_char_pos] - char_caps);
    case TV_BUTTON_7:
        return (button_char_7[button_char_pos] - char_caps);
    case TV_BUTTON_8:
        return (button_char_8[button_char_pos] - char_caps);
    case TV_BUTTON_9:
        return (button_char_9[button_char_pos] - char_caps);
    case TV_BUTTON_LAST:
        return 0x7F;
    case TV_BUTTON_MUTE:
        return '\n';
    default:
        return 0xFF;
    }
}

void button_char_pos_inc(uint32_t code)
{
    switch(code)
    {
    case TV_BUTTON_2:
    case TV_BUTTON_3:
    case TV_BUTTON_4:
    case TV_BUTTON_5:
    case TV_BUTTON_6:
    case TV_BUTTON_8:
        button_char_pos = ++button_char_pos % 3;
        break;
    case TV_BUTTON_7:
    case TV_BUTTON_9:
        button_char_pos = ++button_char_pos % 4;
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

int button_wait_for_input(char *out_str, int x, int y, int text_color, int back_color)
{
    uint32_t code_current = 0;
    uint32_t code_prev = 0;
    char char_current;
    uint32_t char_delay;

    out_str[0] = '\0';
    setTextColor(text_color, back_color);
    Timer_IF_Init(PRCM_TIMERA1, TIMERA1_BASE, TIMER_CFG_ONE_SHOT_UP, TIMER_A, 0);
    Timer_IF_Start(TIMERA1_BASE, TIMER_A, 0xFFFFFFFF);

    while(1)
    {
        // This Function call clears the button ready value //
        code_current = button_code_get();
        char_delay = TICKS_TO_MILLISECONDS(TimerValueGet(TIMERA1_BASE, TIMER_A));

        if (code_current) {
            TimerValueSet(TIMERA1_BASE, TIMER_A, 0);
            char_current = button_char_get(code_current);
            int len = strlen(out_str);

            switch(char_current)
            {
            case 0xFF:  // Caps or bad code
                break;
            case 0x7F:  // Delete
                if (len) {
                    out_str[len-1] = '\0';
                }
                break;
            default:
                if ( (len == 0) ||
                     (code_current != code_prev))
                {
                    out_str[len] = char_current;
                    out_str[len+1] = '\0';
                } else {
                    out_str[len-1] = char_current;
                }
                break;
            }

            if (char_current == '\n') {
                // Reset text screen
                fillRect(x,y,(len+1) * 6, 8, back_color);
                out_str[len] = '\0';
                return 0;
            } else {
                setCursor(x,y);
                fillRect(x,y,(len+1) * 6, 8, back_color);
                Outstr(out_str);
            }

            code_prev = code_current;
        } else if (char_delay > CHAR_TIMEOUT) {
            TimerValueSet(TIMERA1_BASE, TIMER_A, 0);
            code_prev = 0;
        }
    }
}

