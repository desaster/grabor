/* Copyright (C) 2013 Upi Tamminen
 * See the COPYRIGHT file for more information */

#include <stdint.h>
#include "inc/LPC17xx.h"
#include "config.h"
#include "hdr/hdr_sc.h"

inline void led_init(void)
{
    /* LED2 is at 0.22 */
    LPC_PINCON->PINSEL0 &= ~(3<<18);
    LPC_GPIO0->FIODIR |= (1<<9);
}

inline void led_on(void)
{
    LPC_GPIO0->FIOSET |= (1<<9);
}

inline void led_off(void)
{
    LPC_GPIO0->FIOCLR |= (1<<9);
}

inline void led_toggle(void)
{
    LPC_GPIO0->FIOPIN ^= (1<<9);
}

/* vim: set sw=4 et: */
