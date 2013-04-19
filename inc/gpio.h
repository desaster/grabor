/** \file gpio.h
 * \brief Header for gpio.c
 * \details Header for gpio.c
 * \author Freddie Chopin, http://www.freddiechopin.info/
 * \date 2012-04-07
 */

/******************************************************************************
* chip: LPC175x / LPC176x
* compiler: arm-none-eabi-gcc (GNU Tools for ARM Embedded Processors) 4.6.2
* 	20110921 (release) [ARM/embedded-4_6-branch revision 182083]
******************************************************************************/

#ifndef GPIO_H_
#define GPIO_H_

#include "inc/LPC17xx.h"

#include "hdr/hdr_pincon.h"
#include "hdr/hdr_gpio.h"

/*
+=============================================================================+
| global definitions
+=============================================================================+
*/

#define GPIO_COMBINE(sel, mode, mode_od, dir)	((sel) | ((mode) << 8) | ((mode_od) << 16) | ((dir) << 24))

// all possible configurations of GPIO

// input configurations

#define GPIO_IN_PULL_UP					GPIO_COMBINE(LPC_PINCON_PINSELx_DEFAULT_value, LPC_PINCON_PINMODEx_PULL_UP_value, 0, LPC_GPIO_FIODIR_IN_value)
#define GPIO_IN_REPEATER				GPIO_COMBINE(LPC_PINCON_PINSELx_DEFAULT_value, LPC_PINCON_PINMODEx_REPEATER_value, 0, LPC_GPIO_FIODIR_IN_value)
#define GPIO_IN_FLOATING				GPIO_COMBINE(LPC_PINCON_PINSELx_DEFAULT_value, LPC_PINCON_PINMODEx_FLOATING_value, 0, LPC_GPIO_FIODIR_IN_value)
#define GPIO_IN_PULL_DOWN				GPIO_COMBINE(LPC_PINCON_PINSELx_DEFAULT_value, LPC_PINCON_PINMODEx_PULL_DOWN_value, 0, LPC_GPIO_FIODIR_IN_value)

// output configurations

#define GPIO_OUT_PP						GPIO_COMBINE(LPC_PINCON_PINSELx_DEFAULT_value, 0, LPC_PINCON_PINMODE_ODx_PP_value, LPC_GPIO_FIODIR_OUT_value)
#define GPIO_OUT_OD						GPIO_COMBINE(LPC_PINCON_PINSELx_DEFAULT_value, LPC_PINCON_PINMODEx_FLOATING_value, LPC_PINCON_PINMODE_ODx_OD_value, LPC_GPIO_FIODIR_OUT_value)
#define GPIO_OUT_OD_PULL_UP				GPIO_COMBINE(LPC_PINCON_PINSELx_DEFAULT_value, LPC_PINCON_PINMODEx_PULL_UP_value, LPC_PINCON_PINMODE_ODx_OD_value, LPC_GPIO_FIODIR_OUT_value)
#define GPIO_OUT_OD_REPEATER			GPIO_COMBINE(LPC_PINCON_PINSELx_DEFAULT_value, LPC_PINCON_PINMODEx_REPEATER_value, LPC_PINCON_PINMODE_ODx_OD_value, LPC_GPIO_FIODIR_OUT_value)
#define GPIO_OUT_OD_PULL_DOWN			GPIO_COMBINE(LPC_PINCON_PINSELx_DEFAULT_value, LPC_PINCON_PINMODEx_PULL_DOWN_value, LPC_PINCON_PINMODE_ODx_OD_value, LPC_GPIO_FIODIR_OUT_value)

// alternate function configurations

#define GPIO_ALT1_PP_PULL_UP			GPIO_COMBINE(LPC_PINCON_PINSELx_ALT1_value, LPC_PINCON_PINMODEx_PULL_UP_value, LPC_PINCON_PINMODE_ODx_PP_value, 0)
#define GPIO_ALT1_PP_REPEATER			GPIO_COMBINE(LPC_PINCON_PINSELx_ALT1_value, LPC_PINCON_PINMODEx_REPEATER_value, LPC_PINCON_PINMODE_ODx_PP_value, 0)
#define GPIO_ALT1_PP_FLOATING			GPIO_COMBINE(LPC_PINCON_PINSELx_ALT1_value, LPC_PINCON_PINMODEx_FLOATING_value, LPC_PINCON_PINMODE_ODx_PP_value, 0)
#define GPIO_ALT1_PP_PULL_DOWN			GPIO_COMBINE(LPC_PINCON_PINSELx_ALT1_value, LPC_PINCON_PINMODEx_PULL_DOWN_value, LPC_PINCON_PINMODE_ODx_PP_value, 0)
#define GPIO_ALT1_OD_PULL_UP			GPIO_COMBINE(LPC_PINCON_PINSELx_ALT1_value, LPC_PINCON_PINMODEx_PULL_UP_value, LPC_PINCON_PINMODE_ODx_OD_value, 0)
#define GPIO_ALT1_OD_REPEATER			GPIO_COMBINE(LPC_PINCON_PINSELx_ALT1_value, LPC_PINCON_PINMODEx_REPEATER_value, LPC_PINCON_PINMODE_ODx_OD_value, 0)
#define GPIO_ALT1_OD_FLOATING			GPIO_COMBINE(LPC_PINCON_PINSELx_ALT1_value, LPC_PINCON_PINMODEx_FLOATING_value, LPC_PINCON_PINMODE_ODx_OD_value, 0)
#define GPIO_ALT1_OD_PULL_DOWN			GPIO_COMBINE(LPC_PINCON_PINSELx_ALT1_value, LPC_PINCON_PINMODEx_PULL_DOWN_value, LPC_PINCON_PINMODE_ODx_OD_value, 0)

