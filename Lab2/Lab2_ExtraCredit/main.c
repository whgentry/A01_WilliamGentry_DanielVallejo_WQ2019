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
// Application Name     - SPI Demo
// Application Overview - The demo application focuses on showing the required 
//                        initialization sequence to enable the CC3200 SPI 
//                        module in full duplex 4-wire master and slave mode(s).
// Application Details  -
// http://processors.wiki.ti.com/index.php/CC32xx_SPI_Demo
// or
// docs\examples\CC32xx_SPI_Demo.pdf
//
//*****************************************************************************


//*****************************************************************************
//
//! \addtogroup SPI_Demo
//! @{
//
//*****************************************************************************

// Standard includes
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// Driverlib includes
#include "hw_types.h"
#include "hw_memmap.h"
#include "hw_common_reg.h"
#include "hw_ints.h"
#include "spi.h"
#include "rom.h"
#include "rom_map.h"
#include "utils.h"
#include "prcm.h"
#include "uart.h"
#include "interrupt.h"
#include "gpio.h"

// Display Library
#include "Adafruit_SSD1351.h"
#include "Adafruit_GFX.h"
#include "glcdfont.h"
#include "test.h"

// Common interface includes
#include "i2c_if.h"
#include "pinmux.h"


#define APPLICATION_VERSION     "1.1.1"
//*****************************************************************************
//
// Application Master/Slave mode selector macro
//
// MASTER_MODE = 1 : Application in master mode
// MASTER_MODE = 0 : Application in slave mode
//
//*****************************************************************************

#define SPI_IF_BIT_RATE  100000
#define TR_BUFF_SIZE     100

#define MASTER_MSG       "This is CC3200 SPI Master Application\n\r"
#define SLAVE_MSG        "This is CC3200 SPI Slave Application\n\r"

#define ACC_ADDR    0x18
#define X_ADDR      0x03
#define Y_ADDR      0x05
#define Z_ADDR      0x07
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
//! ACC_ReadReg
//!
//! \param  reg address value
//!
//! \return  reg data value
//
//*****************************************************************************
int8_t ACC_ReadReg(int8_t regAddr)
{
    int8_t pucWrDataBuf[1];
    int8_t pucRdDataBuf[1];

    pucWrDataBuf[0] = regAddr;
    I2C_IF_ReadFrom(ACC_ADDR, pucWrDataBuf, 1, pucRdDataBuf, 1);
    return  pucRdDataBuf[0];

}

//*****************************************************************************
//
//! ACC_WriteReg
//!
//! \param  reg address value
//!
//! \return  reg data value
//
//*****************************************************************************
void ACC_WriteReg(int8_t regAddr, int8_t regData)
{
    int8_t writeBuff[2];
    writeBuff[0] = regAddr;
    writeBuff[1] = regData;
    I2C_IF_Write(ACC_ADDR,writeBuff,2,0);
    return;
}
//*****************************************************************************
//
//! Main function
//!
//! \param none
//!
//! \return None.
//
//*****************************************************************************
void main()
 {
    int8_t xtilt;
    int8_t ytilt;
    int16_t xpos;
    int16_t ypos;

    int8_t redx = (rand() % (120 - 8 + 1)) + 8;
    int8_t redy = (rand() % (120 - 8 + 1)) + 8;

    int8_t state = 0;
    int8_t count = 0;

    // Initialize Board configurations
    BoardInit();
    // Muxing UART and SPI lines.
    PinMuxConfig();
    // Enable the SPI module clock
    MAP_PRCMPeripheralClkEnable(PRCM_GSPI,PRCM_RUN_MODE_CLK);
    // I2C Init
    I2C_IF_Open(I2C_MASTER_MODE_FST);

    // Reset the peripheral
    MAP_PRCMPeripheralReset(PRCM_GSPI);

    // Initialize display
    Adafruit_Init();
    fillScreen(BLACK);
    setTextSize(1);
    setTextColor(CYAN,BLACK);

    // Start ball in center of screen
    xpos = 63;
    ypos = 63;

    // set to 16g
    ACC_WriteReg(0x0F,0x0A);
    delay(10);
    // Set offset target to 1 for y
    ACC_WriteReg(0x37,0x00);
    delay(10);
    // Reset offset values
    ACC_WriteReg(0x36,0x80);
    delay(10);

    // Trigger X offset calc
    ACC_WriteReg(0x36,0x20);
    delay(10);
    // Wait for finish
    while((0x10 & ACC_ReadReg(0x36)) == 0);
    // Trigger Y offset calc
    ACC_WriteReg(0x36,0x40);
    delay(10);
    // Wait for finish
    while((0x10 & ACC_ReadReg(0x36)) == 0);
    // Trigger Z offset calc
    ACC_WriteReg(0x36,0x60);
    delay(10);
    // Wait for finish
    while((0x10 & ACC_ReadReg(0x36)) == 0);

    while(1)
    {

        switch(state)
        {
        case(0):
            setTextColor(CYAN,BLACK);
            setCursor(8,8);
            Outstr("Let's Play a Game!");
            setCursor(8,20);
            Outstr("Press SW2 to Start");

            setTextColor(MAGENTA,BLACK);
            setCursor(0,40);
            Outstr("1: Tilt to move green");
            setCursor(0,48);
            Outstr("2: Touch reds");
            setCursor(0,56);
            Outstr("3: Don't touch edge!");


            if (MAP_GPIOPinRead(GPIOA2_BASE,0x40) != 0)
            {
                while(MAP_GPIOPinRead(GPIOA2_BASE,0x40));
                state = 1;
                fillScreen(BLACK);
            }
            break;
        case(1):
            // Read I2C Data
            xtilt = ACC_ReadReg(X_ADDR);
            ytilt = ACC_ReadReg(Y_ADDR);
            MAP_UtilsDelay(500000);
            fillCircle(xpos,ypos,4,BLACK);
            // X saturating
            if ( (xpos + ytilt) > 123 )
                state = 2;
            else if ( (xpos + ytilt) < 4 )
                state = 2;
            else
                xpos += ytilt;
            // Y saturating
            if ( (ypos + xtilt) > 123 )
                state = 2;
            else if ( (ypos + xtilt) < 4 )
                state = 2;
            else
                ypos += xtilt;
            fillCircle(xpos,ypos,4,GREEN);

            fillCircle(redx,redy,4,RED);
            if (xpos == redx && ypos == redy)
            {
                fillCircle(redx,redy,4,BLACK);
                redx = (rand() % (120 - 8 + 1)) + 8;
                redy = (rand() % (120 - 8 + 1)) + 8;
                count++;
                fillCircle(redx,redy,4,RED);
            }
            break;
        case(2):
            setTextColor(RED,BLACK);
            setCursor(40,30);
            Outstr("YOU LOSE!");

            setTextColor(GREEN,BLACK);
            setCursor(40,50);
            char scorestr[16];
            sprintf(scorestr,"Score: %d",count);
            Outstr(scorestr);

            setTextColor(CYAN,BLACK);
            setCursor(10,100);
            Outstr("SW2 to Play Again");

            if (MAP_GPIOPinRead(GPIOA2_BASE,0x40) != 0)
            {
                while(MAP_GPIOPinRead(GPIOA2_BASE,0x40));
                state = 0;
                count = 0;
                fillScreen(BLACK);
            }

        }
    }
}

