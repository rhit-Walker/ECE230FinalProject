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

#define Ddelay 17000

void InitializeSwitches(void)
{
    // Set P1.4, P1.5, P1.6, P1.7 as inputs with pull-ups
    BUTTON_PORT->SEL0 &= ~(BUTTON_1_PIN | BUTTON_2_PIN | BUTTON_3_PIN
            | BUTTON_4_PIN );
    BUTTON_PORT->SEL1 &= ~(BUTTON_1_PIN | BUTTON_2_PIN | BUTTON_3_PIN
            | BUTTON_4_PIN );
    BUTTON_PORT->DIR &= ~(BUTTON_1_PIN | BUTTON_2_PIN | BUTTON_3_PIN
            | BUTTON_4_PIN ); // Inputs
    BUTTON_PORT->REN |= (BUTTON_1_PIN | BUTTON_2_PIN | BUTTON_3_PIN
            | BUTTON_4_PIN );  // Enable resistors
    BUTTON_PORT->OUT |= (BUTTON_1_PIN | BUTTON_2_PIN | BUTTON_3_PIN
            | BUTTON_4_PIN );  // Set pull-ups
}

//void InitializeResetSwitch(void)
//{
//    BUTTON_PORT->DIR &= ~RESETBUTTON_PIN; // P2.3 as input
//    BUTTON_PORT->REN |= RESETBUTTON_PIN;  // Enable pull-up resistor
//    BUTTON_PORT->OUT |= RESETBUTTON_PIN;
//    BUTTON_PORT->IES |= RESETBUTTON_PIN;  // Interrupt on falling edge
//    BUTTON_PORT->IFG &= ~RESETBUTTON_PIN; // Clear interrupt flag
//    BUTTON_PORT->IE |= RESETBUTTON_PIN;   // Enable interrupt for P2.3
//
//    NVIC->ISER[1] = 1 << ((PORT2_IRQn) & 31);
//}
//TODO add more buttons

SwitchState CheckSwitch(int switchNum)
{
    uint8_t pin;
    switch (switchNum)
    {
    case 0:
        pin = BUTTON_1_PIN;
        break;
    case 1:
        pin = BUTTON_2_PIN;
        break;
    case 2:
        pin = BUTTON_3_PIN;
        break;
    case 3:
        pin = BUTTON_4_PIN;
        break;
    default:
        return NotPressed;
    }

    // Check if the button is pressed
    if ((BUTTON_PORT->IN & pin) == 0)
    {
        Debounce();
        return Pressed;
    }
    return NotPressed;
}

void Debounce(void)
{
    unsigned int delay;
    for (delay = 0; delay <= Ddelay; delay++)
        ; // delay for a short interval
}
