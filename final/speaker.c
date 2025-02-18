/*
 * speaker.c
 *
 *  Created on: Feb 10, 2025
 *      Author: martincw
 */

#include <msp.h>

#include <stdio.h>
#include "csHFXT.h"
#include "csLFXT.h"
#include "speaker.h"

void PlayerConfiguration(void){
    SpeakerPort->DIR |= Speaker;            // set P2.4 as output
    SpeakerPort->SEL0 |= Speaker;           // P2.4 set to TA0.1
    SpeakerPort->SEL1 &= ~Speaker;          // 0b01 (secondary mode)
    TIMER_A0->CCR[0] = NOTEA4 - 1;          // TA0 CCR0 compare for period
    TIMER_A0->CCR[1] = (NOTEA4 / 2) - 1;    // TA0 CCR1 compare for duty cycle of the note
    TIMER_A0->CCTL[1] = 0b0000000001100000;
    TIMER_A0->CTL = 0b0000001010010100;
}

void PlayNote(unsigned int CurrentNote){   // Set high pulse-width in CCR1 register
    TIMER_A0->CCR[0] = CurrentNote - 1;
    TIMER_A0->CCR[1] = (CurrentNote / 2) - 1;
}

void NoteDurationConfiguration(void){      // Configure note duration and interrupts
    TIMER_A2->CCR[0] = HALF_NOTE;
    TIMER_A2->CCTL[0]=0x0010;
    TIMER_A2->CTL=0b0000000100010100;
    NVIC->ISER[0] |= (1)<<TA2_0_IRQn;
}

void SignalConfigured(const uint16_t notelist[]){ // Signal that configuration is done
    static char noteindex;
    unsigned int delay;
    noteindex=0;
    while(notelist[noteindex]!=NULL) {
        PlayNote(notelist[noteindex]);
        noteindex=noteindex+1;
        for(delay = NoteDuration; delay != 0; delay--) {
         }
    }
}

void TA2_0_IRQHandler(void){
    static char nextnote=0;     //start with first note in a song
    static char insert_rest=1;

    if(TIMER_A2->CCTL[0] & TIMER_A_CCTLN_CCIFG) // CCR0 interrupt
     {
         TIMER_A2->R = 0;                       // Clear Timer A1
         if (insert_rest==1) {                  // Insert a rest (speaker = off)
             insert_rest=0;
             TIMER_A2->CCR[0] = DELAY100MS;
             SpeakerPort->DIR &= ~Speaker;
         } else {                               // Otherwise play the next Note
             insert_rest=1;                     // 100 ms rest
             SpeakerPort->DIR |= Speaker;
             if(CarnivalNotes[nextnote] != NULL){
                 nextnote=nextnote+1;           // increment to next Note
             } else {
                 nextnote = 0;                  // NULL, repeat the piece
             }
             PlayNote(CarnivalNotes[nextnote]); // Play the Note
             switch(CarnivalBeats[nextnote]){   // Play at the specified duration
                 case 1: TIMER_A2->CCR[0] = FULL_NOTE;
                     break;
                 case 2: TIMER_A2->CCR[0] = HALF_NOTE;
                     break;
                 case 4: TIMER_A2->CCR[0] = QUARTER_NOTE;
                     break;
                 case 8: TIMER_A2->CCR[0] = EIGHTH_NOTE;
                     break;
                 case 16: TIMER_A2->CCR[0] = SIXTEENTH_NOTE;
                     break;
                 default: TIMER_A2->CCR[0] = QUARTER_NOTE;
                     break;
             }
         }
         TIMER_A2->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG; // Clear flag
     }
}
