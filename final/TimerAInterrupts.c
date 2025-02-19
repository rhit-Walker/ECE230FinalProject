/*
 * TimerAInterrupts.c
 *
 *  Created on: Feb 16, 2025
 *      Author: walkerqr
 */

#include "msp.h"
#define oneSecondDelay 23438

void TimerA0Init(void)
{
    TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK | // SMCLK as source (~1 MHz)
            TIMER_A_CTL_MC__UP |      // Up mode
            TIMER_A_CTL_ID__8 |       // divide by 8
            TIMER_A_CTL_CLR;          // Clear timer

    TIMER_A0->EX0 = TIMER_A_EX0_TAIDEX_7;

    TIMER_A0->CCR[0] = oneSecondDelay;             //2 sec delay

    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE;    // Enable interrupt on CCR0

    NVIC->ISER[0] = 1 << (TA0_0_IRQn & 31);  // Enable TimerA0 interrupt in NVIC
}

//void TimerA1Init(void)
//{
//    TIMER_A1->CTL = TIMER_A_CTL_SSEL__SMCLK | // SMCLK as source (~3 MHz default)
//            TIMER_A_CTL_MC__STOP |      // Stop mode
//            TIMER_A_CTL_ID__8 |       // divide by 8
//            TIMER_A_CTL_CLR;          // Clear timer
//
//    TIMER_A1->EX0 = TIMER_A_EX0_TAIDEX_7;
//
//    TIMER_A1->CCR[0] = oneSecondDelay;             //2 sec delay
//
//    TIMER_A1->CCTL[0] = TIMER_A_CCTLN_CCIE;    // Enable interrupt on CCR0
//
//    NVIC->ISER[0] = 1 << (TA1_0_IRQn & 31);  // Enable TimerA0 interrupt in NVIC
//}

