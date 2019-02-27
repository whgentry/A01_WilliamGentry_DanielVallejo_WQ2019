/*
 * dtmf.h
 *
 *  Created on: Feb 19, 2019
 *      Author: William
 */

#ifndef DTMF_H_
#define DTMF_H_

#include "project.h"

#include <stdint.h>
#include <math.h>

// ADC Macros
#define ADC_SAMPLING_FREQ   16000
#define ADC_TIMER_VALUE     (SYS_CLK / ADC_SAMPLING_FREQ)
#define ADC_BUFFER_SIZE     410

#define ADC_MSB_MASK        0x1F
#define ADC_MSB_SHIFT       5
#define ADC_LSB_MASK        0xF8
#define ADC_LSB_SHIFT       3

#define ADC_OFFSET          512
#define ADC_SCALE_DOWN      4

#define ADC_POWER_MIN       100
#define ADC_POWER_MAX       1000

// Buttons
#define DTMF_BUTTON_0      0
#define DTMF_BUTTON_1      1
#define DTMF_BUTTON_2      2
#define DTMF_BUTTON_3      3
#define DTMF_BUTTON_4      4
#define DTMF_BUTTON_5      5
#define DTMF_BUTTON_6      6
#define DTMF_BUTTON_7      7
#define DTMF_BUTTON_8      8
#define DTMF_BUTTON_9      9
#define DTMF_BUTTON_STAR   10
#define DTMF_BUTTON_POUND  11

void adc_add_sample(unsigned long sample);
void dtmf_analyze_buffer(void);
void dtmf_start_sampling(void);
void dtmf_stop_sampling(void);
void dtmf_coeff_Init(void);
int8_t dtmf_tone_ready(void);
int8_t dtmf_tone_get(void);
long int dtmf_goertzel(long int coeff);
char button_char_get(int8_t code);
void button_char_pos_inc(uint32_t code);

#endif /* DTMF_H_ */
