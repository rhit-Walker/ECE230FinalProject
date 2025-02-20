/*
 * speaker.h
 *
 *  Created on: Feb 10, 2025
 *      Author: martincw
 */

#ifndef SPEAKER_H_
#define SPEAKER_H_

#define SpeakerPort P2
#define Speaker BIT4     // P2.4
#define LED1port P1
#define LED1 BIT0

typedef enum  {STOP, START, WAIT, PAUSE} PlayerStatus;

#define SMCLK 48000000     //Hz
#define TimerA0Prescaler 4 //Timer A prescaler
#define TimerA0Clock  SMCLK/TimerA0Prescaler

#define RestNote 1

#define FrequencyC4 261.626
#define FrequencyE4 329.628
#define FrequencyF4 349.228
#define FrequencyFsGb4 369.994
#define FrequencyGsAb4 415.305
#define FrequencyA4 440
#define FrequencyB4 493.88
#define FrequencyCsDb5 554.365
#define FrequencyDsEb5 622.254
#define FrequencyE5 659.255
#define FrequencyF5 698.456
#define FrequencyFsGb5 739.989
#define FrequencyG5 783.991
#define FrequencyGsAb5 830.609
#define FrequencyB5 987.767

#define NOTEC4  TimerA0Clock/FrequencyC4
#define NOTEE4  TimerA0Clock/FrequencyE4
#define NOTEF4  TimerA0Clock/FrequencyF4
#define NOTEFsGb4  TimerA0Clock/FrequencyFsGb4
#define NOTEGsAb4  TimerA0Clock/FrequencyGsAb4
#define NOTEA4  TimerA0Clock/FrequencyA4
#define NOTEB4  TimerA0Clock/FrequencyB4
#define NOTECsDb5  TimerA0Clock/FrequencyCsDb5
#define NOTEDsEb5  TimerA0Clock/FrequencyDsEb5
#define NOTEE5  TimerA0Clock/FrequencyE5
#define NOTEF5  TimerA0Clock/FrequencyF5
#define NOTEFsGb5  TimerA0Clock/FrequencyFsGb5
#define NOTEG5  TimerA0Clock/FrequencyG5
#define NOTEGsAb5  TimerA0Clock/FrequencyGsAb5
#define NOTEB5  TimerA0Clock/FrequencyB5


#define ACLK 32768 //Hz
#define FULL_NOTE       ACLK    //1 second
#define HALF_NOTE   ACLK/2  //0.5 second
#define QUARTER_NOTE    ACLK/4  //0.25 second
#define EIGHTH_NOTE  ACLK/8
#define SIXTEENTH_NOTE ACLK/16
#define DELAY100MS  ACLK/10

#define NoteDuration 1000000


/*
 * Configure speaker as the Timer A0 and CCR1 output pin, configured to produce
 * sound on CCR1 output compare.
 *
 * \return None
 */
extern void PlayerConfiguraion(void);


/*
 * Play a note, send the Timer A0 cycle count to play the next note within an
 * array of Notes
 *
 * \return None
 */
extern void PlayNote(unsigned int CurrentNote);


/*
 * Configure Timer A1 and CCR0 to generate the note duration.
 * Configure the interrupts for the Speaker.
 *
 * \return None
 */
extern void NoteDurationConfiguration(void);


/*
 * Play a small sequence of Notes to signal that configuration has finished,
 * and that the system is now awaiting user input to start.
 *
 *
 * \return None
 */
extern void SignalConfigured(void);

/*
 * Interrupt handler used to play the next note within the Note with a specific beat
 * from the Note and Beat arrays. Generates a 100ms rest between notes similar to
 * project 3, as the piece we are using is intended to be played 'staccato'.
 *
 * \return None
 */
void TA2_0_IRQHandler(void);

#endif /* SPEAKER_H_ */
