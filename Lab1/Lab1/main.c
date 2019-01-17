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
// Application Name     - Lab1
// Application Overview - The objective of this application is to demonstrate 
//                        simple UART comunication and controlling GPIO pins.
//                        Press SW2 to blink LEDS in unison and set Pin 4 low.
//                        Press Sw3 to have a 3 bit counter shown on the LEDS 
//                        and set Pin 4 high. When the either switch is pressed 
//                        the corresponding switch will be transmitted over UART
//                        
// Group Members        - William Gentry and Daniel Vallejo
//
//*****************************************************************************

//****************************************************************************
//
//! \addtogroup blinky
//! @{
//
//****************************************************************************

// Standard includes
#include <stdio.h>

// Driverlib includes
#include "hw_types.h"
#include "hw_ints.h"
#include "hw_memmap.h"
#include "hw_common_reg.h"
#include "interrupt.h"
#include "hw_apps_rcm.h"
#include "prcm.h"
#include "rom.h"
#include "rom_map.h"
#include "prcm.h"
#include "gpio.h"
#include "utils.h"
#include "uart.h"

// Common interface includes
#include "gpio_if.h"
#include "pin.h"
#include "pin_mux_config.h"
#include "uart_if.h"

//*****************************************************************************
//                          MACROS
//*****************************************************************************
#define APPLICATION_VERSION  "1.1.1"
#define APP_NAME             "GPIO"
#define CONSOLE              UARTA0_BASE
#define UartGetChar()        MAP_UARTCharGet(CONSOLE)
#define UartPutChar(c)       MAP_UARTCharPut(CONSOLE,c)
#define MAX_STRING_LENGTH    80

//*****************************************************************************
//                 GLOBAL VARIABLES -- Start
//*****************************************************************************
#if defined(ccs)
extern void (* const g_pfnVectors[])(void);
#endif
#if defined(ewarm)
extern uVectorEntry __vector_table;
#endif
//*****************************************************************************
//                 GLOBAL VARIABLES -- End
//*****************************************************************************


//*****************************************************************************
//                      LOCAL FUNCTION PROTOTYPES                           
//*****************************************************************************
void LEDBlinkyRoutine();
static void BoardInit(void);

//*****************************************************************************
//                      LOCAL FUNCTION DEFINITIONS                         
//*****************************************************************************

//*****************************************************************************
//
//! Application startup display on UART
//!
//! \param  none
//!
//! \return none
//!
//*****************************************************************************
static void
DisplayBanner(char * AppName)
{

    Report("\n\n\n\r");
    Report("*************************************************\n\r");
    Report("\tCC3200 %s Application       \n\r", AppName);
    Report("*************************************************\n\r");
    Report("\n\n\n\r");
}


//*****************************************************************************
//
//! Configures the pins as GPIOs and peroidically toggles the lines
//!
//! \param None
//! 
//! This function  
//!    1. Configures 3 lines connected to LEDs as GPIO
//!    2. Sets up the GPIO pins as output
//!    3. Periodically toggles each LED one by one by toggling the GPIO line
//!
//! \return None
//
//*****************************************************************************
void LEDBlinkyRoutine()
{
    //
    // Lab 1 part II program
    //

    enum SwitchState { NA, SW2, SW3 };  // possible states of machine

    long sw2, sw3, count = 0;
    enum SwitchState state = NA;
    
    GPIO_IF_LedOff(MCU_ALL_LED_IND);
    
    while(1)
    {   
        // Read presses
        // GPIOPinRead takes base and adds as offset to pin
        // GP22 = 16 + 6
        // GP13 = 8 + 5
        
        // Store the state of pin into variables
        sw3 = GPIOPinRead(GPIOA1_BASE,GPIO_PIN_5);
        sw2 = GPIOPinRead(GPIOA2_BASE,GPIO_PIN_6);
        
        // State Machine
        switch(state) 
        {
            case(SW2): // In this state SW2 will have no affect until SW3 is pressed
                switch(count)
                {
                    case(7):
                        count = 0; // Set all LEDS off
                        break;
                    default:
                        count = 7; // Set all LEDS on
                }
                if(sw3)
                {
                    Message("SW3 pressed\n\r");
                    GPIOPinWrite(GPIOA3_BASE,GPIO_PIN_4,0x00);
                    state = SW3;
                }
                break;

            case(SW3): // In this state SW3 will have no affect until SW2 is pressed
                switch(count)
                {
                    case(7):
                        count = 0; // Reset to zero after 0b111
                        break;
                    default:
                        count++;   // Increment count
                        break;
                }
                if(sw2)
                {
                    Message("SW2 pressed\n\r");
                    GPIOPinWrite(GPIOA3_BASE,GPIO_PIN_4,GPIO_PIN_4);
                    state = SW2;
                }
                break;

            case(NA): // Default state when first powered up
                if (sw3)
                {
                    Message("SW3 pressed\n\r");
                    GPIOPinWrite(GPIOA3_BASE,GPIO_PIN_4,0x00);
                    state = SW3;
                }
                else if(sw2)
                {
                    Message("SW2 pressed\n\r");
                    GPIOPinWrite(GPIOA3_BASE,GPIO_PIN_4,GPIO_PIN_4);
                    state = SW2;
                }
                break;

        }

        // variable count will then be bit masked to control the 3 LEDS
        // control green
        if (count & 1)
            GPIO_IF_LedOn(MCU_GREEN_LED_GPIO);
        else
            GPIO_IF_LedOff(MCU_GREEN_LED_GPIO);
        // control orange
        if (count & 2)
            GPIO_IF_LedOn(MCU_ORANGE_LED_GPIO);
        else
            GPIO_IF_LedOff(MCU_ORANGE_LED_GPIO);
        // control red
        if (count & 4)
            GPIO_IF_LedOn(MCU_RED_LED_GPIO);
        else
            GPIO_IF_LedOff(MCU_RED_LED_GPIO);
        
        // Delay to see the changes in LEDs
        MAP_UtilsDelay(4000000);
    }

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
//****************************************************************************
//
//! Main function
//!
//! \param none
//! 
//! This function  
//!    1. Invokes the LEDBlinkyTask
//!
//! \return None.
//
//****************************************************************************
int
main()
{
    //
    // Initialize Board configurations
    //
    BoardInit();
    
    //
    // Power on the corresponding GPIO port B for 9,10,11.
    // Set up the GPIO lines to mode 0 (GPIO)
    //
    PinMuxConfig();

    GPIO_IF_LedConfigure(LED1|LED2|LED3);
    GPIO_IF_LedOff(MCU_ALL_LED_IND);
    
    InitTerm();
    //
    // Clearing the Terminal.
    //
    ClearTerm();

    DisplayBanner(APP_NAME);
    Message("****************************************************\n\r");
    Message("\tPush SW3 to start LED binary counting\n\r");
    Message("\tPush SW2 to blink LEDs on and off\n\r");
    Message("****************************************************\n\n\n\r");

    //
    // Start the LEDBlinkyRoutine
    //
    LEDBlinkyRoutine();
    return 0;
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
