#include "msp.h"
#include "buttons.h"
#include "LED_indicator.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <TimerAInterrupts.h>

#define NUM_MOLES 4

// Game state variables
volatile int activeMole = 4;
volatile int resetFlag = 0; // Set by reset button interrupt
volatile int missFlag = 1;  // 1 = waiting for input; 0 = input received
volatile int startFlag = 1;
volatile int moleToggle = 1;
int lives = 3;
int score = 0;

void main(void)
{
    // Initialize system
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD; // Stop watchdog timer
    LED_init();
    InitializeSwitches();
    InitializeResetSwitch();

    CS->KEY = CS_KEY_VAL;             // Unlock clock system
    CS->CTL0 = CS_CTL0_DCORSEL_0;     // Set DCO to 1 MHz
    CS->CTL1 = CS_CTL1_SELS__DCOCLK | // Set SMCLK source as DCO
            CS_CTL1_SELM__DCOCLK;  // Set MCLK source as DCO
    CS->KEY = 0;                      // Lock clock system

    TimerA0Init();
    __enable_irq();

    // Game state variables
    SwitchState buttonStates[NUM_MOLES] = { NotPressed };

    srand(time(NULL));

    while (1)
    {
        missFlag = 1; // Enable timeout detection
        int pressedButton = -1;

        // Wait for button press or reset
        while (missFlag && !resetFlag)
        {
            if (lives <= 0)
            {
                TIMER_A0->CTL &= ~TIMER_A_CTL_MC_MASK;
                LEDS_OFF();
                printf(" GAME OVER! Final Score: %d\n", score);
                printf(" Press Reset Button to restart.\n");

                // Wait for reset interrupt
                while (!resetFlag)
                    ;
            }
            int k;
            for (k = 0; k < NUM_MOLES; k++)
            {
                buttonStates[k] = CheckSwitch(k);

                // Check if a button is pressed
                if (buttonStates[k] == Pressed && missFlag)
                {
                    pressedButton = k;
//                    Debounce();
                    missFlag = 0;

                    // Check if button is correct
                    if (pressedButton == activeMole)
                    {
                        LEDS_OFF();
                        score++;
                        printf(" BONK! Score: %d\n", score);
                    }
                    else
                    {
                        // Wrong button

                        lives--;
                        printf("Wrong button! Lives left: %d\n", lives);
                        LEDS_OFF();
                    }

                    // Restart TimerA0 to begin the next mole cycle
//                    TIMER_A0->CTL &= ~TIMER_A_CTL_MC_MASK;
//                    TIMER_A0->CTL |= TIMER_A_CTL_CLR;
//                    TIMER_A0->CTL |= TIMER_A_CTL_MC__UP;
                }
            }
        }

        // Handle reset
        if (resetFlag)
        {
            printf(" Game Reset!\n");
            score = 0;
            lives = 3;
            activeMole = 1;
            resetFlag = 0;
            startFlag = 1;

            // Restart TimerA0
            TIMER_A0->CTL |= TIMER_A_CTL_MC__UP;
        }
    }
}

void TA0_0_IRQHandler(void)
{
    // Clear interrupt flag
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
    // Check if the mole was hit
    if (moleToggle)
    {
        // Select new mole
        if (missFlag == 1 && startFlag == 0)
        {
            //        lives--;
            printf("Missed! Lives left: %d\n", lives);
        }
        LEDS_OFF();
        activeMole = rand() % NUM_MOLES;
        // Light up new mole
        switch (activeMole)
        {
        case 0:
            LED1_ON();
            break;
        case 1:
            LED2_ON();
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
        LEDS_OFF();
        missFlag = 0;
    }
    startFlag = 0;
    moleToggle = !moleToggle;
}

void PORT2_IRQHandler(void)
{
    if (P2->IFG & RESETBUTTON_PIN)
    {
        P2->IFG &= ~RESETBUTTON_PIN;
        LEDS_OFF();
        resetFlag = 1;
        printf(" Reset button pressed!\n");
    }
}
