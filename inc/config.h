/** \file config.h
 * \brief Basic configuration of the project
 * \author Freddie Chopin, http://www.freddiechopin.info/
 * \date 2012-04-06
 */

/******************************************************************************
* project: lpc1769_blink_led
* chip: LPC1769
* compiler: arm-none-eabi-gcc (GNU Tools for ARM Embedded Processors) 4.6.2
* 	20110921 (release) [ARM/embedded-4_6-branch revision 182083]
******************************************************************************/

#ifndef CONFIG_H_
#define CONFIG_H_

#include "hdr/hdr_bitband.h"

/*
+=============================================================================+
| global definitions
+=============================================================================+
*/

#define CRYSTAL								12000000ul	///< quartz crystal resonator which is connected to the chip
#define FREQUENCY							100000000ul	///< desired target frequency of the core

#define LED_GPIO							LPC_GPIO0	///< GPIO port to which the LED is connected
#define LED_pin								22		///< pin number of the LED

#define LED									(1 << LED_pin)
#define LED_FIOPIN							LED_GPIO->FIOPIN	///< output register for the LED
#define LED_bb								bitband_t BITBAND(&LED_FIOPIN, LED_pin)	///< bit-band "variable" to directly handle the pin

/*
+=============================================================================+
| strange variables
+=============================================================================+
*/

/*
+=============================================================================+
| global variables
+=============================================================================+
*/

/*
+=============================================================================+
| global functions' declarations
+=============================================================================+
*/

/******************************************************************************
* END OF FILE
******************************************************************************/
#endif /* CONFIG_H_ */
