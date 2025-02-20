    //TODO Change the file name to your name
//File Name: ece230w24_25Project3template.c
//Author: Cruz Martinez
//Date: January 22, 2025
//ECE230 Winter 2024-2025

//This is template for Project #3 Music Box for ECE230 Winter 2024-2025

    //TODO Add your name and date to your file
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
#include "csHFXT.h"
#include "csLFXT.h"

#define SpeakerPort P2
#define Speaker BIT4 //P2.4
#define LED1port P1
#define LED1 BIT0
//PlayerStatus a place holder to implement Start and Pause feature for feature 4 implementation
typedef enum  {STOP, START, WAIT, PAUSE} PlayerStatus;
//Pressed is low
typedef enum _SwitchState {Pressed, NotPressed} SwitchState;

//48MHz system clock and 4:1 prescaler
//12MHz Timer A clock
//Half period count = (48,000,000/Note frequency)
#define SMCLK 48000000 //Hz
#define TimerA0Prescaler 4 //Timer A prescaler
#define TimerA0Clock  SMCLK/TimerA0Prescaler

//rest note, one shot signal of 1/48MHz pulse width is generated,
//which is not audible
//This is not the inserted rest for 100ms when Speaker should remain low voltage.
#define RestNote 1 // change 3277?

#define FrequencyG3 195.998 // All in Hz
#define FrequencyGsAb3 207.652
#define FrequencyA3 220
#define FrequencyAsBb3 233.082
#define FrequencyB3 246.942
#define FrequencyC4 261.626
#define FrequencyCsDb4 277.183
#define FrequencyD4 293.665
#define FrequencyDsEb4 311.127
#define FrequencyE4 329.628
#define FrequencyF4 349.228
#define FrequencyFsGb4 369.994
#define FrequencyG4 391.995
#define FrequencyGsAb4 415.305
#define FrequencyA4 440
#define FrequencyAsBb4 466.164
#define FrequencyB4 493.88
#define FrequencyC5 523.26
#define FrequencyCsDb5 554.365
#define FrequencyD5 587.33
#define FrequencyDsEb5 622.254
#define FrequencyE5 659.255
#define FrequencyF5 698.456
#define FrequencyFsGb5 739.989
#define FrequencyG5 783.991
#define FrequencyGsAb5 830.609
#define FrequencyA5 880
#define FrequencyAsBb5 932.328
#define FrequencyB5 987.767
#define FrequencyC6 1046.502
#define FrequencyD6 1174.659

// Note A4 - 440 Hz, B4 - 493.88 Hz, C5 - 523.26 Hz
#define NOTEG3  TimerA0Clock/FrequencyG3
#define NOTEGsAb3  TimerA0Clock/FrequencyGsAb3
#define NOTEA3  TimerA0Clock/FrequencyA3
#define NOTEAsBb3  TimerA0Clock/FrequencyAsBb3
#define NOTEB3  TimerA0Clock/FrequencyB3
#define NOTEC4  TimerA0Clock/FrequencyC4
#define NOTECsDb4  TimerA0Clock/FrequencyCsDb4
#define NOTED4  TimerA0Clock/FrequencyD4
#define NOTEDsEb4  TimerA0Clock/FrequencyDsEb4
#define NOTEE4  TimerA0Clock/FrequencyE4
#define NOTEF4  TimerA0Clock/FrequencyF4
#define NOTEFsGb4  TimerA0Clock/FrequencyFsGb4
#define NOTEG4  TimerA0Clock/FrequencyG4
#define NOTEGsAb4  TimerA0Clock/FrequencyGsAb4
#define NOTEA4  TimerA0Clock/FrequencyA4    // (48,000,000/440)=4545.45
#define NOTEAsBb4  TimerA0Clock/FrequencyAsBb4
#define NOTEB4  TimerA0Clock/FrequencyB4   //4049.57
#define NOTEC5  TimerA0Clock/FrequencyC5   //3822.19
#define NOTECsDb5  TimerA0Clock/FrequencyCsDb5
#define NOTED5  TimerA0Clock/FrequencyD5
#define NOTEDsEb5  TimerA0Clock/FrequencyDsEb5
#define NOTEE5  TimerA0Clock/FrequencyE5
#define NOTEF5  TimerA0Clock/FrequencyF5
#define NOTEFsGb5  TimerA0Clock/FrequencyFsGb5
#define NOTEG5  TimerA0Clock/FrequencyG5
#define NOTEGsAb5  TimerA0Clock/FrequencyGsAb5
#define NOTEA5  TimerA0Clock/FrequencyA5
#define NOTEAsBb5  TimerA0Clock/FrequencyAsBb5
#define NOTEB5  TimerA0Clock/FrequencyB5
#define NOTEC6  TimerA0Clock/FrequencyC6
#define NOTED6  TimerA0Clock/FrequencyD6

