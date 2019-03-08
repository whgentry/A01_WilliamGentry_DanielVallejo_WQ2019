/*
 * project.h
 * Group all the header files to clean up main.c
 *  Created on: Jan 31, 2019
 *      Author: William
 */

#ifndef PROJECT_H_
#define PROJECT_H_

// Standard includes
#include <string.h>
#include <stdio.h>
#include <stdint.h>

// Driverlib includes
#include "hw_types.h"
#include "hw_memmap.h"
#include "hw_common_reg.h"
#include "hw_ints.h"
#include "hw_timer.h"
#include "spi.h"
#include "rom.h"
#include "rom_map.h"
#include "utils.h"
#include "prcm.h"
#include "uart.h"
#include "interrupt.h"
#include "gpio.h"
#include "timer.h"

// Display Library
#include "Adafruit_SSD1351.h"
#include "Adafruit_GFX.h"
#include "glcdfont.h"
#include "test.h"

// TV Remote Button Data
#include "tv_remote.h"

// Common interface includes
#include "uart_if.h"
#include "timer_if_custom.h"
#include "pinmux.h"

// Interrupt Port and pin
#define INT_IR_BASE     INT_GPIOA0
#define IR_BASE         GPIOA0_BASE
#define IR_PIN          0x80

// UART Macros
#define UART_T_BASE     UARTA1_BASE
#define INT_UART_T      INT_UARTA1
#define UART_T_PERIPH   PRCM_UARTA1
#define UART_T_TX       0
#define UART_T_RX       0

//#define UART_BAUD_RATE  115200
//#define SYSCLK          80000000


// Timer Macros
#define MICROSECONDS_TO_TICKS(ms)   ((SYS_CLK/1000000) * (ms))
#define TICKS_TO_MICROSECONDS(ms)   ((ms) / (SYS_CLK/1000000))
#define MILLISECONDS_TO_TICKS(ms)   ((SYS_CLK/1000) * (ms))
#define TICKS_TO_MILLISECONDS(ms)   ((ms) / (SYS_CLK/1000))

#define CHAR_TIMEOUT 1000

#endif /* PROJECT_H_ */
