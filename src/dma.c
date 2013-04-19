/* Copyright (C) 2013 Upi Tamminen
 * See the COPYRIGHT file for more information
 *
 * This code is not used at the moment, but keep it around anyway */

#include <stdint.h>
#include "inc/LPC17xx.h"
#include "config.h"
#include "hdr/hdr_sc.h"

#include "tprintf.h"
#include "spi.h"

void dma_init(uint8_t *buf, uint16_t len)
{
    LPC_SC->PCONP |= (1 << 29); // power up gpdma

    LPC_PINCON->PINSEL4 &= ~0x7fff;     // P2.0->P2.7 GPIO
    LPC_GPIO2->FIODIR &= ~0xff;         // P2.0->P2.7 input

    LPC_GPDMACH0->DMACCConfig = 0; // stop ch0 dma

    LPC_GPDMA->DMACConfig |= 1 << 0; // (table 557, pg 599)
    LPC_GPDMA->DMACSync &= ~(1 << 10); // use MAT1.0 for Sync

    // Timer1 Match Compare 0 as DMA request (table 558, pg 599)
    LPC_SC->DMAREQSEL |= 1 << 2;

    LPC_GPDMA->DMACIntErrClr |= 0xff; // (table 549, pg 596)
    LPC_GPDMA->DMACIntTCClear |= 0xff; // (table 547, pg 595);

    //LPC_GPDMACH0->DMACCDestAddr = (uint32_t) &dstdata[0];
    LPC_GPDMACH0->DMACCDestAddr = (uint32_t) &buf[0];
    //LPC_GPDMACH0->DMACCSrcAddr = (uint32_t) &srcdata[0];
    LPC_GPDMACH0->DMACCSrcAddr = (uint32_t) &(LPC_GPIO2->FIOPIN);

    // we are not using the linked list (table 562, pg 602)
    LPC_GPDMACH0->DMACCLLI = 0;

    LPC_GPDMACH0->DMACCControl = (len & 0xfff)
        | (0 << 12)     // source burst size (12 - 14) = 1
        | (0 << 15)     // destination burst size (15 - 17) = 1
        | (0 << 18)     // source width (18 - 20) = 8 bit
        | (0 << 21)     // destination width (21 - 23) = 8 bit
        | (0 << 24)     // source AHB select (24) = AHB 0
        | (0 << 25)     // destination AHB select (25) = AHB 0
        | (0 << 26)     // source increment (26) = no increment
        | (1 << 27)     // destination increment (27) = increment
        | (0 << 28)     // mode select (28) = access in user mode
        | (0 << 29)     // (29) = access not bufferable
        | (0 << 30)     // (30) = access not cacheable
        | (1 << 31);    // terminal count interrupt enabled

    // 10 corresponds to MAT1.0 and it is selected as the source request
    // peripheral (table 543, pg 592)
    LPC_GPDMACH0->DMACCConfig = 1 // enable ch0
        | (10 << 1)     // source peripheral (1 - 5) = MAT1.0
        | (0 << 6)      // destination request peripheral (6 - 10) = none
        | (2 << 11)     // flow control (11 - 13) = per to mem
        | (0 << 14)     // (14) = mask out error interrupt
        | (1 << 15)     // (15) = mask out terminal count interrupt
        | (0 << 16)     // (16) = no locked transfers
        | (0 << 18);    // (27) = no HALT
}

void dma_start(uint8_t *buf, uint16_t len)
{
    LPC_GPIO1->FIODIR &= ~(1 << 18); // 1.18 = input
    LPC_PINCON->PINSEL3 |= (3 << 4); // 1.18 = CAP1.0

    // setup timer1
    LPC_SC->PCONP |= 1 << 2; // Power up
    LPC_SC->PCLKSEL0 |= 0x01 << 4; // CCLK

    LPC_TIM1->TCR = 0x00; // stop the timer

    LPC_TIM1->MR0 = 1;
    LPC_TIM1->MCR = 1 << 1; // reset on MR0

    LPC_TIM1->IR |= 0xff; // Clear all timer interrupts if there are any

    LPC_TIM1->TCR |=
          (1 << 0)  // counter enable
        | (1 << 1); // counter reset
    /* Counter Mode: TC is incremented on rising edges on the CAP
     * input selected by bits 3:2. */
    LPC_TIM1->CTCR |= (1 << 0);
    LPC_TIM1->CTCR &= ~(3 << 2); // CAP1.0 for TIMER1
    LPC_TIM1->CCR = 0; // capture on the rising edges on CAP1.0

    dma_init(buf, len);

    LPC_TIM1->TCR = 0x01; // start timer.
}

void dma_test_data(uint8_t *buf)
{
    uint16_t d = 0;
    uint16_t s = 0;
    uint16_t fail = 0;
    uint16_t success = 0;
    uint32_t i;

    for (i = 0; i < 2048; i ++) {
        if (buf[i] != '\xaa') {
            fail ++;
        } else {
            success ++;
        }
    }

    //tprintf("d [%d] s [%d] fail [%d]\r\n", d, s, fail);
    tprintf("success [%d] fail [%d]\r\n", success, fail);
}

void dma_test(uint8_t *buf)
{
    uint16_t i;

    for (i = 0; i < 2048; i ++) buf[i] = 'd';
    
    dma_start(buf, 2048);
    //spi_write(CMD_FOO);
    while (!(LPC_GPDMA->DMACIntTCStat & 0x01));

    dma_test_data(buf);
}

void dma_transfer(uint8_t *buf)
{
    dma_start(buf, 2048);
    //spi_write(CMD_FOO);
    while (!(LPC_GPDMA->DMACIntTCStat & 0x01));
}

void dma_showdebug(void)
{
    tprintf("LPC_TIM1->TC [%d]\r\n", LPC_TIM1->TC);
    tprintf("LPC_GPDMACH0->DMACIntTCStat [%d]\r\n",
        (LPC_GPDMA->DMACIntTCStat & 0x01));
}

/* vim: set sw=4 et: */