//const uint16_t NotesSequence[] = {NOTEG3, NOTEGsAb3, NOTEA3, NOTEAsBb3, NOTEB3, NOTEC4, NOTECsDb4, NOTED4, NOTEDsEb4,
//                                  NOTEE4, NOTEF4, NOTEFsGb4, NOTEG4, NOTEGsAb4, NOTEA4, NOTEAsBb4, NOTEB4, NOTEC5, NOTECsDb5,
//                                  NOTED5, NOTEDsEb5, NOTEE5, NOTEF5, NOTEFsGb5, NOTEG5, NOTEGsAb5, NOTEA5, NOTEB5, NOTEC6, NOTED6,
//                                  RestNote, NULL};

const uint16_t NotesSequence[] = {NOTEE5, NOTEDsEb5, NOTEE5, NOTEFsGb5, NOTEGsAb5, NOTEE5, RestNote, NULL};


//end with NULL
//This is the full song
//const uint16_t TwinkleTwinkleLittleStar[] = {NOTEG3, NOTEG3, NOTED4, NOTED4, NOTEE4, NOTEE4, NOTED4, RestNote,
//                               NOTEC4, NOTEC4, NOTEB3, NOTEB3, NOTEA3, NOTEA3, NOTEG3, RestNote,
//                               NOTED4, NOTED4, NOTEC4, NOTEC4, NOTEB3, NOTEB3, NOTEA3, RestNote,
//                               NOTED4, NOTED4, NOTEC4, NOTEC4, NOTEB3, NOTEB3, NOTEA3, RestNote,
//                               NOTEG3, NOTEG3, NOTED4, NOTED4, NOTEE4, NOTEE4, NOTED4, RestNote,
//                               NOTEC4, NOTEC4, NOTEB3, NOTEB3, NOTEA3, NOTEA3, NOTEG3, RestNote,
//                               RestNote, RestNote, RestNote, RestNote,
//                               NULL};

//Option 1 of 2-D music sheet
//1 = QUARTER_NOTE. 2 = HALF_NOTE. 4 = FULL_NOTE.
//Exteneded music sheet array: {note, duration} pairs
//each row has two items: (Note, duration)
//const uint16_t ANewSong[][2] = {NOTEA4, 1, NOTEC5, 1, RestNote, 2,
//                              NOTEA4, 4, NOTEB4, 2, NOTEC5, RestNote, 1,
//                              NULL, NULL};

