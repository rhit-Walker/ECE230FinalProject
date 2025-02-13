/*
 * TEST MAIN
 */

#include "msp.h"
#include "buttons.h"
#include "LED_indicator.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


/**
 * main.c
 */
void main(void)
{

    LED_init();
    InitializeSwitches();
    SwitchState Button_Check, Button1Status; // TODO add more as we get there
    CurrentLED ActiveLED;
    Button_Check = NotPressed;  //idea is that we'll confirm a button pressed by checking if this variable
    Button1Status = NotPressed; //is pressed or not, and changing the needed one to accomodate our system
    int min = 0;
    int max = 3;

	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
	while(1){

	    srand(time(NULL));
	    ActiveLED = rand() % (max - min + 1) + min; // Generates a random LED

	    switch(ActiveLED){ // assuming the last button press turned off the LED
	    case LED1:
	        LED1_toggle();
	        break;
        case LED2:
            LED2_toggle();
            break;
        case LED3:
            LED3_toggle();
            break;
        case LED4:
            LED4_toggle();
            break;
        default:
            LED_OFF();
            break;
	    }

	    while(Button_Check==NotPressed){
	        Button1Status = CheckSwitch1();
	        Debounce();
	        if(Button1Status==Pressed){
	            Button_Check = Pressed;
	            break;
	        }
	        //Button2Status = CheckSwitch2() etc etc
	    }

	    if(Button_Check==Pressed){
	        Button_Check = NotPressed;
	        if(Button1Status==Pressed){
	            Button1Status = NotPressed;
	            if(ActiveLED==LED1){
	                LED1_toggle();
	            } else{
	                LED_OFF();
	            }
	        }
//	        if(Button2Status==Pressed){ // TODO implement additional buttons and stuff
//	            Button2Status = NotPressed;
//	            LED2_toggle();
//	            break;
//	        }

	    }

	}
}
