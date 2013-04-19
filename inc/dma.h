/* Copyright (C) 2013 Upi Tamminen
 * See the COPYRIGHT file for more information */

#ifndef __DMA_H 
#define __DMA_H

void dma_init(uint8_t *buf, uint16_t len);
void dma_start(uint8_t *buf, uint16_t len);
void dma_transfer(uint8_t *buf);
void dma_test(uint8_t *buf);
void dma_showdebug(void);

#endif

/* vim: set sw=4 et: */
