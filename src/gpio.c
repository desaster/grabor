/** \file gpio.c
 * \brief GPIO driver.
 * \details Function for configuring a GPIO pin
 * \author Freddie Chopin, http://www.freddiechopin.info/
 * \date 2012-04-07
 */

/******************************************************************************
* chip: LPC175x / LPC176x
* compiler: arm-none-eabi-gcc (GNU Tools for ARM Embedded Processors) 4.6.2
* 	20110921 (release) [ARM/embedded-4_6-branch revision 182083]
*
* prefix: gpio_
*
* available global functions:
* 	void gpio_pin_cfg(LPC_GPIO_TypeDef *gpio_ptr, uint32_t pin, uint32_t configuration)
*
* available local functions:
*
* available interrupt handlers:
******************************************************************************/

/*
+=============================================================================+
| includes
+=============================================================================+
*/

#include <stdint.h>

#include "inc/LPC17xx.h"

#include "gpio.h"

/*
+=============================================================================+
| local definitions
+=============================================================================+
*/

#define GPIO_GET_SEL(combination)			(((combination) >> 0) & 0xFF)
#define GPIO_GET_MODE(combination)			(((combination) >> 8) & 0xFF)
#define GPIO_GET_MODE_OD(combination)		(((combination) >> 16) & 0xFF)
#define GPIO_GET_DIR(combination)			(((combination) >> 24) & 0xFF)

/*
+=============================================================================+
| module variables
+=============================================================================+
*/

/*
+=============================================================================+
| local functions' declarations
+=============================================================================+
*/

/*
+=============================================================================+
| global functions
+=============================================================================+
*/

/*------------------------------------------------------------------------*//**
* \brief Configures pin.
* \details Configures one pin in one port.
*
* \param [in] gpio_ptr points to the configuration structure of desired GPIO
* port
* \param [in] pin selects one pin, [0,31]
* \param [in] configuration is a combined value of PINSEL, PINMODE, PINMODE_OD,
* and FIODIR register bitfields, allowed values:
* {GPIO_IN_{PULL_UP, REPEATER, FLOATING, PULL_DOWN},
* GPIO_OUT_{PP, OD}, GPIO_OUT_OD_{PULL_UP, REPEATER, PULL_DOWN},
* GPIO_{ALT1, ALT2, ALT3}_{PP, OD}_{PULL_UP, REPEATER, FLOATING, PULL_DOWN}}
*//*-------------------------------------------------------------------------*/

void gpio_pin_cfg(LPC_GPIO_TypeDef *gpio_ptr, uint32_t pin, uint32_t configuration)
{
	uint32_t gpio, sel, mode, mode_od, dir;
	volatile uint32_t *pinsel, *pinmode, *pinmode_od;

	// calculate the number of GPIO that should be configured
	gpio = (((uint32_t)gpio_ptr) - LPC_GPIO_BASE) / (LPC_GPIO1_BASE - LPC_GPIO0_BASE);

	// get address of appropriate PINSEL register
	pinsel = &(&LPC_PINCON->PINSEL0)[gpio * 2 + pin / 16];

	sel = *pinsel;							// localize
	sel &=~ (LPC_PINCON_PINSELx_mask << (pin * 2));	// clear current setting
	sel |= (GPIO_GET_SEL(configuration) << (pin * 2));	// apply new setting
	*pinsel = sel;							// save back

	// get address of appropriate PINMODE register
	pinmode = &(&LPC_PINCON->PINMODE0)[gpio * 2 + pin / 16];

	mode = *pinmode;						// localize
	mode &=~ (LPC_PINCON_PINMODEx_mask << (pin * 2));	// clear current setting
	mode |= (GPIO_GET_MODE(configuration) << (pin * 2));	// apply new setting
	*pinmode = mode;						// save back

	// get address of appropriate PINMODE_OD register
	pinmode_od = &(&LPC_PINCON->PINMODE_OD0)[gpio];

	mode_od = *pinmode_od;					// localize
	mode_od &=~ (LPC_PINCON_PINMODE_ODx_mask << pin);	// clear current setting
	mode_od |= (GPIO_GET_MODE_OD(configuration) << pin);	// apply new setting
	*pinmode_od = mode_od;					// save back

	dir = gpio_ptr->FIODIR;					// localize
	dir &=~ (LPC_GPIO_FIODIR_mask << pin);	// clear current setting
	dir |= (GPIO_GET_DIR(configuration) << pin);	// apply new setting
	gpio_ptr->FIODIR = dir;					// save back
}

/*
+=============================================================================+
| local functions
+=============================================================================+
*/

/*
+=============================================================================+
| ISRs
+=============================================================================+
*/

/******************************************************************************
* END OF FILE
******************************************************************************/
