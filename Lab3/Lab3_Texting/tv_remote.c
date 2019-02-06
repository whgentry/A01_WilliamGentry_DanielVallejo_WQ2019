/*
 * tv_remote.c
 *
 *  Created on: Feb 6, 2019
 *      Author: William
 */
#include "tv_remote.h"


//*****************************************************************************
//
//! \button_decode
//!
//! Converts button code to number
//!
//! \param button_code.
//!
//! \return button_num
//*****************************************************************************
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

