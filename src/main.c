/* Copyright (C) 2013 Upi Tamminen
 * See the COPYRIGHT file for more information
 *
 * Based on the sample lpc1769 project by Freddie Chopin
 * http://www.freddiechopin.info/
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "inc/LPC17xx.h"
#include "config.h"
#include "hdr/hdr_sc.h"
#include "gpio.h"
#include "cpu.h"
#include "tprintf.h"
#include "spi.h"
#include "uart0.h"
#include "led.h"
#include "tvp7002.h"
#include "utils.h"
#include "cpld.h"

#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/pbuf.h"
#include "lwip/udp.h"
#include "lwip/tcp.h"
#include "lwip/tcp_impl.h"
#include "lwip/stats.h"
#include "netif/etharp.h"

#include "enc28j60.h"
#include "enc28j60if.h"

volatile uint32_t msTicks;
volatile uint32_t ledTicks;
volatile uint32_t arpTicks;
volatile uint32_t tcpFastTicks;
volatile uint32_t tcpSlowTicks;
volatile uint32_t statsTicks;

/* It's called "dmabuf", but we are not actually using dma at the moment */
uint8_t dmabuf[2048];

uint8_t enc_int;

void EINT3_IRQHandler(void)
{
    if ((LPC_GPIOINT->IO0IntStatF & (1 << 27)) != 0) {
        LPC_GPIOINT->IO0IntClr = (1 << 27);
        enc_int = 1;
    }
}

void SysTick_Handler(void)
{
    msTicks++;
    tcpFastTicks++;
    tcpSlowTicks++;
    arpTicks++;
    ledTicks++;
    statsTicks++;
}

void board_init(void)
{
    /* Setup SysTick Timer for 1 msec interrupts  */
    if (SysTick_Config(FREQUENCY / 1000)) {
        while (1); /* Capture error */
    }

    /* clkout of 10mhz on 1.27 */
    LPC_PINCON->PINSEL3 &=~(3<<22);
    LPC_PINCON->PINSEL3 |= (1<<22);
    LPC_SC->CLKOUTCFG = (1<<8) | (11<<4); //enable and divide by 12

    /* tvp7002_reset is at 0.10 */
    LPC_PINCON->PINSEL0 &= ~(3<<20);
    LPC_GPIO0->FIODIR |= (1<<10);
    LPC_GPIO0->FIOSET |= (1<<10);

    /* tvp7002_pwdn is at 0.11 */
    LPC_PINCON->PINSEL0 &= ~(3<<22);
    LPC_GPIO0->FIODIR |= (1<<11);
    LPC_GPIO0->FIOSET |= (1<<11);

    /* SAVE is at 0.16 */
    LPC_PINCON->PINSEL1 &= (3 << 0);
    LPC_GPIO0->FIODIR |= (1<<16);
    LPC_GPIO0->FIOSET &= (1<<16);

    /* XFR is at 0.15 */
    LPC_PINCON->PINSEL0 &= (3 << 30);
    LPC_GPIO0->FIODIR |= (1<<15);
    LPC_GPIO0->FIOSET &= (1<<15);

    /* OUTCLK is at 0.17 */
    LPC_PINCON->PINSEL1 &= (3 << 2);
    LPC_GPIO0->FIODIR |= (1<<17);
    LPC_GPIO0->FIOSET &= (1<<17);

    /* pixel data input from CPLD */
    LPC_PINCON->PINSEL4 &= ~0x7fff;     // P2.0->P2.7 GPIO
    LPC_GPIO2->FIODIR &= ~0xff;         // P2.0->P2.7 input

    /* enc28j60 reset is at 0.7 */
    LPC_PINCON->PINSEL0 &= ~(3<<14);
    LPC_GPIO0->FIODIR |= (1<<7);

    /* enc28j60 interrupt at 0.27 */
    enc_int = 0;
    LPC_PINCON->PINSEL1 &= (3<<22);
    LPC_GPIO0->FIODIR &= ~(1<<27);
    LPC_GPIOINT->IO0IntEnR &= (1 << 27); // rising edge no
    LPC_GPIOINT->IO0IntEnF |= (1 << 27); // falling edge yes
    NVIC_SetPriority(EINT3_IRQn, 0);
    NVIC_EnableIRQ(EINT3_IRQn);

    /* enc28j60 manual SSEL at 0.28 */
    LPC_PINCON->PINSEL1 &= (3<<24);
    LPC_GPIO0->FIODIR |= (1<<28);
    LPC_GPIO0->FIOSET |= (1<<28);

    spi_init();

    led_init();

    UART0_Init(115200);

    /* I2C */
    if (I2CInit((uint32_t) I2CMASTER) == 0) {
        tprintf("Fatal error!\n");
        while (1);
    }
}

void tvp7002_start(void)
{
    tprintf("Resetting TVP7002...");
    LPC_GPIO0->FIOCLR |= (1<<11);
    LPC_GPIO1->FIOCLR |= (1<<10);
    delay_ms(50);
    LPC_GPIO1->FIOSET |= (1<<10);
    tprintf(" Done.\r\n");
    delay_ms(100);
    tprintf("Initializing TVP7002...");
    tvp7002_init();
    tprintf(" Done.\r\n");
}

int main(void)
{
    arpTicks = 
        tcpFastTicks = 
        tcpSlowTicks = 
        ledTicks = 
        statsTicks = 0;

    system_init();
    pll0_start(CRYSTAL, FREQUENCY);
    board_init();

    tprintf("Reset!\r\n");
    delay_ms(500);

    tvp7002_start();

    tprintf("Initializing ethernet...");
    ethernet_init();
    tprintf(" Done.\r\n");

    while (1) {
        /*
        if (enc_int) {
            enc_int = 0;
            ethernet_action();
            led_toggle();
        }
        */
        ethernet_action();

        if (arpTicks >= ETHARP_TMR_INTERVAL) {
            arpTicks = 0;
            etharp_tmr();
        }
        if (tcpFastTicks >= TCP_FAST_INTERVAL) {
            tcpFastTicks = 0;
            tcp_fasttmr();
        }
        if (tcpSlowTicks >= TCP_SLOW_INTERVAL) {
            tcpFastTicks = 0;
            tcp_slowtmr();
        }
        if (ledTicks >= 1000) {
            ledTicks = 0;
            led_toggle();
        }
        if (statsTicks >= 5000) {
            statsTicks = 0;
            //stats_display();
            enc_test();
        }
    }
}

/* vim: set sw=4 et: */