//Option 2 of 2-D music sheet
//const uint16_t PlayfulPNotes[] = {RestNote, NOTEC4, NOTEF4, NOTEA4, NOTEC5, NOTEAsBb4, NOTEG4, NOTEE4, NOTEF4, RestNote, RestNote,
//                                  NOTEC4, NOTEF4, NOTEA4, NOTEC5, NOTEAsBb4, NOTEG4, NOTEE4, NOTEF4, RestNote, RestNote,
//                                  NOTEA3, NOTED4, NOTEF4, NOTEA4, NOTEG4, NOTEE4, NOTECsDb4, NOTED4, RestNote, RestNote,
//                                  NOTEA3, NOTED4, NOTEF4, NOTEA4, NOTEG4, NOTEE4, NOTECsDb4, NOTED4, RestNote, RestNote,
//                                  NOTEC4, NOTEF4, NOTEA4, NOTEC5, NOTEAsBb4, NOTEG4, NOTEE4, NOTEF4, RestNote, RestNote,
//                                  NOTEC4, NOTEF4, NOTEA4, NOTEC5, NOTEAsBb4, NOTEG4, NOTEC5, NOTEA4, RestNote,
//                                  NOTEG4, NOTEE4, NOTEA4, NOTEF4, RestNote, NOTEE4, NOTEC4, NOTEF4, NOTED4, RestNote, // 2
//                                  NOTEC4, NOTEA3, NOTED4, NOTEB3, NOTEF4, NOTEF4, NOTEF4, NOTEF5, NOTEF5, NOTEE5, NOTEC5, NOTEF5, NOTED5, RestNote, RestNote,
//                                  NOTEA4, NOTEC5, NOTEE5, NOTEA5, NOTEA5, NOTEA5, NOTEG5, NOTEF5, NOTEE5, NOTEE5, RestNote,
//                                  NOTEE5, NOTED5, NOTEC5, NOTEC5, RestNote, NOTEC5, NOTEB4, NOTEA4, NOTEA4, RestNote,
//                                  NOTEA4, NOTEC5, NOTEE5, NOTED5, NOTEB4, NOTEG4, NOTEF5, NOTEA5, NOTEF5, NOTEE5, NOTEC5, NOTEE5,
//                                  NOTEE4, NOTEE5, NOTEB4, NOTEC5, NOTEE5, NOTEA4, NOTEE4, NOTEE5, NOTEGsAb4, NOTEA4, NOTEE5, NOTEC5,
//                                  NOTEG4, NOTEG5, NOTED5, NOTEE5, NOTEG5, NOTEC5, NOTEA4, RestNote, // 3
//                                  NOTEE4, NOTEA4, NOTEE4, NOTEA3, NOTEAsBb3, //4
//                                  NOTEC4, NOTEE4, NOTEA4, NOTEA4, NOTEA4, NOTEG4, NOTEF4, NOTEE4, NOTEE4, RestNote,
//                                  NOTED4, NOTEC4, NOTEB3, NOTEC4, NOTEB3, NOTEA3, RestNote,
//                                  NOTEB3, NOTEC4, NOTED4, NOTEE4, NOTEF4, NOTEG4, NOTEA4, NOTEB4, NOTEC5, NOTED5, NOTEE5, // Big scale
//                                  NOTEF5, NOTEG5, NOTEA5, NOTEB5, NOTEC6, RestNote, RestNote, // 5
//                                  NOTEC5, NOTEF5, NOTEA5, NOTEC6, NOTEAsBb5, NOTEG5, NOTEE5, NOTEF5, RestNote,
//                                  NOTEC5, NOTEF5, NOTEA5, NOTEC6, NOTEAsBb5, NOTEG5, NOTEE5, NOTED5, RestNote,
//                                  NOTEA4, NOTED5, NOTEF5, NOTEA5, NOTEG5, NOTEE5, NOTECsDb5, NOTED5, RestNote,
//                                  NOTEA4, NOTED5, NOTEF5, NOTEA5, NOTEG5, NOTEE5, NOTEC5, NOTEAsBb4, RestNote, // 6/7 (splicing them together)
//                                  NOTED4, NOTEE4, NOTEF4, NOTEF4, NOTEG4, NOTEA4, NOTEC5, NOTEA4, NOTEC5, NOTEA4, NOTEC5, NOTEA4,
//                                  NOTEC5, NOTEF5, NOTEA5, NOTEC6, RestNote,
//                                  NOTEC6, NOTEAsBb5, NOTEA5, NOTEA5, RestNote, // Get ready for the end
//                                  NOTED6, NOTEC6, NOTEAsBb5, NOTEA5, NOTEG5, NOTEF5, NOTEE5, NOTED5, NOTEC5, NOTEAsBb4,
//                                  NOTEA4, NOTEG4, NOTEF4, RestNote, NOTEF5, RestNote, // Emulate a Grand Pause
//                                  NULL};
const uint16_t CarnivalNotes[] = {RestNote, NOTEE5, NOTEDsEb5, NOTEFsGb4, RestNote, NOTEE5, NOTEFsGb5, NOTEFsGb5, NOTEFsGb4, NOTEFsGb4, RestNote,
                             NOTEGsAb4, NOTEGsAb4, RestNote, NOTECsDb5, NOTECsDb5, RestNote, //Star
                             NOTEFsGb4, NOTEB4, RestNote, NOTEB4, RestNote, NOTEGsAb4, NOTEB4, NOTEB4, RestNote, NOTEFsGb4, NOTEB4, NOTEA4, NOTEGsAb4, NOTEE4,
                             RestNote, //Repeat1
                             NOTEFsGb4, NOTEB4, RestNote, NOTEB4, RestNote, NOTEGsAb4, NOTEB4, NOTEB4, RestNote, NOTEFsGb4, NOTEB4, NOTEA4, NOTEGsAb4, NOTEE4,
                             NOTEE5, NOTEDsEb5, NOTEFsGb4, RestNote, NOTEE5, NOTEFsGb5, NOTEFsGb5, NOTEFsGb4, NOTEFsGb4, RestNote,
                             NOTEGsAb4, NOTEGsAb4, RestNote, NOTECsDb5, NOTECsDb5, // beginning repeats
                             RestNote, NOTEFsGb4, NOTEB4, RestNote, NOTEB4, RestNote, NOTEE5, NOTEFsGb5, NOTEB5, NOTEFsGb5, NOTEF5, //Star
                             NOTEE5, NOTEDsEb5, NOTEB4, NOTEB4, NOTEFsGb4, NOTEB4, RestNote, NOTEFsGb4, RestNote, NOTEFsGb4,
                             NOTEB4, NOTEB4, NOTEF4, RestNote, NOTEE5, NOTEFsGb5, NOTEB5, NOTEFsGb5, NOTEF5,
                             NOTEB4, NOTEB4, NOTEFsGb4, NOTEB4, RestNote, NOTEFsGb4, RestNote, NOTEFsGb4,
                             NOTEB4, NOTEB4, NOTEF4, RestNote, NOTEE5, NOTEFsGb5, NOTEB5, NOTEFsGb5, NOTEF5,
                             NOTEB4, NOTEB4, NOTEFsGb4, NOTEB4, RestNote, NOTEFsGb4, RestNote, NOTEFsGb4,
                             NOTEB4, NOTEB4, NOTEF4, RestNote, NOTEE5, NOTEFsGb5, NOTEB5, NOTEFsGb5, NOTEF5, NULL};
