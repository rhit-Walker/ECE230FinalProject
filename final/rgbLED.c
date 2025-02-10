/*! \file */
/*!
 * rgbLED.c
 *
 * Description: RGB driver for LED2 on MSP432P4111 Launchpad
 *
 *  Created on:
 *      Author:
 */

#include "msp.h"
#include "rgbLED.h"

#define Bit1 0b00000010

void RGBLED_init(void) {
    // DONE set LED2 pins as GPIO outputs
    RGB_PORT->DIR |= RGB_ALL_PINS;
    RGB_PORT->SEL0 &= ~RGB_ALL_PINS;
    RGB_PORT->SEL1 &= ~RGB_ALL_PINS;

    // DONE set LED2 outputs to LOW
    RGB_PORT->OUT &= ~RGB_ALL_PINS;
}

void RGBLED_toggleRed(void) {
    // XOR bit 0 of Port 2 for red LED
    RGB_PORT->OUT = (RGB_PORT->OUT) ^ RGB_RED_PIN;
}

void RGBLED_toggleGreen(void) {
    // Done toggle bit 1 of Port 2 OUT register
    RGB_PORT->OUT = (RGB_PORT->OUT) ^ RGB_GREEN_PIN;
}

void RGBLED_toggleBlue(void) {
    // Done toggle bit 2 of Port 2 OUT register
    RGB_PORT->OUT = (RGB_PORT->OUT) ^ RGB_BLUE_PIN;
}

void RGBLED_setColor(Colors color) {
    RGB_PORT->OUT = (RGB_PORT->OUT & ~(RGB_RED_PIN | RGB_GREEN_PIN | RGB_BLUE_PIN)) | color;
}

