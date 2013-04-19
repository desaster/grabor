/* Copyright (C) 2013 Upi Tamminen
 * See the COPYRIGHT file for more information */

#ifndef __UTILS_H 
#define __UTILS_H

__INLINE void delay_ms(uint32_t dlyTicks);
uint16_t hex2int(char *a, uint16_t len);
void split_args(char *buf, char *p1, char *p2);

#endif

/* vim: set sw=4 et: */
