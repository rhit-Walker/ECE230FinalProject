


#include "msp.h"
#include "buttons.h"
#include "LED_indicator.h"
#include "speaker.h"
#include "csHFXT.h"
#include "csLFXT.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <TimerAInterrupts.h>
#include <lcd4bits_ece230w25template.h>

/*
 * main.c the main driver for our game
 */

#define NUM_MOLES 4

// Game state variables
volatile int activeMole = 4;
volatile int resetFlag = 0; // Set by reset button interrupt
volatile int missFlag = 0;  // 1 = waiting for input; 0 = input received
//volatile int startFlag = 1; // prevents first interrupt from decrementing life
volatile int moleToggle = 0; // toggles between turning off mole and turning on mole
volatile int inputFlag = 0; // prevents excessive button presses
volatile int penaltyFlag = 0; // prevents excessive button presses
volatile int oneSecondDelay = 23438; // modifiable to speed up
volatile int startingDelay = 23438;
int decreaseDelay = 500;
int lives = 3;
extern int score = 0;
int switchOne = 0;  // defined for the start
int switchFour = 3; // defined for the reset
int switchLCD = 25000; // simple delay to switch the LCD screen in the end state
int flipFlop = 0;
int victory = 25;   // victory requirement



void main(void)
{
    SwitchState buttonStates[NUM_MOLES] = { NotPressed }; // Game state variables
    // Initialize system
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD; // Stop watchdog timer
    configHFXT();
    configLFXT();


    LED_init();
    InitializeSwitches();
    //InitializeResetSwitch();
    lcd4bits_init();

    CS->KEY = CS_KEY_VAL;             // Unlock clock system
    CS->CTL0 = CS_CTL0_DCORSEL_0;     // Set DCO to 1 MHz
    CS->CTL1 = CS_CTL1_SELS__DCOCLK | // Set SMCLK source as DCO
            CS_CTL1_SELM__DCOCLK;  // Set MCLK source as DCO
    CS->KEY = 0;                      // Lock clock system

    PlayerConfiguration();
    SignalConfigured();

    lcd_clear();
    lcd_SetLineNumber(FirstLine);
    lcd_puts("  Whack-A-Mole");
    lcd_SetLineNumber(SecondLine);
    lcd_puts("Press 1 to Start");

    while(buttonStates[switchOne] != Pressed){
        buttonStates[switchOne] = CheckSwitch(switchOne); // wait for button 1 to be pressed
    }                                                     // before starting

    NoteDurationConfiguration();
    TimerA3Init(startingDelay);

    __enable_irq();

    srand(time(NULL));

    while (1)
    {

        int pressedButton = -1;

        // Wait for button press or reset
        while (missFlag && !resetFlag)
        {
            if (lives <= 0)
            {
                TIMER_A0->CTL &= ~TIMER_A_CTL_MC_MASK;
                LEDS_OFF();
                lcd_clear();
                char displaystr[16];
                if(score >= victory){  // display victory or game over based on score
                    sprintf(displaystr, "    VICTORY!");
                } else {
                    sprintf(displaystr, "   GAME OVER!");
                }
                lcd_SetLineNumber(FirstLine);
                lcd_puts(displaystr);
                lcd_SetLineNumber(SecondLine);
                char scoreStr[16];
                sprintf(scoreStr, "Score: %d", score);
                lcd_puts(scoreStr);


                // disable the IRQ and wait for the reset button to be pressed
                __disable_irq();
                while (1){
                    buttonStates[switchFour] = CheckSwitch(switchFour); // wait for button 4 to be pressed
                    if(buttonStates[switchFour] == Pressed){

                        LEDS_OFF(); // Reset lives and Score
                        lives = 4;
                        score = 0;
                        startingDelay = oneSecondDelay;
                        Debounce();
                        PlayerConfiguration(); // reconfigure
                        NoteDurationConfiguration();

                        __enable_irq();

                        break;
                    }
                    if(switchLCD == 0 && flipFlop == 1){
                        flipFlop = 0;
                        switchLCD = 25000;
                        lcd_clear();
                        lcd_SetLineNumber(FirstLine);
                        lcd_puts(displaystr);
                        lcd_SetLineNumber(SecondLine);
                        lcd_puts("Press 4 to Start");
                    }
                    if(switchLCD == 0 && flipFlop == 0){
                        flipFlop = 1;
                        switchLCD = 25000;
                        lcd_clear();
                        lcd_SetLineNumber(FirstLine);
                        lcd_puts(displaystr);
                        lcd_SetLineNumber(SecondLine);
                        sprintf(scoreStr, "Score: %d", score);
                        lcd_puts(scoreStr);
                    }
                    switchLCD--;
                }
            }
            int k;
            for (k = 0; k < NUM_MOLES; k++)
            {
                buttonStates[k] = CheckSwitch(k);

                // Check if a button is pressed
                if (buttonStates[k] == Pressed && missFlag && !inputFlag)
                {
                    inputFlag = 1;
                    pressedButton = k;
//                    Debounce();
                    missFlag = 0;

                    // Check if button is correct
                    if (pressedButton == activeMole&& !moleToggle)
                    {
//                        activeMole = -1;
                        LEDS_OFF();
                        score++;
                        if(score >= 5){
                            startingDelay = startingDelay - decreaseDelay;
                        }
                        lcd_clear();
                        lcd_SetLineNumber(FirstLine);
                        lcd_puts("     BONK! ");
                        lcd_SetLineNumber(SecondLine);
                        char scoreStr[16];
                        sprintf(scoreStr, "Score: %d", score);
                        lcd_puts(scoreStr);

                    }
                    else if(!penaltyFlag)
                    {
                        // Wrong button
                        penaltyFlag = 1;
                        lives--;
                        lcd_clear();
                        lcd_SetLineNumber(FirstLine);
                        lcd_puts("     MISS! ");

                        lcd_SetLineNumber(SecondLine);
                        char livesStr[16];
                        sprintf(livesStr, "Lives: %d", lives);
                        lcd_puts(livesStr);

                        LEDS_OFF();
                    } else{
                        continue;
                    }

                    // Restart TimerA0 to begin the next mole cycle
//                    TIMER_A0->CTL &= ~TIMER_A_CTL_MC_MASK;
//                    TIMER_A0->CTL |= TIMER_A_CTL_CLR;
//                    TIMER_A0->CTL |= TIMER_A_CTL_MC__UP;
                }
            }
        }
        // Handle reset
        if (resetFlag) // I think this is dead code
        {
            lcd_clear();
            lcd_SetLineNumber(FirstLine);
            lcd_puts(" GAME RESET!");
            score = 0;
            lives = 3;
            activeMole = 4;
            resetFlag = 0;
//            startFlag = 1;

            // Restart TimerA0
            TIMER_A0->CTL |= TIMER_A_CTL_MC__UP;
        }
    }
}

