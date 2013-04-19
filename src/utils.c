/* Copyright (C) 2013 Upi Tamminen
 * See the COPYRIGHT file for more information */

#include <stdint.h>
#include <string.h>
#include "inc/LPC17xx.h"
#include "config.h"
#include "hdr/hdr_sc.h"
#include "tprintf.h"

extern volatile uint32_t msTicks;

__INLINE void delay_ms(uint32_t dlyTicks)
{
    uint32_t curTicks;

    curTicks = msTicks;
    while ((msTicks - curTicks) < dlyTicks);
}

uint16_t hex2int(char *a, uint16_t len)
{
    uint8_t i;
    uint16_t val = 0;

    for(i=0;i<len;i++)
       if(a[i] <= 57)
        val += (a[i]-48)*(1<<(4*(len-1-i)));
       else
        val += (a[i]-55)*(1<<(4*(len-1-i)));
    return val;
}

void split_args(char *buf, char *p1, char *p2)
{
    uint8_t n = 0, c, p1c = 0, p2c = 0;
    for (c = 0; c < strlen(buf); c ++) {
        if (buf[c] == ' ' && !n) {
            n = 1;
            continue;
        }
        if (n == 0) {
            p1[p1c++] = buf[c];
        } else {
            p2[p2c++] = buf[c];
        }
    }
    p1[p1c] = 0;
    p2[p2c] = 0;
}

/* vim: set et sw=4: */
