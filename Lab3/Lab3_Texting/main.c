//*****************************************************************************
//
// Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/ 
// 
// 
//  Redistribution and use in source and binary forms, with or without 
//  modification, are permitted provided that the following conditions 
//  are met:
//
//    Redistributions of source code must retain the above copyright 
//    notice, this list of conditions and the following disclaimer.
//
//    Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the 
//    documentation and/or other materials provided with the   
//    distribution.
//
//    Neither the name of Texas Instruments Incorporated nor the names of
//    its contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
//  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
//  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
//  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
//  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
//  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
//  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//*****************************************************************************

//*****************************************************************************
//
// Application Name     - Lab3
// Application Details  -
//
//*****************************************************************************

#include "project.h"

#define APPLICATION_VERSION     "3.1"

//*****************************************************************************
//                 GLOBAL VARIABLES -- Start
//*****************************************************************************
#if defined(ccs)
extern void (* const g_pfnVectors[])(void);
#endif
#if defined(ewarm)
extern uVectorEntry __vector_table;
#endif

// My variables
uint32_t code_current = 0;
uint32_t code_prev = 0;
char out_str[128] = "";
//*****************************************************************************
//                 GLOBAL VARIABLES -- End
//*****************************************************************************

//*****************************************************************************
//
//! \GPIOA1IntHandler
//!
//! Handles interrupts for GPIO A1,
//!
//! \param None.
//!
//! \return None.
//
//*****************************************************************************
static void
GPIOA1IntHandler(void)
{
    static uint32_t bit_count = 0;
    static uint32_t current_reading = 0;
    static uint8_t  data_repeat = 0;

    unsigned long ulStatus;
    uint32_t pulse_time;

    pulse_time = TICKS_TO_MILLISECONDS(TimerValueGet(TIMERA0_BASE, TIMER_A));
    TimerValueSet(TIMERA0_BASE, TIMER_A, 0);

    switch(pulse_time)
    {
    case(TV_STATE_START):
        bit_count = 0;
        current_reading = 0;
        break;
    case(TV_STATE_ONE):
        current_reading |= ((TV_BIT_MASK_START >> bit_count));
        bit_count++;
        break;
    case(TV_STATE_ZERO):
        bit_count++;
        break;
    case(TV_STATE_REPEAT):
        data_repeat = 1;
        break;
    default:
        bit_count = 0;
        current_reading = 0;
        data_repeat = 0;
        break;
    }

    if (bit_count == 24 && !data_repeat) {
        button_code_set(current_reading);
    }

    // Clear all interrupts for Timer unit 0.
    ulStatus = MAP_GPIOIntStatus(IR_BASE, true);
    MAP_GPIOIntClear(IR_BASE, ulStatus);

    // Alert to the user
    //Message("Completed GPIOA1 Interrupt Handler \n\r");
}
//*****************************************************************************
//
//! Initialize Interupts for IRReceiver
//!
//! \param None.
//!
//! \return None.
//
//*****************************************************************************
static void
IntIRReceiver_Init(void)
{
    // GPIO Interrupt Intialization
    MAP_IntPrioritySet(INT_IR_BASE, INT_PRIORITY_LVL_0);
    MAP_GPIOIntTypeSet(IR_BASE, IR_PIN, GPIO_RISING_EDGE);
    MAP_GPIOIntRegister(IR_BASE, GPIOA1IntHandler);
    MAP_GPIOIntClear(IR_BASE, IR_PIN);
    MAP_GPIOIntEnable(IR_BASE, IR_PIN);

    // Setup Configure Timer For signal Read
    Timer_IF_Init(PRCM_TIMERA0, TIMERA0_BASE, TIMER_CFG_PERIODIC_UP, TIMER_A, 0);
    Timer_IF_Start(TIMERA0_BASE, TIMER_A, 0xFFFFFFFF);

    // Setup Timer for character select timeout
    Timer_IF_Init(PRCM_TIMERA1, TIMERA1_BASE, TIMER_CFG_ONE_SHOT_UP, TIMER_A, 0);
    Timer_IF_Start(TIMERA1_BASE, TIMER_A, 0xFFFFFFFF);
}

//*****************************************************************************
//
//! Board Initialization & Configuration
//!
//! \param  None
//!
//! \return None
//
//*****************************************************************************
static void
BoardInit(void)
{
/* In case of TI-RTOS vector table is initialize by OS itself */
#ifndef USE_TIRTOS
  //
  // Set vector table base
  //
#if defined(ccs)
    MAP_IntVTableBaseSet((unsigned long)&g_pfnVectors[0]);
#endif
#if defined(ewarm)
    MAP_IntVTableBaseSet((unsigned long)&__vector_table);
#endif
#endif
    //
    // Enable Processor
    //
    MAP_IntMasterEnable();
    MAP_IntEnable(FAULT_SYSTICK);

    PRCMCC3200MCUInit();
}

//*****************************************************************************
//
//! Main function for spi demo application
//!
//! \param none
//!
//! \return None.
//
//*****************************************************************************
void main()
{
    // Initialize Board configurations
    BoardInit();

    // Muxing UART and SPI lines.
    PinMuxConfig();

    // Enable the SPI module clock
    MAP_PRCMPeripheralClkEnable(PRCM_GSPI,PRCM_RUN_MODE_CLK);

    // Initialising the Terminal.
    InitTerm();

    // Clearing the Terminal.
    ClearTerm();

    // Display the Banner
//    Message("\n\n\n\r");
//    Message("\t\t   ********************************************\n\r");
//    Message("\t\t        CC3200 Lab 3 Application  \n\r");
//    Message("\t\t   ********************************************\n\r");
//    Message("\n\n\n\r");

    // Reset the peripheral
    MAP_PRCMPeripheralReset(PRCM_GSPI);

    // Set up interrupt for IR Reciever
    IntIRReceiver_Init();

    // Initialize Display
    Adafruit_Init();
    fillScreen(BLACK);

    // Text Setup
    setTextWrap(1);
    setTextSize(1);
    setTextColor(RED,WHITE);

    char char_current;
    uint32_t char_delay;

    while(1)
    {
        // This Function call clears the button ready value //
        code_current = button_code_get();
        char_delay = TICKS_TO_MILLISECONDS(TimerValueGet(TIMERA1_BASE, TIMER_A));

        if (code_current || char_delay > CHAR_TIMEOUT) {
            TimerValueSet(TIMERA1_BASE, TIMER_A, 0);
            char_current = button_char_get(code_current);
            int len = strlen(out_str);
//            drawChar(0,120,char_current,YELLOW,BLACK,1);

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
                     (code_current != code_prev) ||
                     (char_delay > CHAR_TIMEOUT) )
                {
                    out_str[len] = char_current;
                    out_str[len+1] = '\0';
                } else {
                    out_str[len-1] = char_current;
                }
                break;
            }

            if (char_current == '\0') {
                // send the out_str over uart
                fillRect(0,0,128,64,WHITE);
                out_str[0] = '\0';
            } else if ( !(char_delay > CHAR_TIMEOUT)) {
                Message(out_str);
                setCursor(0,0);
                fillRect(0,0,128,64,WHITE);
                Outstr(out_str);
            }

            code_prev = code_current;
        }
    }
}

