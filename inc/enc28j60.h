/* Copyright (C) 2013 Upi Tamminen
 * See the COPYRIGHT file for more information */

#ifndef __ENC28J60_H 
#define __ENC28J60_H

#include <stdint.h>

void enc_init(const uint8_t *mac);
uint8_t enc_test(void);
uint8_t enc_action(void);
void enc_send_packet(const uint8_t *buf, uint16_t count);
void enc_send_packet_begin(uint16_t count);
void enc_send_packet_end(uint16_t count);
uint16_t enc_receive_packet_begin(void);
void enc_receive_packet(uint8_t *buf, uint16_t len);
void enc_receive_packet_end(void);
void enc_get_mac_addr(uint8_t *mac_addr);
uint8_t enc_revid(void);

#endif

/* vim: set sw=4 et: */
