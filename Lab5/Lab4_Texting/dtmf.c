/*
 * dtmf.c
 *
 *  Created on: Feb 19, 2019
 *      Author: William
 */

#include "dtmf.h"

// Global Variables
static int adc_buffer[ADC_BUFFER_SIZE];
static uint16_t adc_index = 0;
static int8_t dtmf_ready = 0;
static int8_t dtmf_tone = 0;
static int8_t new_dig = 0;

int power_all[8];               // array to store calculated power of 8 frequencies
int coeff[8];                   // array to store the calculated coefficients
int f_tone[8]={697, 770, 852, 941, 1209, 1336, 1477, 1633}; // frequencies of rows & columns

static uint32_t button_code = 0;
static uint32_t button_code_prev = 0;
static int8_t button_num = 0;
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

// DTMF Table
int8_t row_col[4][4] =
    {
    {1, 2, 3, -1},
    {4, 5, 6, -1},
    {7, 8, 9, -1},
    {10, 0, 11, -1}
    };

void adc_add_sample(unsigned long sample)
{
    int temp = ((int)sample - ADC_OFFSET) >> ADC_SCALE_DOWN;
    adc_buffer[adc_index++] = ((int)sample - ADC_OFFSET) >> ADC_SCALE_DOWN;

    if (adc_index == ADC_BUFFER_SIZE) {
        dtmf_stop_sampling();
        dtmf_analyze_buffer();
        dtmf_start_sampling();
        adc_index = 0;
    }

    return;
}

void dtmf_coeff_Init(void)
{
    int i = 0;
    for (i = 0; i < 8; i++){
        coeff[i] = (2*cos(2*M_PI*(f_tone[i]/(float)ADC_SAMPLING_FREQ)))*(1<<14);
    }

    int c0 = coeff[0];
    int c1 = coeff[1];
    int c2 = coeff[2];
    int c3 = coeff[3];
    int c4 = coeff[4];
    int c5 = coeff[5];
    int c6 = coeff[6];
    int c7 = coeff[7];

    while(0);

    return;
}

void dtmf_analyze_buffer(void)
{
    int i,row,col,max_power;

    for (i=0;i<8;i++) {
        power_all[i]=dtmf_goertzel(coeff[i]); // call goertzel to calculate the power at each frequency and store it in the power_all array
    }

    int t0 = power_all[0];
    int t1 = power_all[1];
    int t2 = power_all[2];
    int t3 = power_all[3];
    int t4 = power_all[4];
    int t5 = power_all[5];
    int t6 = power_all[6];
    int t7 = power_all[7];

    // find the maximum power in the row frequencies and the row number

    max_power=0;            //initialize max_power=0

    for(i=0;i<4;i++) {        //loop 4 times from 0>3 (the indecies of the rows)
        if (power_all[i] > max_power) {   //if power of the current row frequency > max_power
            max_power=power_all[i];     //set max_power as the current row frequency
            row=i;                      //update row number
        }
    }


    // find the maximum power in the column frequencies and the column number

    max_power=0;            //initialize max_power=0

    for(i=4;i<8;i++) {       //loop 4 times from 4>7 (the indecies of the columns)
        if (power_all[i] > max_power) {  //if power of the current column frequency > max_power
            max_power=power_all[i];     //set max_power as the current column frequency
            col=i;                      //update column number
        }
    }


//    if(power_all[col]==0 && power_all[row]==0) {//if the maximum powers equal zero > this means no signal or inter-digit pause
//        new_dig=1;                             //set new_dig to 1 to display the next decoded digit
//    }

    if(power_all[col] < ADC_POWER_MIN && power_all[row] < ADC_POWER_MIN) {//if the maximum powers equal zero > this means no signal or inter-digit pause
        new_dig=1;                             //set new_dig to 1 to display the next decoded digit
    }

    if((power_all[col]>ADC_POWER_MAX  && power_all[row]>ADC_POWER_MAX) && (new_dig==1)) { // check if maximum powers of row & column exceed certain threshold AND new_dig flag is set to 1
            dtmf_tone = row_col[row][col-4];
            dtmf_ready = 1;
            new_dig=0;                                              // set new_dig to 0 to avoid displaying the same digit again.
    }


    return;
}

