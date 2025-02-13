/*! \file */
/*!
 * LED_indicator.h
 *
 * Description: RGB driver for LED2 on MSP432P4111 Launchpad
 *
 *  Created on: 2/10/2025
 *      Author: Cruz Martinez
 */

#ifndef LED_INDICATOR_H_
#define LED_INDICATOR_H_

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

#include "msp.h"

typedef enum  _CurrentLED {LED1, LED2, LED3, LED4} CurrentLED;

// making use of the RGB port and other LED ports in this project
#define LED_PORT1               P1
#define LED_PORT2               P2
#define LED1_PIN                0b00000001 // P1.0
#define LED2_PIN                0b00000001 // P2.0
#define LED3_PIN                0b00000010 // P2.1
#define LED4_PIN                0b00000100 // P2.2

#define LED_ALL_PINS            0b00000111 // redefine later...maybe

/*!
 * \brief This function configures the LED pins as output pins
 *
 * This function configures P1.0, P2.0, P2.1, and P2.2 as output pins
 *  for the four LEDs used, and initializes them as 'off'
 *
 * \return None
 */
extern void LED_init(void);


/*!
 * \brief This function toggles LED1
 *
 * This function toggles the LED on P1.0
 *
 * \return None
 */
extern void LED1_toggle(void);


/*!
 * \brief This function toggles LED2
 *
 * This function toggles the LED on P2.0
 *
 * \return None
 */
extern void LED2_toggle(void);


/*!
 * \brief This function toggles LED3
 *
 * This function toggles the LED on P2.1
 *
 * \return None
 */
extern void LED3_toggle(void);


/*!
 * \brief This function toggles LED4
 *
 * This function toggles the LED on P2.2
 *
 * \return None
 */
extern void LED4_toggle(void);

/*!
 * \brief This function toggles all LEDs
 *
 * This function toggles each LED and turns them off.
 *
 * \return None
 */
extern void LED_OFF(void);


//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif /* LED_indicator_H_ */
