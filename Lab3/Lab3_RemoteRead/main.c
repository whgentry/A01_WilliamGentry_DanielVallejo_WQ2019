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
volatile uint32_t pulse_time = 0;
volatile uint32_t button_code = 0;
volatile int8_t button_num = 0;
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
    static uint32_t reading_signal = 0;
    static uint32_t bit_count = 0;
    static uint32_t bit_mask = TV_BIT_MASK_START;
    static uint32_t current_reading = 0;
    unsigned long ulStatus;

    pulse_time = TICKS_TO_MILLISECONDS(TimerValueGet(TIMERA0_BASE, TIMER_A));
    TimerValueSet(TIMERA0_BASE, TIMER_A, 0);

    switch(pulse_time)
    {
    case(TV_STATE_START):
        reading_signal = 1;
        bit_count = 0;
        bit_mask = TV_BIT_MASK_START;
        current_reading = 0;
        break;
    case(TV_STATE_ONE):
        current_reading |= ((bit_mask >> bit_count));
        bit_count++;
        break;
    case(TV_STATE_ZERO):
        bit_count++;
        break;
    default:
        reading_signal = 1;
        bit_count = 0;
        bit_mask = TV_BIT_MASK_START;
        current_reading = 0;
        break;
    }

    if (bit_count == 24) {
//        Message("Button Datagram Read\n\r");
        button_code = current_reading;

        reading_signal = 1;
        bit_count = 0;
        bit_mask = TV_BIT_MASK_START;
        current_reading = 0;
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

    // Setup Configure Timer
    Timer_IF_Init(PRCM_TIMERA0, TIMERA0_BASE, TIMER_CFG_PERIODIC_UP, TIMER_A, 0);
    Timer_IF_Start(TIMERA0_BASE, TIMER_A, 0xFFFFFFFF);
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
    setTextSize(18);
    setTextColor(CYAN,BLACK);


    char str[32] = "";
    uint32_t button_prev;

    while(1)
    {
        UtilsDelay(1000000);
        if (button_code != button_prev) {
            sprintf(str, "%x", button_decode(button_code));
            Message(str);
            fillScreen(BLACK);
            setCursor(0,0);
            Outstr(str);
        }

        button_prev = button_code;
    }
}

