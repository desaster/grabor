/** \file hdr_special_registers.h
 * \brief Header with definition of bits in special registers in ARMv7-M(E) core
 * \author Freddie Chopin, http://www.freddiechopin.info/
 * \date 2012-03-18
 */

/******************************************************************************
* chip: ARMv7-M(E) (Cortex-M3 / Cortex-M4)
* compiler: arm-none-eabi-gcc (GNU Tools for ARM Embedded Processors) 4.6.2
* 	20110921 (release) [ARM/embedded-4_6-branch revision 182083]
******************************************************************************/

#ifndef HDR_SPECIAL_REGISTERS_H_
#define HDR_SPECIAL_REGISTERS_H_

/*
+-----------------------------------------------------------------------------+
| PRIMASK - Register to mask out configurable exceptions
+-----------------------------------------------------------------------------+
*/

#define PRIMASK_MASK_EXCEPTIONS_bit			0

#define PRIMASK_MASK_EXCEPTIONS				(1 << PRIMASK_MASK_EXCEPTIONS_bit)

/*
+-----------------------------------------------------------------------------+
| FAULTMASK - Register to raise priority to the HardFault level
+-----------------------------------------------------------------------------+
*/

#define FAULTMASK_MASK_EXCEPTIONS_bit		0

#define FAULTMASK_MASK_EXCEPTIONS			(1 << FAULTMASK_MASK_EXCEPTIONS_bit)

/*
+-----------------------------------------------------------------------------+
| CONTROL - The special-purpose control register
+-----------------------------------------------------------------------------+
*/

#define CONTROL_THREAD_UNPRIVILEGED_bit		0
#define CONTROL_ALTERNATE_STACK_bit			1

#define CONTROL_THREAD_UNPRIVILEGED			(1 << CONTROL_THREAD_UNPRIVILEGED_bit)
#define CONTROL_ALTERNATE_STACK				(1 << CONTROL_ALTERNATE_STACK_bit)

/******************************************************************************
* END OF FILE
******************************************************************************/
#endif /* HDR_SPECIAL_REGISTERS_H_ */
