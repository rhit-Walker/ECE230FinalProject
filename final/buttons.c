/*
 * buttons.c
 *
 * Description: Button Driver file for the various buttons used in the final project
 *
 *  Created on: Feb 13, 2025
 *      Author: martincw
 */

#include "msp.h"
#include "buttons.h"

#define Ddelay 6500


void InitializeSwitches(void){
    BUTTON_PORT->SEL0 &= ~BUTTON_1_PIN;
    BUTTON_PORT->SEL1 &= ~BUTTON_1_PIN;
    BUTTON_PORT->DIR |= BUTTON_1_PIN;
    BUTTON_PORT->OUT |= BUTTON_1_PIN;
    BUTTON_PORT->REN |= BUTTON_1_PIN;

    //TODO add more buttons
}

SwitchState CheckSwitch1(void){ // simple button check might need to be tweaked
    if((BUTTON_PORT->IN & BUTTON_1_PIN) != BUTTON_1_PIN){ //Check if pressed
        Debounce();
        return Pressed;
    } else {
        return NotPressed;
    }

    //TODO add more checks for other buttons (or one big check for all?)
}

void Debounce(void){
    unsigned int delay;
    for(delay = 0; delay <= Ddelay ; delay++); // delay for a short interval
}

