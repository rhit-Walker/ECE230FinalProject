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


#ifdef __cplusplus
extern "C"
{
#endif

#include "msp.h"

typedef enum  _CurrentLED {LED1, LED2, LED3, LED4} CurrentLED;

// making use of the RGB port and other LED ports in this project
#define LED_PORT               P4
#define LED1_PIN BIT4                   // P4.4
#define LED2_PIN BIT5                   // P4.5
#define LED3_PIN BIT6                   // P4.6
#define LED4_PIN BIT7                   // P4.7


#define LED_ALL_PINS            0b11110000 // redefine later...maybe


extern void LED1_ON(void);
extern void LED1_OFF(void);
extern void LED2_ON(void);
extern void LED2_OFF(void);
extern void LED3_ON(void);
extern void LED3_OFF(void);
extern void LED4_ON(void);
extern void LED4_OFF(void);

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
extern void LEDS_OFF(void);


//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif /* LED_indicator_H_ */