const uint16_t CarnivalBeats[] = {8, 2, 2, 8, 8, 8, 8, 8, 8, 8,
                                  8, 4, 8, 8, 4, 8, 8, 8, 8, 8,
                                  4, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, //repeat
                                  8, 8, 8, 8, 4, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, //end repeat
                                  2, 2, 4, 8, 8, 8, 8, 8, 8, 8, 4, 8, 8, 4, 8, // beginning repeats
                                  8, 8, 8, 8, 4, 8, 16, 16, 8, 8, 8,
                                  2, 2, 8, 8, 8, 8, 8, 8, 8, 8,
                                  8, 8, 8, 8, 16, 16, 8, 8, 8,
                                  8, 8, 8, 8, 8, 8, 8, 8, // Repeat 1
                                  8, 8, 8, 8, 16, 16, 8, 8, 8,
                                  8, 8, 8, 8, 8, 8, 8, 8, // Repeat 2
                                  8, 8, 8, 8, 16, 16, 8, 8, 8, NULL};

//const uint16_t PlayfulPBeats[] = {4, 16, 16, 16, 16, 16, 16, 16, 4, 8, 4, 16, 16, 16, 16, 16, 16, 16, 4, 8, 4,
//                                  16, 16, 16, 16, 16, 16, 16, 4, 8, 4, 16, 16, 16, 16, 16, 16, 16, 4, 8, 4,
//                                  16, 16, 16, 16, 16, 16, 16, 4, 8, 4, 16, 16, 16, 16, 16, 16, 16, 4, 16,
//                                  16, 16, 16, 4, 16, 16, 16, 16, 4, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 4, 8, 4,
//                                  16, 16, 16, 16, 16, 16, 16, 16, 16, 4, 16,
//                                  16, 16, 16, 4, 16, 16, 16, 16, 4, 16,
//                                  16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16 ,16, 16 ,16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16 ,16, 16, 16, 16,
//                                  16, 4, 8,
//                                  16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 4, 8,
//                                  16, 16, 16, 16, 16, 1, 2,
//                                  16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 4, 8, 4,
//                                  16, 16, 16, 16, 16, 16, 16, 4, 8, 16, 16, 16, 16, 16, 16, 16, 4, 8,
//                                  16, 16, 16, 16, 16, 16, 16, 4, 8, 16, 16, 16, 16, 16, 16, 16, 4, 8,
//                                  16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 4, 8,
//                                  16, 16, 16, 4, 8,
//                                  16, 16 ,16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 2, 16, 1, 1,
//                                  NULL};

