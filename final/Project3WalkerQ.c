//Done Change the file name to your name
//File Name: ece230w24_25Project3template.c
//Author: Jianjian Song
//Date: December 17, 2024
//ECE230 Winter 2024-2025

//This is template for Project #3 Music Box for ECE230 Winter 2024-2025

//Done Add your name and date to your file
//Quentin Walker 1/21/2025
//Add relevant comments in your own implementation
//Project #3 Music Box
//MCLK & HSMCLK are 48MHz and ACLK is 32kHz
// Features 1 and 2: (1) Play one song with at least six notes, e.g., Twinkle Twinkle Little Star,
//     with one or two note lengths.  (2) Quarter note duration is 0.25 second or a duration approved
//     by the instructor. (3) The song is repeated continuously. (4) A rest is inserted between each note with
//     duration of no less than 100ms and not greater than the shortest note. During the rest notes,
//     no tone should play.
//Feature 3: (1) Play a song with at three different note lengths, e.g., from full note,
//     half note to quarter notes, e.g., Itsy Bity Spider, (2) the same rest is inserted.
//Feature 4: (1) Add a pushbutton switch to pause and resume the new song.
//           Each time the switch is pressed, the song will pause or resume from its previous status.
//           Song will pause within 1ms after switch is pressed. Song shall resume within 10ms after the switch is released.
/*******************************************************************************
 *   ACLK = LFXT = 32kHz and MCLK = SMCLK = HFXT = 48MHz
 *                MSP432P4111
 *             ------------------
 *          /|\|                |
 *           --|RST             |
 *             |           P1.0 |----> LED1 blinks at note durations
 *             |           P2.4 |--> Timer A0 CCR1 - Speaker
 *             |                |
 *          S2 |P1.4            |
 *             |                |
 *    ---------| PJ.0      PJ.2 |------
 *    |        |                      |
 *LFXT @ 32kHz |                |    HFXT @ 48MHz SMCLK, MCLK
 *    |        |                |     |
 *    ---------| PJ.1      PJ.3 |------
 *  ACLK       ------------------
 *
 *******************************************************************************/
/* DriverLib Includes */
#include <msp.h>

/* Standard Includes */
#include <stdio.h>  //NULL is defined in this header file
//#include "csLFXT.h"
//#include "csHFXT.h"

#define SpeakerPort P2
#define Speaker BIT4  //P2.4
#define LED1port P1
#define LED1 BIT0
//PlayerStatus a place holder to implement Start and Pause feature for feature 4 implementation
typedef enum
{
    START, STOP, WAIT, PAUSE
} PlayerStatus;
//Pressed is low
typedef enum _SwitchState
{
    Pressed, NotPressed
} SwitchState;
PlayerStatus Player = PAUSE;
//48MHz system clock and 4:1 prescaler
//12MHz Timer A clock
//Half period count = (48,000,000/Note frequency)
#define SMCLK 44300000 //Hz
#define TimerA0Prescaler 9 //Timer A prescaler
#define TimerA0Clock  SMCLK/TimerA0Prescaler

//rest note, one shot signal of 1/48MHz pulse width is generated,
//which is not audible
//This is not the inserted rest for 100ms when Speaker should remain low voltage.
#define RestNote 1

#define FrequencyA4 440 //Hz
#define FrequencyB4 493.88 //Hz
#define FrequencyC5 523.26 //Hz
#define FrequencyD4 293.66 //Hz
#define FrequencyG3 196 //Hz
#define FrequencyE4 329.63 //Hz
#define FrequencyB3 246.94 //Hz
#define FrequencyA3 220 //Hz
#define FrequencyC4 261.63 //Hz
#define FrequencyBB4 466.16 //Hz
#define FrequencyG4 392 //Hz
#define FrequencyF4 349.23 //Hz
#define FrequencyD5 587.33 //Hz
#define FrequencyE5 659.25 //Hz
#define FrequencyG5 783.99 //Hz
#define FrequencyF5 698.46 //Hz
#define FrequencyA5 880 //Hz


