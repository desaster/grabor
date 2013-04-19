/* Copyright (C) 2013 Upi Tamminen
 * See the COPYRIGHT file for more information */

#ifndef __CPU_H
#define __CPU_H

#include <stdint.h>

void flash_latency(uint32_t frequency);
void pll0_feed(void);
uint32_t pll0_start(uint32_t crystal, uint32_t frequency);
void system_init(void);

#endif