//===================================
//Configure Speaker as Timer A0 CCR1 output pin
//Configure Timer A0 to produce sound on CCR1 output compare

void PlayerConfiguraion(void) {

    SpeakerPort->DIR |= Speaker;            // set P2.4 as output
    SpeakerPort->SEL0 |= Speaker;           // P2.4 set to TA0.1
    SpeakerPort->SEL1 &= ~Speaker;  // Option 0b01
//set Timer A0 CCR0 compare for period and CCR1 compare for duty cycle of a note
    TIMER_A0->CCR[0] = NOTEA4 - 1; //unsure if needed
    TIMER_A0->CCR[1] = (NOTEA4 / 2) - 1;
    TIMER_A0->CCTL[1] = 0b0000000001100000;
    TIMER_A0->CTL = 0b0000001010010100;

}

//================================
//Play a note by sending its Timer A0 cycle count to this subroutine
//This could be insert in the interrupt service
void PlayNote (unsigned int CurrentNote) {
    TIMER_A0->CCR[0] = CurrentNote - 1;
     // Set high pulse-width in CCR1 register (determines duty cycle)
    TIMER_A0->CCR[1] = (CurrentNote / 2) - 1;
}

//================================

//Exercise 4-3: similar to Exercis 4-3 but uses Timer A1 Up mode and CCR0
//Option 1: Configure Timer A Up-Mode and CCR0 compare interrupt
//ACLK at 32 kHz. prescaler 1:1
//to generate note duration: 0.25 second, 0.5 second, 1 second, etc.
//by setting the time duration in  TIMER_A1->CCR[0]
//configure LED1 to toggle each time CCR0 compare interrupt occurs
//Option 2: uses Exercise 4-3 approach

#define ACLK 32768 //Hz
#define FULL_NOTE       ACLK    //1 second
#define HALF_NOTE   ACLK/2  //0.5 second
#define QUARTER_NOTE    ACLK/4  //0.25 second
#define EIGHTH_NOTE  ACLK/8
#define SIXTEENTH_NOTE ACLK/16
#define THIRTY_SECONDTH_NOTE ACLK/32
#define COMPLEX QUARTER_NOTE + EIGHTH_NOTE
#define DELAY100MS  ACLK/10    //100ms timer delay

