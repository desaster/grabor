/* Copyright (C) 2013 Upi Tamminen
 * See the COPYRIGHT file for more information */

#include <stdint.h>
#include "inc/LPC17xx.h"
#include "config.h"
#include "hdr/hdr_sc.h"

void spi_init(void)
{
    /* Enable AHB clock to the SSP0. */
    LPC_SC->PCONP |= (0x1<<21);

    /* Further divider is needed on SSP0 clock. Using default divided by 4 */
    LPC_SC->PCLKSEL1 &= ~(0x3<<10);
    LPC_SC->PCLKSEL1 |= (0x1<<10); // by 0

    /* SCK0 1.20 */
    LPC_PINCON->PINSEL3 |= (3<<8);
    /* MISO0 1.23, MOSI0 1.24 */
    LPC_PINCON->PINSEL3 |= ((3<<14) | (3<<16));

    /* SSEL0 1.21 */
    LPC_PINCON->PINSEL3 |= (3<<10);

    /*
    LPC_PINCON->PINMODE1 &= (3<<8); // pull-up 1.20
    LPC_PINCON->PINMODE1 &= (3<<10); // pull-up 1.21
    LPC_PINCON->PINMODE1 &= (3<<14); // pull-up 1.23
    LPC_PINCON->PINMODE1 &= (3<<16); // pull-up 1.24
    */
    LPC_GPIO1->FIODIR &= (1<<23);

    /* Set DSS data to 8-bit,
       Frame format SPI, CPOL = 0, CPHA = 0, and SCR is 15  */
    LPC_SSP0->CR0 = 0x0207;
    //LPC_SSP0->CR0 = 0x07;  // SSP max speed, 8 bits
    LPC_SSP0->CR1 = 0x02;

    /* SSPCPSR clock prescale register,
       master mode, minimum divisor is 0x02 */
    //LPC_SSP0->CPSR = 0x12;
    //LPC_SSP0->CPSR = 0x6; // 5.5mhz
    LPC_SSP0->CPSR = 0x5; // 8.33mhz
}

uint8_t spi_write(uint8_t data)
{
    LPC_SSP0->DR = data;
    while ((LPC_SSP0->SR & 0x10));
    return LPC_SSP0->DR;
}

uint8_t spi_read(uint8_t data)
{
    spi_write(data);
    return spi_write(0x00);
}

/* vim: set sw=4 et: */