#define GPIO_ALT2_PP_PULL_UP			GPIO_COMBINE(LPC_PINCON_PINSELx_ALT2_value, LPC_PINCON_PINMODEx_PULL_UP_value, LPC_PINCON_PINMODE_ODx_PP_value, 0)
#define GPIO_ALT2_PP_REPEATER			GPIO_COMBINE(LPC_PINCON_PINSELx_ALT2_value, LPC_PINCON_PINMODEx_REPEATER_value, LPC_PINCON_PINMODE_ODx_PP_value, 0)
#define GPIO_ALT2_PP_FLOATING			GPIO_COMBINE(LPC_PINCON_PINSELx_ALT2_value, LPC_PINCON_PINMODEx_FLOATING_value, LPC_PINCON_PINMODE_ODx_PP_value, 0)
#define GPIO_ALT2_PP_PULL_DOWN			GPIO_COMBINE(LPC_PINCON_PINSELx_ALT2_value, LPC_PINCON_PINMODEx_PULL_DOWN_value, LPC_PINCON_PINMODE_ODx_PP_value, 0)
#define GPIO_ALT2_OD_PULL_UP			GPIO_COMBINE(LPC_PINCON_PINSELx_ALT2_value, LPC_PINCON_PINMODEx_PULL_UP_value, LPC_PINCON_PINMODE_ODx_OD_value, 0)
#define GPIO_ALT2_OD_REPEATER			GPIO_COMBINE(LPC_PINCON_PINSELx_ALT2_value, LPC_PINCON_PINMODEx_REPEATER_value, LPC_PINCON_PINMODE_ODx_OD_value, 0)
#define GPIO_ALT2_OD_FLOATING			GPIO_COMBINE(LPC_PINCON_PINSELx_ALT2_value, LPC_PINCON_PINMODEx_FLOATING_value, LPC_PINCON_PINMODE_ODx_OD_value, 0)
#define GPIO_ALT2_OD_PULL_DOWN			GPIO_COMBINE(LPC_PINCON_PINSELx_ALT2_value, LPC_PINCON_PINMODEx_PULL_DOWN_value, LPC_PINCON_PINMODE_ODx_OD_value, 0)

#define GPIO_ALT3_PP_PULL_UP			GPIO_COMBINE(LPC_PINCON_PINSELx_ALT3_value, LPC_PINCON_PINMODEx_PULL_UP_value, LPC_PINCON_PINMODE_ODx_PP_value, 0)
#define GPIO_ALT3_PP_REPEATER			GPIO_COMBINE(LPC_PINCON_PINSELx_ALT3_value, LPC_PINCON_PINMODEx_REPEATER_value, LPC_PINCON_PINMODE_ODx_PP_value, 0)
#define GPIO_ALT3_PP_FLOATING			GPIO_COMBINE(LPC_PINCON_PINSELx_ALT3_value, LPC_PINCON_PINMODEx_FLOATING_value, LPC_PINCON_PINMODE_ODx_PP_value, 0)
#define GPIO_ALT3_PP_PULL_DOWN			GPIO_COMBINE(LPC_PINCON_PINSELx_ALT3_value, LPC_PINCON_PINMODEx_PULL_DOWN_value, LPC_PINCON_PINMODE_ODx_PP_value, 0)
#define GPIO_ALT3_OD_PULL_UP			GPIO_COMBINE(LPC_PINCON_PINSELx_ALT3_value, LPC_PINCON_PINMODEx_PULL_UP_value, LPC_PINCON_PINMODE_ODx_OD_value, 0)
#define GPIO_ALT3_OD_REPEATER			GPIO_COMBINE(LPC_PINCON_PINSELx_ALT3_value, LPC_PINCON_PINMODEx_REPEATER_value, LPC_PINCON_PINMODE_ODx_OD_value, 0)
#define GPIO_ALT3_OD_FLOATING			GPIO_COMBINE(LPC_PINCON_PINSELx_ALT3_value, LPC_PINCON_PINMODEx_FLOATING_value, LPC_PINCON_PINMODE_ODx_OD_value, 0)
#define GPIO_ALT3_OD_PULL_DOWN			GPIO_COMBINE(LPC_PINCON_PINSELx_ALT3_value, LPC_PINCON_PINMODEx_PULL_DOWN_value, LPC_PINCON_PINMODE_ODx_OD_value, 0)

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

void gpio_pin_cfg(LPC_GPIO_TypeDef *gpio_ptr, uint32_t pin, uint32_t configuration);

/******************************************************************************
* END OF FILE
******************************************************************************/
#endif /* GPIO_H_ */