// Note A4 - 440 Hz, B4 - 493.88 Hz, C5 - 523.26 Hz
#define NOTEA4  TimerA0Clock/FrequencyA4    // (48,000,000/440)=4545.45
#define NOTEB4  TimerA0Clock/FrequencyB4   //4049.57
#define NOTEC5  TimerA0Clock/FrequencyC5   //3822.19
#define NOTED4  TimerA0Clock/FrequencyD4
#define NOTEG3  TimerA0Clock/FrequencyG3
#define NOTEE4  TimerA0Clock/FrequencyE4
#define NOTEB3  TimerA0Clock/FrequencyB3
#define NOTEA3  TimerA0Clock/FrequencyA3
#define NOTEC4  TimerA0Clock/FrequencyC4
#define NOTEBB4  TimerA0Clock/FrequencyBB4
#define NOTEG4  TimerA0Clock/FrequencyG4
#define NOTEF4  TimerA0Clock/FrequencyF4
#define NOTED5  TimerA0Clock/FrequencyD5
#define NOTEE5  TimerA0Clock/FrequencyE5
#define NOTEG5  TimerA0Clock/FrequencyG5
#define NOTEF5  TimerA0Clock/FrequencyF5
#define NOTEA5  TimerA0Clock/FrequencyA5


const uint16_t NotesSequence[] = { NOTEA4, NOTEB4, RestNote, NOTEC5, RestNote,
NULL };

//end with NULL
//This is the full song
const uint16_t TwinkleTwinkle[][2] = {
        { NOTEG3, 1 },
        { NOTEG3, 1 },
        { NOTED4, 1 },
        { NOTED4, 1 },  // Quarter notes
        { NOTEE4, 1 },
        { NOTEE4, 1 },
        { NOTED4, 1 },
        { RestNote, 1 }, // Half note, rest
        { NOTEC4, 1 }, { NOTEC4, 1 }, { NOTEB3, 1 }, { NOTEB3, 1 },
        { NOTEA3, 1 },
        { NOTEA3, 1 },
        { NOTEG3, 1 },
        { RestNote, 1 }, // Half note, rest
        { NOTED4, 1 }, { NOTED4, 1 }, { NOTEC4, 1 }, { NOTEC4, 1 },
        { NOTEB3, 1 }, { NOTEB3, 1 }, { NOTEA3, 1 }, { RestNote, 1 }, { NOTED4,
                                                                        1 },
        { NOTED4, 1 }, { NOTEC4, 1 }, { NOTEC4, 1 }, { NOTEB3, 1 },
        { NOTEB3, 1 }, { NOTEA3, 1 }, { RestNote, 1 }, { NOTEG3, 1 }, { NOTEG3,
                                                                        1 },
        { NOTED4, 1 },
        { NOTED4, 1 },  // Quarter notes
        { NOTEE4, 1 }, { NOTEE4, 1 }, { NOTED4, 1 }, { RestNote, 1 }, { NOTEC4,
                                                                        1 },
        { NOTEC4, 1 }, { NOTEB3, 1 }, { NOTEB3, 1 }, { NOTEA3, 1 },
        { NOTEA3, 1 }, { NOTEG3, 1 }, { RestNote, 1 }, { 0, 0 } // End of song
};

const uint16_t ItsyBitsySpider[][2] = {
    {NOTEG3, 1}, {NOTEC4, 1}, {NOTEC4, 1}, {NOTEC4, 1},
    {NOTED4, 1}, {NOTEE4, 2}, {NOTEE4, 1}, {NOTEE4, 1},
    {NOTED4, 1}, {NOTEC4, 1}, {NOTED4, 1}, {NOTEE4, 1},
    {NOTEC4, 4}, {NOTEE4, 2}, {NOTEE4, 1}, {NOTEF4, 1},
    {NOTEG4, 3}, {NOTEG4, 1}, {NOTEF4, 1}, {NOTEE4, 1},
    {NOTEF4, 1}, {NOTEG4, 1}, {NOTEE4, 4}, {NOTEC4, 2},
    {NOTEC4, 1}, {NOTED4, 1}, {NOTEE4, 3}, {NOTEE4, 1},
    {NOTED4, 1}, {NOTEC4, 1}, {NOTED4, 1}, {NOTEE4, 1},
    {NOTEC4, 2}, {NOTEG3, 1}, {NOTEG3, 1}, {NOTEC4, 1},
    {NOTEC4, 1}, {NOTEC4, 1}, {NOTED4, 1}, {NOTEE4, 2},
    {NOTEE4, 1}, {NOTEE4, 1}, {NOTED4, 1}, {NOTEC4, 1},
    {NOTED4, 1}, {NOTEE4, 1}, {NOTEC4, 7}, {RestNote, 1},
    {0, 0} // End of the song
};


