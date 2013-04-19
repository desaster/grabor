/* Copyright (C) 2013 Upi Tamminen
 * See the COPYRIGHT file for more information */

#ifndef __TVP7002_H 
#define __TVP7002_H

#include "i2c.h"

#define TVP7002_ADDR     0xB8

uint32_t tvp7002_set(uint8_t addr, uint8_t val);
uint8_t tvp7002_get(uint8_t addr);
void tvp7002_init(void);
void tvp7002_info(void);

#endif

/* vim: set et sw=4: */
