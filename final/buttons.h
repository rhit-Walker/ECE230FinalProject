/*
 * buttons.h
 *
 * Description: Button Header file for the various buttons used in the final project
 *
 *  Created on: Feb 13, 2025
 *      Author: martincw
 *
 */

#ifndef BUTTONS_H_
#define BUTTONS_H_

#define BUTTON_PORT P1            //Port 1
#define BUTTON_1_PIN 0b00100000   //Pin 1.5
//TODO #define more buttons

typedef enum _SwitchState {NotPressed, Pressed} SwitchState; // maybe


/*
 * Configures and initializes all switches (buttons) to act as inputs
 *
 * \return None
 */
extern void InitializeSwitches(void);


/*
 * Checks the state of switch1 (assumes the player only presses one switch at a time)
 *
 * \return SwitchState
 */
extern SwitchState CheckSwitch1(void);


/*
 * Delay inserted after button presses to ensure the state of the button.
 *
 * \return None
 */
extern void Debounce(void);



#endif /* BUTTONS_H_ */