const uint16_t TetrisTheme[][2] = {
    {NOTEE5, 2}, {NOTEB4, 1}, {NOTEC5, 1}, {NOTED5, 1},
    {NOTEC5, 1}, {NOTEB4, 1}, {NOTEA4, 1}, {NOTEA4, 1},
    {NOTEC5, 1}, {NOTEE5, 1}, {NOTED5, 1}, {NOTEC5, 1},
    {NOTEB4, 1}, {NOTEB4, 1}, {NOTEC5, 1}, {NOTED5, 1},
    {NOTEE5, 1}, {NOTEC5, 1}, {NOTEA4, 1}, {NOTEA4, 1},
    {NOTEA4, 1}, {NOTED5, 1}, {NOTEF5, 1}, {NOTEA5, 1},
    {NOTEG5, 1}, {NOTEF5, 1}, {NOTEE5, 1}, {NOTED5, 1},
    {NOTEF5, 1}, {NOTEE5, 1}, {NOTEC5, 1}, {NOTEE5, 1},
    {NOTEE5, 1}, {NOTEB4, 1}, {NOTEC5, 1}, {NOTED5, 1},
    {NOTEC5, 1}, {NOTEB4, 1}, {NOTEA4, 1}, {NOTEA4, 1},
    {NOTEC5, 1}, {NOTEE5, 1}, {NOTED5, 1}, {NOTEC5, 1},
    {NOTEB4, 1}, {NOTEB4, 1}, {NOTEC5, 1}, {NOTED5, 1},
    {NOTEE5, 1}, {NOTEC5, 1}, {NOTEA4, 1}, {NOTEA4, 1},
    {NOTEA4, 1}, {NOTEA4, 1}, {RestNote, 1}, {RestNote, 1},
    {NOTEF5, 1}, {NOTED5, 1}, {NOTEF5, 1}, {NOTEG5, 1},
    {NOTEA5, 1}, {NOTEG5, 1}, {NOTEF5, 1}, {NOTEE5, 1},
    {NOTED5, 1}, {NOTEF5, 1}, {NOTEG5, 1}, {NOTEA5, 1},
    {NOTEG5, 1}, {NOTEF5, 1}, {NOTEE5, 1}, {NOTEC5, 1},
    {NOTEA4, 1}, {NOTEB4, 1}, {NOTEC5, 1}, {NOTED5, 1},
    {NOTEE5, 1}, {NOTED5, 1}, {NOTEC5, 1}, {NOTEB4, 1},
    {NOTEC5, 1}, {NOTEE5, 1}, {NOTEG5, 1}, {NOTEF5, 1},
    {NOTED5, 1}, {NOTEF5, 1}, {NOTEA5, 1}, {NOTEG5, 1},
    {0, 0}
};

//Option 1 of 2-D music sheet
//1 = QUARTER_NOTE. 2 = HALF_NOTE. 4 = FULL_NOTE.
//Exteneded music sheet array: {note, duration} pairs
//each row has two items: (Note, duration)
const uint16_t ANewSong[][2] = { NOTEA4, 1, NOTEC5, 1, RestNote, 2,
NOTEA4,
                                 4, NOTEB4, 2, NOTEC5, RestNote, 1,
                                 NULL,
                                 NULL };

//Option 2 of 2-D music sheet
const uint16_t ANewSongNotes[] = { NOTEA4, RestNote, NOTEB4, NOTEC5, RestNote,
NULL };
const uint16_t ANewSongBeats[] = { 1, 2, 4, 1, 1, 4, 2, 2, 4, NULL };

//===================================
//Configure Speaker as Timer A0 CCR1 output pin
//Configure Timer A0 to produce sound on CCR1 output compare

void PlayerConfiguraion(void)
{

    SpeakerPort->DIR |= Speaker;            // set P2.4 as output
    SpeakerPort->SEL0 |= Speaker;           // P2.4 set to TA0.1
    SpeakerPort->SEL1 &= ~Speaker;  // Option 0b01
//set Timer A0 CCR0 compare for period and CCR1 compare for duty cycle of a note
    TIMER_A0->CCR[0] = 0;                 // Initialize CCR0
    TIMER_A0->CCR[1] = 0;                 // Initialize CCR1
    TIMER_A0->CCTL[1] = TIMER_A_CCTLN_OUTMOD_7; // Reset/Set output mode
    TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK |   // Use SMCLK
            TIMER_A_CTL_MC__UP |       // Up Mode
            TIMER_A_CTL_CLR;           // Clear timer
}

