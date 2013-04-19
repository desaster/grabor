/** \file vectors.c
 * \brief LPC175x / LPC176x vector table and __Default_Handler()
 * \details LPC175x / LPC176x vector table and __Default_Handler()
 * \author Freddie Chopin, http://www.freddiechopin.info/
 * \date 2012-03-24
 */

/******************************************************************************
* chip: LPC175x / LPC176x
* compiler: arm-none-eabi-gcc (GNU Tools for ARM Embedded Processors) 4.6.2
* 	20110921 (release) [ARM/embedded-4_6-branch revision 182083]
******************************************************************************/

/*------------------------------------------------------------------------*//**
* \brief Default interrupt handler.
* \details Default interrupt handler, used for interrupts that don't have their
* own handler defined.
*//*-------------------------------------------------------------------------*/

static void __Default_Handler(void) __attribute__ ((interrupt));
static void __Default_Handler(void)
{
	while (1);
}

/*
+=============================================================================+
| assign all unhandled interrupts to the default handler
+=============================================================================+
*/

// Non Maskable Interrupt
void NMI_Handler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// All class of fault
void HardFault_Handler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// Cortex-M3 Memory Management Interrupt
void MemManage_Handler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// Cortex-M3 Bus Fault Interrupt
void BusFault_Handler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// Cortex-M3 Usage Fault Interrupt
void UsageFault_Handler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// Reserved 0x1C
void __Reserved_0x1C_Handler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// Reserved 0x20
void __Reserved_0x20_Handler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// Reserved 0x24
void __Reserved_0x24_Handler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// Reserved 0x28
void __Reserved_0x28_Handler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// Cortex-M3 SV Call Interrupt
void SVCall_Handler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// Cortex-M3 Debug Monitor Interrupt
void DebugMon_Handler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// Reserved 0x34
void __Reserved_0x34_Handler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// Cortex-M3 Pend SV Interrupt
void PendSV_Handler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// Cortex-M3 System Tick Interrupt
void SysTick_Handler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// Watchdog Timer Interrupt
void WDT_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// Timer0 Interrupt
void TIMER0_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// Timer1 Interrupt
void TIMER1_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// Timer2 Interrupt
void TIMER2_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// Timer3 Interrupt
void TIMER3_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// UART0 Interrupt
void UART0_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// UART1 Interrupt
void UART1_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// UART2 Interrupt
void UART2_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// UART3 Interrupt
void UART3_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// PWM1 Interrupt
void PWM1_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// I2C0 Interrupt
void I2C0_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// I2C1 Interrupt
void I2C1_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// I2C2 Interrupt
void I2C2_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// SPI Interrupt
void SPI_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// SSP0 Interrupt
void SSP0_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// SSP1 Interrupt
void SSP1_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// PLL0 Lock (Main PLL) Interrupt
void PLL0_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// Real Time Clock Interrupt
void RTC_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// External Interrupt 0 Interrupt
void EINT0_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// External Interrupt 1 Interrupt
void EINT1_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// External Interrupt 2 Interrupt
void EINT2_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// External Interrupt 3 Interrupt
void EINT3_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// A/D Converter Interrupt
void ADC_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// Brown-Out Detect Interrupt
void BOD_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// USB Interrupt
void USB_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// CAN Interrupt
void CAN_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// General Purpose DMA Interrupt
void DMA_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// I2S Interrupt
void I2S_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// Ethernet Interrupt
void ENET_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// Repetitive Interrupt Timer Interrupt
void RIT_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// Motor Control PWM Interrupt
void MCPWM_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// Quadrature Encoder Interface Interrupt
void QEI_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// PLL1 Lock (USB PLL) Interrupt
void PLL1_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// USB Activity interrupt
void USBActivity_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// CAN Activity interrupt
void CANActivity_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

/*
+=============================================================================+
| Vector table
+=============================================================================+
*/

extern const char __main_stack_end;			// imported main stack end (from linker script)

void Reset_Handler(void);					// import the address of Reset_Handler()

void (* const vectors[])(void) __attribute__ ((section(".vectors"))) = {
		(void (*)(void))&__main_stack_end,	// Main stack end address
		Reset_Handler,						// Reset
		NMI_Handler,						// Non Maskable Interrupt
		HardFault_Handler,					// All class of fault
		MemManage_Handler,					// Cortex-M3 Memory Management Interrupt
		BusFault_Handler,					// Cortex-M3 Bus Fault Interrupt
		UsageFault_Handler,					// Cortex-M3 Usage Fault Interrupt
		__Reserved_0x1C_Handler,			// Reserved 0x1C
		__Reserved_0x20_Handler,			// Reserved 0x20
		__Reserved_0x24_Handler,			// Reserved 0x24
		__Reserved_0x28_Handler,			// Reserved 0x28
		SVCall_Handler,						// Cortex-M3 SV Call Interrupt
		DebugMon_Handler,					// Cortex-M3 Debug Monitor Interrupt
		__Reserved_0x34_Handler,			// Reserved 0x34
		PendSV_Handler,						// Cortex-M3 Pend SV Interrupt
		SysTick_Handler,					// Cortex-M3 System Tick Interrupt
		WDT_IRQHandler,						// Watchdog Timer Interrupt
		TIMER0_IRQHandler,					// Timer0 Interrupt
		TIMER1_IRQHandler,					// Timer1 Interrupt
		TIMER2_IRQHandler,					// Timer2 Interrupt
		TIMER3_IRQHandler,					// Timer3 Interrupt
		UART0_IRQHandler,					// UART0 Interrupt
		UART1_IRQHandler,					// UART1 Interrupt
		UART2_IRQHandler,					// UART2 Interrupt
		UART3_IRQHandler,					// UART3 Interrupt
		PWM1_IRQHandler,					// PWM1 Interrupt
		I2C0_IRQHandler,					// I2C0 Interrupt
		I2C1_IRQHandler,					// I2C1 Interrupt
		I2C2_IRQHandler,					// I2C2 Interrupt
		SPI_IRQHandler,						// SPI Interrupt
		SSP0_IRQHandler,					// SSP0 Interrupt
		SSP1_IRQHandler,					// SSP1 Interrupt
		PLL0_IRQHandler,					// PLL0 Lock (Main PLL) Interrupt
		RTC_IRQHandler,						// Real Time Clock Interrupt
		EINT0_IRQHandler,					// External Interrupt 0 Interrupt
		EINT1_IRQHandler,					// External Interrupt 1 Interrupt
		EINT2_IRQHandler,					// External Interrupt 2 Interrupt
		EINT3_IRQHandler,					// External Interrupt 3 Interrupt
		ADC_IRQHandler,						// A/D Converter Interrupt
		BOD_IRQHandler,						// Brown-Out Detect Interrupt
		USB_IRQHandler,						// USB Interrupt
		CAN_IRQHandler,						// CAN Interrupt
		DMA_IRQHandler,						// General Purpose DMA Interrupt
		I2S_IRQHandler,						// I2S Interrupt
		ENET_IRQHandler,					// Ethernet Interrupt
		RIT_IRQHandler,						// Repetitive Interrupt Timer Interrupt
		MCPWM_IRQHandler,					// Motor Control PWM Interrupt
		QEI_IRQHandler,						// Quadrature Encoder Interface Interrupt
		PLL1_IRQHandler,					// PLL1 Lock (USB PLL) Interrupt
		USBActivity_IRQHandler,				// USB Activity interrupt
		CANActivity_IRQHandler, 			// CAN Activity interrupt
};

/******************************************************************************
* END OF FILE
******************************************************************************/