void NoteDurationConfiguration() {

    LED1port->DIR |= LED1;                    // Set P1.0 as output for LED1
    LED1port->OUT &= ~LED1;
    /* Configure Timer_A1 and CCRs */
     // Set initial period in CCR0 register. This assumes timer starts at 0
 //    TIMER_A1->CCR[0] = QUARTER_NOTE;
     TIMER_A2->CCR[0] = HALF_NOTE;
     // Configure CCR0 for Compare mode with interrupt enabled (no output mode - 0)
     // TODO configure CCR0
     TIMER_A2->CCTL[0]=0x0010;
     // Configure Timer_A1 in UP Mode with source ACLK prescale 1:1 and no interrupt
     // TODO configure Timer_A1: ACLK, UP mode, TACLR=1 bit 2, no interrupt
     TIMER_A2->CTL=0b0000000100010100;  //0x0114

     /* Configure global interrupts and NVIC */
     // Enable TA1 TA1CCR0 compare interrupt
     // TODO enable interrupt by setting IRQ bit in NVIC ISER0 register

     NVIC->ISER[0] |= (1)<<TA2_0_IRQn;
}

//================================
//Play the note list once to test the notes
#define NoteDuration 1000000
void  PlayAllNotesOnce(const uint16_t notelist[]){
    static char noteindex;
    unsigned int delay;
    noteindex=0;
    while(notelist[noteindex]!=NULL) {
        PlayNote(notelist[noteindex]);
    noteindex=noteindex+1;
    for(delay = NoteDuration; delay != 0; delay--) {
     }
    } //end while()
}//end PlayAllNotesOnce(void)

    //TODO complete start switch configuration function
//switch port and masks
#define SwitchPort P1   //Port 1
#define Switch1   0b00000010    //P1.1
#define Switch2   0b00010000    //P1.4
//#define Switch2   0b00100000    //P1.5

void StartSwitchConfiguration(void) {
    SwitchPort->SEL0 &= ~Switch2;
    SwitchPort->SEL1 &= ~Switch2;
    SwitchPort->DIR |= Switch2;
    SwitchPort->OUT |= Switch2;
    SwitchPort->REN |= Switch2;
}

SwitchState CheckStopSwitch(void){
    //check Switch S1 and return Pressed or NotPressed
    if((SwitchPort->IN & Switch2) != Switch2){
        return Pressed;
    }
    return NotPressed;
}

#define ddelay 5500
void debounce(void){
    unsigned int delay;
    for(delay = 0; delay <= ddelay ; delay++);
}


int main(void)
{
    PlayerStatus Player=STOP;
    SwitchState StopSwitch=NotPressed;

    /* Stop Watchdog timer */
     WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;

    //TODO Sets MCLK and SMCLK to 48MHz HFXT
    configHFXT();


    //TODO Configure LFXT to use 32kHz crystal, source to ACLK
    configLFXT();

    PlayerConfiguraion();
//test to see if the player works before switching on interrupt\
//This is the idea of divide and conquer
    PlayAllNotesOnce(NotesSequence);

    StartSwitchConfiguration();

    Player = STOP;   //place holder for Feature 3 and 3.
    StopSwitch=CheckStopSwitch();
    while(Player == STOP){
        StopSwitch=CheckStopSwitch();
        if(StopSwitch == Pressed){
            break;
        }
    }

    NoteDurationConfiguration();    //start time duration interrupt

    __enable_irq();                 // Enable global interrupt

    while (1) {
        //TODO If StopSwitch is pressed, switch Player between START and PAUSE
        StopSwitch=CheckStopSwitch();
        if(StopSwitch==Pressed) {
            if(Player==START) { //PAUSE the song within 1ms after switch pressed, wait for button release
            //stop Timer A1 and Timer A0
                TIMER_A0->CTL &= ~0b0000000000110000;
                TIMER_A2->CTL &= ~0b0000000000110000;
/*
* Here is one way to stop both timers:
* TIMER_A0->CTL&=~0b0000000000110000;  //bits 5-4=0b00
*  TIMER_A1->CTL&=~0b0000000000110000;  //bits 5-4=0b00
*/
            //switch Speaker to input
            //debounce()
                SpeakerPort->DIR &= ~Speaker;
                debounce();
                while((SwitchPort->IN & Switch2) != Switch2){
                        }
                debounce();
            //wait for button to release and debounce()
                Player = PAUSE;
            }
            else  {  //wait for switch to be released and START the song within 10ms after switch released
            //debounce()
            //wait for button replease
            //debounce()
            //start Timer A1 and timer A0
                debounce();
                while((SwitchPort->IN & Switch2) != Switch2){
                        }
                debounce();
                TIMER_A0->CTL |=0b0000000000010000;
                TIMER_A2->CTL |=0b0000000000010000;

/*
* Here is one way to start both timers:
* TIMER_A0->CTL |=0b0000000000010000;  //bits 5-4=0b01
* TIMER_A1->CTL |=0b0000000000010000;  //bits 5-4=0b01
*/
            //switch Speaker to output
                SpeakerPort->DIR |= Speaker;
                Player = START;
            }
        }  //if (StopSwtch==Pressed)
    }  //while (1)

    //while(1);   //hold the program here
} // end main()