//================================
//Play a note by sending its Timer A0 cycle count to this subroutine
//This could be insert in the interrupt service
void PlayNote(unsigned int CurrentNote)
{
    if (CurrentNote == RestNote)
    {
        TIMER_A0->CCR[0] = 0;               // Stop PWM
        TIMER_A0->CCR[1] = 0;               // Set duty cycle to 0
        SpeakerPort->DIR &= ~Speaker;       // Disable speaker output
    }
    else
    {
        TIMER_A0->CCR[0] = CurrentNote - 1; // Set PWM period
        TIMER_A0->CCR[1] = (CurrentNote / 2) - 1; // 50% duty cycle
        SpeakerPort->DIR |= Speaker;        // Enable speaker output
    }
}

//================================

//Exercise 4-3: similar to Exercis 4-3 but uses Timer A1 Up mode and CCR0
//Option 1: Configure Timer A Up-Mode and CCR0 compare interrupt
//ACLK at 32 kHz. prescaler 1:1
//to generate note duration: 0.25 second, 0.5 second, 1 second, etc.
//by setting the time duration in  TIMER_A1->CCR[0]
//configure LED1 to toggle each time CCR0 compare interrupt occurs
//Option 2: uses Exercise 4-3 approach

#define ACLK 32650 //Hz
#define FULL_NOTE       ACLK    //1 second
#define HALF_NOTE   ACLK/2  //0.5 second
#define QUARTER_NOTE    ACLK/4  //0.25 second
#define EIGHTH_NOTE     ACLK/8
#define DELAY100MS  ACLK/10    //100ms timer delay

void NoteDurationConfiguration()
{

    LED1port->DIR |= LED1;                    // Set P1.0 as output for LED1
    LED1port->OUT &= ~LED1;
    /* Configure Timer_A1 and CCRs */
    // Set initial period in CCR0 register. This assumes timer starts at 0
    TIMER_A1->CCR[0] = QUARTER_NOTE;
//    TIMER_A1->CCR[0] = HALF_NOTE;
    // Configure CCR0 for Compare mode with interrupt enabled (no output mode - 0)
    // Done configure CCR0
    TIMER_A1->CCTL[0] = 0x0010;
    // Configure Timer_A1 in UP Mode with source ACLK prescale 1:1 and no interrupt
    // Done configure Timer_A1: ACLK, UP mode, TACLR=1 bit 2, no interrupt
    TIMER_A1->CTL = 0b0000000100010100;  //0x0114

    /* Configure global interrupts and NVIC */
    // Enable TA1 TA1CCR0 compare interrupt
    // Done enable interrupt by setting IRQ bit in NVIC ISER0 register
    NVIC->ISER[0] |= (1) << TA1_0_IRQn;
    __enable_irq();
}


//================================
//Play the note list once to test the notes
#define NoteDuration 1000000
void PlayAllNotesOnce(const uint16_t notelist[])
{
    static char noteindex;
    unsigned int delay;
    noteindex = 0;
    while (notelist[noteindex] != NULL)
    {
        PlayNote(notelist[noteindex]);
        noteindex = noteindex + 1;
        for (delay = NoteDuration; delay != 0; delay--)
        {
        }
    } //end while()
} //end PlayAllNotesOnce(void)

//Done complete start switch configuration function
//switch port and masks
#define SwitchPort P1   //Port 1
#define Switch1   0b00000010    //P1.1
#define Switch2   0b00010000    //P1.4
//#define Switch1   0b00100000    //P1.5

void StartSwitchConfiguration(void)
{
    SwitchPort->DIR &= ~(Switch1 | Switch2);
    SwitchPort->REN |= (Switch1 | Switch2);
    SwitchPort->OUT |= (Switch1 | Switch2);
    SwitchPort->IES |= (Switch1 | Switch2);
    SwitchPort->IFG &= ~Switch1;
    SwitchPort->IE |= (Switch1 | Switch2);
    NVIC->ISER[1] |= (1 << (PORT1_IRQn - 32));
}

SwitchState CheckStopSwitch(void)
{
    //check Switch S1 and return Pressed or NotPressed
    if ((SwitchPort->IN & Switch1) == 0)
    {
        return Pressed;
    }
    else
    {
        return NotPressed;
    }
}

