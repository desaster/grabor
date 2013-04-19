/* Copyright (C) 2013 Upi Tamminen
 * See the COPYRIGHT file for more information */

#include "inc/LPC17xx.h"
#include "tprintf.h"
#include "utils.h"

extern uint8_t dmabuf[2048];

/* instruct the cpld to capture an image from tvp7002 to sram */
void cpld_capture(void)
{
    tprintf("Saving...");
    LPC_GPIO0->FIOSET |= (1<<16);
    delay_ms(100);
    LPC_GPIO0->FIOCLR |= (1<<16);
    delay_ms(100);
    tprintf(" done.\r\n");
}

/* transfer some data from sram via cpld */
void cpld_xfr(void)
{
    uint16_t i;

    LPC_GPIO0->FIOSET |= (1<<15);
    for (i = 0; i < 2048; i ++) {
        while (!(LPC_GPIO0->FIOPIN & (1<<17)));
        dmabuf[i] = (uint8_t) LPC_GPIO2->FIOPIN;
        while ((LPC_GPIO0->FIOPIN & (1<<17)));
    }
    LPC_GPIO0->FIOCLR |= (1<<15);
}

/* vim: set et sw=4: */
