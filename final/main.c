#include "msp.h"
#include "main.h"

/**
 * main.c
 */



void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
	initTimer();
	initButtons();
	initLEDS();
	initLCD8Bit();
}
