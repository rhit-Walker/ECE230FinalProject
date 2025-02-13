
/*! \file */
/*!b
 * LED_indicator.c
 *
 * Description:
 *
 *  Created on: 2/10/25
 *      Author: Cruz Martinez
 */

#include "msp.h"
#include "LED_indicator.h"


void LED_init(void) { // Initializes all LEDs (add more?)
    LED_PORT2->DIR |= LED_ALL_PINS;
    LED_PORT2->SEL0 &= ~LED_ALL_PINS;
    LED_PORT2->SEL1 &= ~LED_ALL_PINS;
    LED_PORT2->OUT &= ~LED_ALL_PINS;

    LED_PORT1->DIR |= LED1_PIN;
    LED_PORT1->SEL0 &= ~LED1_PIN;
    LED_PORT1->SEL1 &= ~LED1_PIN;
    LED_PORT1->OUT &= ~LED1_PIN;
}

// Toggle functions for LEDs:
void LED1_toggle(void) {
    LED_PORT1->OUT = (LED_PORT1->OUT) ^ LED1_PIN;
}

void LED2_toggle(void) {
    LED_PORT2->OUT = (LED_PORT2->OUT) ^ LED2_PIN;
}

void LED3_toggle(void) {
    LED_PORT2->OUT = (LED_PORT2->OUT) ^ LED3_PIN;
}

void LED4_toggle(void) {
    LED_PORT2->OUT = (LED_PORT2->OUT) ^ LED4_PIN;
}

void LED_OFF(void){
    LED_PORT2->OUT = ~LED_ALL_PINS;
    LED_PORT1->OUT = ~LED1_PIN;
}