/* Timer_A1 CCR0 interrupt service routine, i.e., the handler */
//Timer A1 Up CCR0 compare interrupt to generate note duration
//by setting up TIMER_A1->CCR[0].
//The same compare interrupt can be used to generate 100ms rest note
//by turning off Speaker for 100ms with a compare interrupt
//Toggle LED1 to show note durations
//This handler name TimerA1CCR0ServiceRoute is not correct
    //TODO Find the right handler name for Timer A1 CCR0 interrupt
void TA2_0_IRQHandler (void)
{
    static char nextnote=0; //start with first note in a song
    static char insert_rest=1;  //true on start
    //TODO
     /* Check if interrupt triggered by CCR0 */
    //clear interrupt flag
    //TODO toggle LED1
    LED1port->OUT ^= LED1;
    //TODO serve CCR0 interrupt
     if(TIMER_A2->CCTL[0] & TIMER_A_CCTLN_CCIFG)
     {
         TIMER_A2->R = 0; // Clear Timer A1 count
         if (insert_rest==1) {
        //TODO set CCR[0] interrupt to generate a 100ms rest
        //turn off speaker;  SpeakerPort->DIR &= ~Speaker; to set P2.4 as input
             insert_rest=0;
             TIMER_A2->CCR[0] = DELAY100MS;
             SpeakerPort->DIR &= ~Speaker;
     }
     else { //play next note
         //TODO set next note duration interrupt
         //play the current note
         //get next note into  TIMER_A0_CCR[0] and  TIMER_A0_CCR[1]
         insert_rest=1; //insert 100ms rest after the note is played.
         SpeakerPort->DIR |= Speaker;
         if(CarnivalNotes[nextnote] != NULL){
             nextnote=nextnote+1;
         }
         else {
             nextnote = 0;
         }
         PlayNote(CarnivalNotes[nextnote]);
         switch(CarnivalBeats[nextnote]){
             case 1: TIMER_A2->CCR[0] = FULL_NOTE;
                 break;
             case 2: TIMER_A2->CCR[0] = HALF_NOTE;
                 break;
             case 4: TIMER_A2->CCR[0] = QUARTER_NOTE;
                 break;
             case 8: TIMER_A2->CCR[0] = EIGHTH_NOTE;
                 break;
             case 16: TIMER_A2->CCR[0] = SIXTEENTH_NOTE;
                 break;
             case 9: TIMER_A2->CCR[0] = COMPLEX;
                 break;
             case 32: TIMER_A2->CCR[0] = THIRTY_SECONDTH_NOTE;
                 break;
             default: TIMER_A2->CCR[0] = QUARTER_NOTE;
                 break;
         }
     }
     TIMER_A2->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG; // Clear flag
     }
}
//end interrupt service