void TA3_0_IRQHandler(void)
{
    // Clear interrupt flag
    TIMER_A3->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
    // Check if the mole was hit
    penaltyFlag = 0;
    if (moleToggle)
    {
        inputFlag = 0;
        // Select new mole
        LEDS_OFF();
        activeMole = rand() % NUM_MOLES;
        // Light up new mole
        switch (activeMole)
        {
        case 0:
            LED1_ON();
            break;
        case 1:
            LED_PORT->OUT |= LED2_PIN;
            break;
        case 2:
            LED3_ON();
            break;
        case 3:
            LED4_ON();
            break;
        default:
            missFlag = 0;
            LEDS_OFF();
            break;
        }
    }
    else
    {
        if (missFlag == 1  && !penaltyFlag)
        {
            penaltyFlag = 1;
            lives--;
            lcd_clear();
            lcd_puts("     MISS!");
            lcd_SetLineNumber(SecondLine);
            char livesStr[16];
            sprintf(livesStr, "Lives: %d", lives);
            lcd_puts(livesStr);
        }
        LEDS_OFF();
        missFlag = 0;
    }
//    startFlag = 0
    missFlag = 1;
    moleToggle = !moleToggle;
    TimerA3Init(startingDelay);
}

//void PORT2_IRQHandler(void)
//{
//    if (P2->IFG & RESETBUTTON_PIN)
//    {
//        P2->IFG &= ~RESETBUTTON_PIN;
//        LEDS_OFF();
//        resetFlag = 1;
//        printf(" Reset button pressed!\n");
//    }
//}