int8_t dtmf_tone_ready(void)
{
    return dtmf_ready;
}

int8_t dtmf_tone_get(void)
{
    dtmf_ready = 0;
    return dtmf_tone;
}

void dtmf_start_sampling(void)
{
    MAP_TimerIntEnable(TIMERA0_BASE, TIMER_TIMA_TIMEOUT);
    return;
}

void dtmf_stop_sampling(void)
{
    MAP_TimerIntDisable(TIMERA0_BASE, TIMER_TIMA_TIMEOUT);
    return;
}

long int dtmf_goertzel(long int coeff)
{
    //initialize variables to be used in the function
    int Q, Q_prev, Q_prev2,i;
    long prod1,prod2,prod3,power;

    Q_prev = 0;         //set delay element1 Q_prev as zero
    Q_prev2 = 0;        //set delay element2 Q_prev2 as zero
    power=0;            //set power as zero

    for (i=0; i<ADC_BUFFER_SIZE; i++) { // loop N times and calculate Q, Q_prev, Q_prev2 at each iteration
        int temp = adc_buffer[i];
        Q = (adc_buffer[i]) + ((coeff* Q_prev)>>14) - (Q_prev2); // >>14 used as the coeff was used in Q15 format
        Q_prev2 = Q_prev;                                    // shuffle delay elements
        Q_prev = Q;
    }

    //calculate the three products used to calculate power
    prod1=( (long) Q_prev*Q_prev);
    prod2=( (long) Q_prev2*Q_prev2);
    prod3=( (long) Q_prev *coeff)>>14;
    prod3=( prod3 * Q_prev2);

    power = ((prod1+prod2-prod3))>>8; //calculate power using the three products and scale the result down

    return power;
}

char button_char_get(int8_t code)
{
    static int8_t code_prev = 0;
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
    case DTMF_BUTTON_0:
        return ' ';
    case DTMF_BUTTON_1:
        if (char_caps) {
            char_caps = 0;
        } else {
            char_caps = 32;
        }
        return 0xFF;
    case DTMF_BUTTON_2:
        return (button_char_2[button_char_pos] - char_caps);
    case DTMF_BUTTON_3:
        return (button_char_3[button_char_pos] - char_caps);
    case DTMF_BUTTON_4:
        return (button_char_4[button_char_pos] - char_caps);
    case DTMF_BUTTON_5:
        return (button_char_5[button_char_pos] - char_caps);
    case DTMF_BUTTON_6:
        return (button_char_6[button_char_pos] - char_caps);
    case DTMF_BUTTON_7:
        return (button_char_7[button_char_pos] - char_caps);
    case DTMF_BUTTON_8:
        return (button_char_8[button_char_pos] - char_caps);
    case DTMF_BUTTON_9:
        return (button_char_9[button_char_pos] - char_caps);
    case DTMF_BUTTON_STAR:
        return 0x7F;
    case DTMF_BUTTON_POUND:
        return '\n';
    default:
        return 0xFF;
    }
}

void button_char_pos_inc(uint32_t code)
{
    switch(code)
    {
    case DTMF_BUTTON_2:
    case DTMF_BUTTON_3:
    case DTMF_BUTTON_4:
    case DTMF_BUTTON_5:
    case DTMF_BUTTON_6:
    case DTMF_BUTTON_8:
        button_char_pos = ++button_char_pos % 3;
        break;
    case DTMF_BUTTON_7:
    case DTMF_BUTTON_9:
        button_char_pos = ++button_char_pos % 4;
        break;
    case DTMF_BUTTON_STAR:
    case DTMF_BUTTON_POUND:
    case DTMF_BUTTON_0:
    case DTMF_BUTTON_1:
    default:
        return;
    }
}



