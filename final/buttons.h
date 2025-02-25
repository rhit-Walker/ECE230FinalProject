
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

#define BUTTON_PORT P2            //Port 1

#define BUTTON_1_PIN  BIT3   // P2.3
#define BUTTON_2_PIN  BIT5   // P2.5
#define BUTTON_3_PIN  BIT6   // P2.6
#define BUTTON_4_PIN  BIT7   // P2.7

//#define RESETBUTTON_PIN  BIT3   // P2.3
//TODO #define more buttons

typedef enum _SwitchState {NotPressed, Pressed} SwitchState; // maybe


/*
 * Configures and initializes all switches (buttons) to act as inputs
 *
 * \return None
 */
extern void InitializeSwitches(void);


extern void InitializeResetSwitch(void);

/*
 * Checks the state of a given switch
 * \param switchNum The switch number (1 to 4)
 * \return SwitchState Pressed/NotPressed
 */
extern SwitchState CheckSwitch(int switchNum);



/*
 * Delay inserted after button presses to ensure the state of the button.
 *
 * \return None
 */
extern void Debounce(void);

/*
 * Delay inserted after resetting.
 *
 * \return None
 */
extern void Debounce_Long(void);



#endif /* BUTTONS_H_ */