int main(void)
{
    P7->DIR |= BIT0;    // P7.0 output
    P7->SEL0 |= BIT0;
    P7->SEL1 &= ~BIT0;

//    SwitchState StopSwitch = NotPressed;

    /* Stop Watchdog timer */
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;
    CS->KEY = CS_KEY_VAL;
    //Done Sets MCLK and SMCLK to 48MHz HFXT
    CS->CTL2 |= CS_CTL2_HFXT_EN;
    CS->CTL2 &= ~CS_CTL2_HFXTDRIVE;
    CS->CTL2 |= CS_CTL2_HFXTFREQ_6;
    CS->CTL1 &= ~(CS_CTL1_SELM_MASK | CS_CTL1_SELS_MASK);
    CS->CTL1 |= CS_CTL1_SELS__HFXTCLK;
    CS->CTL1 |= CS_CTL1_SELM__HFXTCLK;   // Set MCLK to HFXT
//    CS->CTL1 |= (0b101 << CS_CTL1_SELM_OFS) | (0b101 << CS_CTL1_SELS_OFS);
    //Done Configure LFXT to use 32kHz crystal, source to ACLK
    CS->CTL2 |= CS_CTL2_LFXT_EN;
    CS->CTL2 |= CS_CTL2_LFXTDRIVE_0;
    CS->CTL1 &= ~CS_CTL1_SELA_MASK;
    CS->CTL1 |= CS_CTL1_SELA__LFXTCLK;
    CS->KEY = 0;
    PlayerConfiguraion();
//test to see if the player works before switching on interrupt\
//This is the idea of divide and conquer

    NoteDurationConfiguration();    //start time duration interrupt

    StartSwitchConfiguration();

    // Enable global interrupt
//    PlayAllNotesOnce(NotesSequence);
    Player = PAUSE;   //place holder for Feature 3 and 3.
    while (1)
    {
//
    }  //while (1)

    while (1)
        ;   //hold the program here
} // end main()

/* Timer_A1 CCR0 interrupt service routine, i.e., the handler */
//Timer A1 Up CCR0 compare interrupt to generate note duration
//by setting up TIMER_A1->CCR[0].
//The same compare interrupt can be used to generate 100ms rest note
//by turning off Speaker for 100ms with a compare interrupt
//Toggle LED1 to show note durations
//This handler name TimerA1CCR0ServiceRoute is not correct
//Done Find the right handler name for Timer A1 CCR0 interrupt
void TA1_0_IRQHandler(void)
{
    static char nextnote = 0;
    static char insert_rest = 1;

    if (TIMER_A1->CCTL[0] & TIMER_A_CCTLN_CCIFG && Player == START)
    {
        TIMER_A1->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG; // Clear interrupt flag
        LED1port->OUT ^= LED1;                    // Toggle LED for debugging

        if (insert_rest)
        {
            // Insert a rest (100ms)
            TIMER_A1->CCR[0] += DELAY100MS;       // 100ms rest
            PlayNote(RestNote);                  // Silence the speaker
            insert_rest = 0;                     // Next, play the note
        }
        else
        {
            // Play the next note
            uint16_t note_frequency = ItsyBitsySpider[nextnote][0];
            uint16_t note_duration = ItsyBitsySpider[nextnote][1];

            if (note_frequency == 0 && note_duration == 0)
            {
                // End of song, restart
                nextnote = 0;
                return;
            }


            PlayNote(note_frequency);                     // Play the note
            TIMER_A1->CCR[0] = note_duration * QUARTER_NOTE; // Set duration
            nextnote++;
            insert_rest = 1;                              // Insert rest next
        }
    }
}

//end interrupt service
void debounce(void)
{
//delay time for debouncing switches
    unsigned int debounceTime = 7000;
    while (debounceTime--)
        ;
} //end debounce()

void PORT1_IRQHandler(void)
{
    if (P1->IFG & Switch1)
    {

        P1->IFG &= ~Switch1;  // Clear the interrupt flag
        debounce();

            // Toggle the Player state
            if (Player == START)
            {
                TIMER_A0->CTL &= ~TIMER_A_CTL_MC_MASK;  // Stop Timer_A0
                TIMER_A1->CTL &= ~TIMER_A_CTL_MC_MASK;  // Stop Timer_A1
                SpeakerPort->DIR &= ~Speaker;          // Disable speaker output
                Player = PAUSE;
            }
            else if (Player == PAUSE)
            {
                TIMER_A0->CTL |= TIMER_A_CTL_MC__UP;   // Start Timer_A0
                TIMER_A1->CTL |= TIMER_A_CTL_MC__UP;   // Start Timer_A1
                SpeakerPort->DIR |= Speaker;           // Enable speaker output
                Player = START;
            }
    }
}

