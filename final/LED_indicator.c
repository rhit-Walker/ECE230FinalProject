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

void LED_init(void)
{ // Initializes all LEDs (add more?)
    LED_PORT->DIR |= LED_ALL_PINS;
    LED_PORT->SEL0 &= ~LED_ALL_PINS;
    LED_PORT->SEL1 &= ~LED_ALL_PINS;
    LED_PORT->OUT &= ~LED_ALL_PINS;
}

// Toggle functions for LEDs:
// LED1
void LED1_ON(void)
{
    LED_PORT->OUT |= LED1_PIN;
}
void LED1_OFF(void)
{
    LED_PORT->OUT &= ~LED1_PIN;
}

// LED2
void LED2_ON(void)
{
    LED_PORT->OUT |= LED2_PIN;
}
void LED2_OFF(void)
{
    LED_PORT->OUT &= ~LED2_PIN;
}

// LED3
void LED3_ON(void)
{
    LED_PORT->OUT |= LED3_PIN;
}
void LED3_OFF(void)
{
    LED_PORT->OUT &= ~LED3_PIN;
}

// LED4
void LED4_ON(void)
{
    LED_PORT->OUT |= LED4_PIN;
}
void LED4_OFF(void)
{
    LED_PORT->OUT &= ~LED4_PIN;
}

void LEDS_OFF(void)
{
    LED_PORT->OUT &= ~LED_ALL_PINS;
}

