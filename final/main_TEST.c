#include "msp.h"
#include "buttons.h"
#include "LED_indicator.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <TimerAInterrupts.h>
#include <lcd4bits_ece230w25template.h>

#define NUM_MOLES 4

// Game state variables
volatile int activeMole = 4;
volatile int resetFlag = 0; // Set by reset button interrupt
volatile int missFlag = 0;  // 1 = waiting for input; 0 = input received
//volatile int startFlag = 1; // prevents first interrupt from decrementing life
volatile int moleToggle = 0; // toggles between turning off mole and turning on mole
volatile int inputFlag = 0; // prevents excessive button presses
volatile int penaltyFlag = 0; // prevents excessive button presses
int lives = 3;
extern int score = 0;

void main(void)
{
    // Initialize system
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD; // Stop watchdog timer
    LED_init();
    InitializeSwitches();
    InitializeResetSwitch();
    lcd4bits_init();

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
        int pressedButton = -1;

        // Wait for button press or reset
        while (missFlag && !resetFlag)
        {
            if (lives <= 0)
            {
                TIMER_A0->CTL &= ~TIMER_A_CTL_MC_MASK;
                LEDS_OFF();
                lcd_clear();
                lcd_SetLineNumber(FirstLine);
                lcd_puts(" GAME OVER!");
                lcd_SetLineNumber(SecondLine);
                char scoreStr[16];
                sprintf(scoreStr, "Score: %d", score);
                lcd_puts(scoreStr);

                // Wait for reset interrupt
                while (!resetFlag)
                    ;
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
                        lcd_clear();
                        lcd_SetLineNumber(FirstLine);
                        lcd_puts(" BONK!");
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
                        lcd_puts(" MISS!");

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
        if (resetFlag)
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

void TA0_0_IRQHandler(void)
{
    // Clear interrupt flag
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
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
            lcd_puts(" MISS!");
            lcd_SetLineNumber(SecondLine);
            char livesStr[16];
            sprintf(livesStr, "Lives: %d", lives);
            lcd_puts(livesStr);
        }
        LEDS_OFF();
        missFlag = 0;
    }
//    startFlag = 0;
    missFlag = 1;
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
