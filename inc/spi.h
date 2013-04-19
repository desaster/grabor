/* Copyright (C) 2013 Upi Tamminen
 * See the COPYRIGHT file for more information */

#ifndef __SPI_H 
#define __SPI_H

void spi_init(void);
uint16_t spi_write(uint16_t data);
uint16_t spi_read(uint16_t data);

#endif

/* vim: set sw=4 et: */
