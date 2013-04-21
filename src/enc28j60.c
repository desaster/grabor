/* Copyright (C) 2013 Upi Tamminen
 * See the COPYRIGHT file for more information */

#include <stdint.h>
#include "inc/LPC17xx.h"
#include "config.h"
#include "hdr/hdr_sc.h"
#include "utils.h"
#include "tprintf.h"
#include "spi.h"
#include "enc28j60.h"
#include "enc28j60reg.h"

#define TX_START        (0x1FFF - 0x600)
#define RX_END          (TX_START-1)

static uint8_t enc_current_bank;
static uint16_t enc_next_packet;

uint8_t enc_read(uint8_t data);
void enc_switch_bank(uint8_t new_bank);
void enc_wcr(uint8_t reg, uint8_t val);
void enc_reset(void);
uint8_t enc_read_reg(uint8_t reg, uint8_t bank);

/* Macros for accessing registers.
 * These macros should be used instead of calling the functions directly.
 * They simply pass the register's bank as an argument, so the caller
 * doesn't have to deal with that.
 */
#define READ_REG(reg) enc_read_reg(reg, reg ## _BANK)
#define WRITE_REG(reg, value) enc_write_reg(reg, reg ## _BANK, value)
#define READ_MREG(reg) enc_read_mreg(reg, reg ## _BANK)
#define SET_REG_BITS(reg, mask) enc_set_bits(reg, reg ## _BANK, mask)
#define CLEAR_REG_BITS(reg, mask) enc_clear_bits(reg, reg ## _BANK, mask)

__INLINE void wait(void)
{
    volatile uint32_t i;
    /* tested at 100mhz using argument of 1 */
    for (i = 0; i < 98; i ++);
}

__INLINE void delay_ssel(void)
{
    volatile int i;
    for (i = 0; i < 150; i ++); // if in doubt, increase me
}

__INLINE void ssel_enable(void)
{
    //delay_ssel();
    LPC_GPIO0->FIOCLR |= (1<<28);
    //delay_ssel();
}

__INLINE void ssel_disable(void)
{
    delay_ssel();
    LPC_GPIO0->FIOSET |= (1<<28);
    //delay_ssel();
}

uint8_t enc_write(uint8_t data)
{
    uint8_t val;
    ssel_enable();
    val = spi_write(data);
    ssel_disable();
    return val;
}

void enc_reset(void)
{
    LPC_GPIO0->FIOCLR |= (1<<7);
    delay_ms(10);
    LPC_GPIO0->FIOSET |= (1<<7);
    delay_ms(10);
    enc_write(0xff);
    delay_ms(10);
}

/**
 * Read Control Register (RCR)
 */
uint8_t enc_rcr(uint8_t reg)
{
    uint8_t val;

    ssel_enable();
    spi_write(reg);
    val = spi_write(0xFF);
    ssel_disable();

    return val;
}

/*
 * Write control register
 */
void enc_wcr(uint8_t reg, uint8_t val)
{
    ssel_enable();
    spi_write(0x40 | reg);
    spi_write(val);
    ssel_disable();
}

/**
 * Read Control Register for MAC an MII registers.
 * Reading MAC and MII registers produces an initial dummy
 * byte. Presumably because it takes longer to fetch the values
 * of those registers.
 */
uint8_t enc_rcr_m(uint8_t reg)
{
    uint8_t val;

    ssel_enable();
    spi_write(reg);
    spi_write(0xFF);
    val = spi_write(0xFF); // Dummy
    ssel_disable();

    return val;
}

/**
 * Read Buffer Memory.
 */
void enc_rbm(uint8_t *buf, uint16_t count)
{
    int i;

    ssel_enable();
    spi_write(0x20 | 0x1A);
    for (i = 0; i < count; i++) {
        *buf = spi_write(0xFF);
        buf++;
    }
    ssel_disable();
}

/**
 * Write Buffer Memory.
 */
void enc_wbm(const uint8_t *buf, uint16_t count)
{
    int i;

    ssel_enable();
    spi_write(0x60 | 0x1A);
    for (i = 0; i < count; i++) {
        spi_write(*buf);
        buf++;
    }
    ssel_disable();
}

/**
 * Bit Field Set.
 * Set the bits of argument 'mask' in the register 'reg'.
 * Not valid for MAC and MII registers.
 */
void enc_bfs(uint8_t reg, uint8_t mask)
{
    ssel_enable();
    spi_write(0x80 | reg);
    spi_write(mask);
    ssel_disable();
}

/**
 * Bit Field Clear.
 * Clear the bits of argument 'mask' in the register 'reg'.
 * Not valid for MAC and MII registers.
 */
void enc_bfc(uint8_t reg, uint8_t mask)
{
    ssel_enable();
    spi_write(0xA0 | reg);
    spi_write(mask);
    ssel_disable();
}

void enc_switch_bank(uint8_t new_bank)
{
    uint8_t econ1;

    if (new_bank == enc_current_bank || new_bank == ANY_BANK) {
        return;
    }

    econ1 = enc_rcr(ENC_ECON1);
    econ1 &= ~ENC_ECON1_BSEL_MASK;
    econ1 |= (new_bank & ENC_ECON1_BSEL_MASK) << ENC_ECON1_BSEL_SHIFT;
    enc_wcr(ENC_ECON1, econ1);
    enc_current_bank = new_bank;
}

/**
 * High level register read. Switches bank as appropriate.
 */
uint8_t enc_read_reg(uint8_t reg, uint8_t bank)
{
    if (bank != enc_current_bank) {
        enc_switch_bank(bank);
    }

    return enc_rcr(reg);
}

/**
 * High level bit field set. Switches bank as appropriate.
 */
void enc_set_bits(uint8_t reg, uint8_t bank, uint8_t mask)
{
    if (bank != enc_current_bank) {
        enc_switch_bank(bank);
    }

    enc_bfs(reg, mask);
}

/**
 * High level bit field clear. Switches bank as appropriate.
 */
void enc_clear_bits(uint8_t reg, uint8_t bank, uint8_t mask)
{
    if (bank != enc_current_bank) {
        enc_switch_bank(bank);
    }

    enc_bfc(reg, mask);
}

/**
 * High level MAC/MII register read. Switches bank as appropriate.
 */
uint8_t enc_read_mreg(uint8_t reg, uint8_t bank)
{
    if (bank != enc_current_bank) {
        enc_switch_bank(bank);
    }

    return enc_rcr_m(reg);
}

/**
 * High level register write. Switches bank as appropriate.
 */
void enc_write_reg(uint8_t reg, uint8_t bank, uint8_t value)
{
    if (bank != enc_current_bank) {
        enc_switch_bank(bank);
    }

    enc_wcr(reg, value);
}

/**
 * Read value from PHY address.
 * Reading procedure is described in ENC28J60 datasheet
 * section 3.3.
 */
uint16_t enc_phy_read(uint8_t addr)
{
    uint8_t stat;
    uint8_t ret;
    /*
     1. Write the address of the PHY register to read
     from into the MIREGADR register.*/
    WRITE_REG(ENC_MIREGADR, addr);

    /*2. Set the MICMD.MIIRD bit. The read operation
     begins and the MISTAT.BUSY bit is set.*/
    WRITE_REG(ENC_MICMD, 0x1);

    /*3. Wait 10.24 .s. Poll the MISTAT.BUSY bit to be
     certain that the operation is complete. While
     busy, the host controller should not start any
     MIISCAN operations or write to the MIWRH
     register.
     When the MAC has obtained the register
     contents, the BUSY bit will clear itself.*/

    /* Assuming that we are running at 1MHz, a single cycle is
     * 1 us */
    //MAP_SysCtlDelay(((MAP_SysCtlClockGet()/3)/1000));
    wait();

    do {
        stat = READ_MREG(ENC_MISTAT);
    } while (stat & ENC_MISTAT_BUSY);

    /*4. Clear the MICMD.MIIRD bit.*/
    WRITE_REG(ENC_MICMD, 0x00);

    /*5. Read the desired data from the MIRDL and
     MIRDH registers. The order that these bytes are
     accessed is unimportant.
     */
    ret = READ_MREG(ENC_MIRDL) & 0xFF;
    ret |= READ_MREG(ENC_MIRDH) << 8;

    return ret;
}

/**
 * Write value to PHY address.
 * Reading procedure is described in ENC28J60 datasheet
 * section 3.3.
 */
void enc_phy_write(uint8_t addr, uint16_t value)
{
    uint8_t stat;

    WRITE_REG(ENC_MIREGADR, addr);
    WRITE_REG(ENC_MIWRL, value & 0xFF);
    WRITE_REG(ENC_MIWRH, value >> 8);

    wait();

    do {
        stat = READ_MREG(ENC_MISTAT);
    } while (stat & ENC_MISTAT_BUSY);
}

/**
 * Set the memory area to use for receiving packets.
 */
void enc_set_rx_area(uint16_t start, uint16_t end)
{
    WRITE_REG(ENC_ERXSTL, start & 0xFF);
    WRITE_REG(ENC_ERXSTH, (start >> 8) & 0xFFF);

    WRITE_REG(ENC_ERXNDL, end & 0xFF);
    WRITE_REG(ENC_ERXNDH, (end >> 8) & 0xFFF);

    WRITE_REG(ENC_ERXRDPTL, start & 0xFF);
    WRITE_REG(ENC_ERXRDPTH, (start >> 8) & 0xFFF);
}

/**
 * Set the MAC address.
 */
void enc_set_mac_addr(const uint8_t *mac_addr)
{
    WRITE_REG(ENC_MAADR1, mac_addr[0]);
    WRITE_REG(ENC_MAADR2, mac_addr[1]);
    WRITE_REG(ENC_MAADR3, mac_addr[2]);
    WRITE_REG(ENC_MAADR4, mac_addr[3]);
    WRITE_REG(ENC_MAADR5, mac_addr[4]);
    WRITE_REG(ENC_MAADR6, mac_addr[5]);
}

/**
 * Read the MAC address.
 */
void enc_get_mac_addr(uint8_t *mac_addr)
{
    mac_addr[0] = READ_REG(ENC_MAADR1);
    mac_addr[1] = READ_REG(ENC_MAADR2);
    mac_addr[2] = READ_REG(ENC_MAADR3);
    mac_addr[3] = READ_REG(ENC_MAADR4);
    mac_addr[4] = READ_REG(ENC_MAADR5);
    mac_addr[5] = READ_REG(ENC_MAADR6);
}

void enc_init(const uint8_t *mac)
{
    uint16_t phyreg;

    enc_reset();
    //while ((enc_read(0x1D) & ENC_ESTAT_CLKRDY) == 0);

    enc_switch_bank(0);
    //tprintf("Econ: 0x%x\r\n", READ_REG(ENC_ECON1));
    //tprintf("REV: 0x%x\r\n", READ_REG(ENC_EREVID));

    //SET_REG_BITS(ENC_ECON1, ENC_ECON1_TXRST | ENC_ECON1_RXRST);
    SET_REG_BITS(ENC_ECON1, ENC_ECON1_TXRST);
    CLEAR_REG_BITS(ENC_ECON1, ENC_ECON1_RXEN);

    SET_REG_BITS(ENC_ECON2, ENC_ECON2_AUTOINC);

    enc_set_rx_area(0x000, RX_END);

    phyreg = enc_phy_read(ENC_PHSTAT2);
    phyreg &= ~ENC_PHSTAT2_DPXSTAT;
    enc_phy_write(ENC_PHSTAT2, phyreg);

    phyreg = enc_phy_read(ENC_PHCON1);
    phyreg &= ~ENC_PHCON_PDPXMD;
    enc_phy_write(ENC_PHCON1, phyreg);

    /* Setup receive filter to receive
     * broadcast, multicast and unicast to the given MAC */
#if 1
    tprintf("Setting MAC: [%x:%x:%x:%x:%x:%x]\n",
        mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
#endif
    enc_set_mac_addr(mac);
    /* WRITE_REG(
        ENC_ERXFCON,
        ENC_ERXFCON_UCEN | ENC_ERXFCON_CRCEN | ENC_ERXFCON_BCEN |
        ENC_ERXFCON_MCEN); */
    WRITE_REG(
        ENC_ERXFCON,
        ENC_ERXFCON_UCEN | ENC_ERXFCON_CRCEN | ENC_ERXFCON_BCEN);

    /* Initialize MAC */
    WRITE_REG(ENC_MACON1,
          ENC_MACON1_TXPAUS | ENC_MACON1_RXPAUS | ENC_MACON1_MARXEN);

    WRITE_REG(
        ENC_MACON3,
        (0x1 << ENC_MACON3_PADCFG_SHIFT) | ENC_MACON3_TXRCEN |
        /*ENC_MACON3_FULDPX |*/ENC_MACON3_FRMLNEN);

    WRITE_REG(ENC_MAMXFLL, 1518 & 0xFF);
    WRITE_REG(ENC_MAMXFLH, (1518 >> 8) & 0xFF);

    WRITE_REG(ENC_MABBIPG, 0x12);
    WRITE_REG(ENC_MAIPGL, 0x12);
    WRITE_REG(ENC_MAIPGH, 0x0C);

    SET_REG_BITS(ENC_EIE, ENC_EIE_INTIE | ENC_EIE_PKTIE);

    CLEAR_REG_BITS(ENC_ECON1, ENC_ECON1_TXRST | ENC_ECON1_RXRST);
    SET_REG_BITS(ENC_ECON1, ENC_ECON1_RXEN);

    /* moved from send_start */
    WRITE_REG(ENC_ETXSTL, TX_START & 0xFF);
    WRITE_REG(ENC_ETXSTH, TX_START >> 8);

#if 1
    uint8_t mc[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    enc_get_mac_addr(mc);
    tprintf("Mac addr seems to be: [%x:%x:%x:%x:%x:%x]\n",
        mc[0], mc[1], mc[2], mc[3], mc[4], mc[5]);
#endif
}

uint8_t enc_action(void)
{
    /* Errata 4
     * The Receive Packet Pending Interrupt Flag
     * (EIR.PKTIF) does not reliably/accurately report
     * the status of pending packets. */
    /*
    uint8_t reg = READ_REG(ENC_EIR);
    if (reg & ENC_EIR_PKTIF) {
        if (READ_REG(ENC_EPKTCNT) > 0) {
            return 1;
        }
    }
    */
    if (READ_REG(ENC_EPKTCNT) > 0) {
        return 1;
    }
    return 0;
}

void enc_print_status(uint8_t *status)
{
    tprintf("STATUS BITS: \r\n");
    tprintf("  ByteCount: %i, CollisionCount: %i, TotByteOnWire: %i\r\n",
        status[0] | (status[1] << 8),
        status[2] & 0x0f,
        status[4] | (status[5] << 8));
    tprintf("  TxDone: %i, CRCErr:%i, LenChkErr: %i, LenOutOfRange: %i\r\n",
        TSV_GETBIT(status, TSV_TXDONE),
        TSV_GETBIT(status, TSV_TXCRCERROR),
        TSV_GETBIT(status, TSV_TXLENCHKERROR),
        TSV_GETBIT(status, TSV_TXLENOUTOFRANGE));
    tprintf("  Multicast: %i, Broadcast: %i, "
        "PacketDefer: %i, ExDefer: %i\r\n",
        TSV_GETBIT(status, TSV_TXMULTICAST),
        TSV_GETBIT(status, TSV_TXBROADCAST),
        TSV_GETBIT(status, TSV_TXPACKETDEFER),
        TSV_GETBIT(status, TSV_TXEXDEFER));
    tprintf("  ExCollision: %i, LateCollision: %i, "
        "Giant: %i, Underrun: %i\r\n",
        TSV_GETBIT(status, TSV_TXEXCOLLISION),
        TSV_GETBIT(status, TSV_TXLATECOLLISION),
        TSV_GETBIT(status, TSV_TXGIANT),
        TSV_GETBIT(status, TSV_TXUNDERRUN));
    tprintf("  ControlFrame: %i, PauseFrame: %i, "
         "BackPressApp: %i, VLanTagFrame: %i\r\n",
         TSV_GETBIT(status, TSV_TXCONTROLFRAME),
         TSV_GETBIT(status, TSV_TXPAUSEFRAME),
         TSV_GETBIT(status, TSV_BACKPRESSUREAPP),
         TSV_GETBIT(status, TSV_TXVLANTAGFRAME));
}

static void enc_regdump(void)
{
    tprintf(
        "HwRevID: 0x%x\n"
        "Cntrl: ECON1   ECON2   ESTAT   EIR     EIE\n"
        "       0x%x  0x%x  0x%x  0x%x  0x%x\n"
        "MAC  : MACON1  MACON3  MACON4\n"
        "       0x%x  0x%x  0x%x\n"
        "Rx   : ERXST  ERXND  ERXWRPT ERXRDPT ERXFCON   EPKTCNT   MAMXFL\n"
        "       0x%x 0x%x 0x%x  0x%x  "
        "0x%x    0x%x    0x%x\n"
        "Tx   : ETXST  ETXND  MACLCON1\n"
        "       0x%x 0x%x 0x%x\n",
        READ_REG(ENC_EREVID),
        READ_REG(ENC_ECON1), READ_REG(ENC_ECON2),
        READ_REG(ENC_ESTAT), READ_REG(ENC_EIR),
        READ_REG(ENC_EIE), READ_REG(ENC_MACON1),
        READ_REG(ENC_MACON3), READ_REG(ENC_MACON4),
        READ_REG(ENC_ERXSTL), READ_REG(ENC_ERXNDL),
        READ_REG(ENC_ERXWRPTL),
        READ_REG(ENC_ERXRDPTL),
        READ_REG(ENC_ERXFCON),
        READ_REG(ENC_EPKTCNT),
        READ_REG(ENC_MAMXFLL), READ_REG(ENC_ETXSTL),
        READ_REG(ENC_ETXNDL),
        READ_REG(ENC_MACLCON1));
}

void enc_send_packet_begin(uint16_t count)
{
    uint8_t control;
    uint16_t tx_end;

    /* Errata B7 10 */
    SET_REG_BITS(ENC_ECON1, ENC_ECON1_TXRST);
    CLEAR_REG_BITS(ENC_ECON1, ENC_ECON1_TXRST);
    CLEAR_REG_BITS(ENC_EIR, ENC_EIR_TXIF);

    /* moved to init */
    /* WRITE_REG(ENC_ETXSTL, TX_START & 0xFF);
    WRITE_REG(ENC_ETXSTH, TX_START >> 8); */

    /*
    tprintf("Setting EWRPT to [0x%x] (0x%x | 0x%x)\r\n",
        TX_START, (TX_START & 0xFF), TX_START >> 8);
    */
    WRITE_REG(ENC_EWRPTL, TX_START & 0xFF);
    WRITE_REG(ENC_EWRPTH, TX_START >> 8);

    tx_end = TX_START + count;
    WRITE_REG(ENC_ETXNDL, tx_end & 0xFF);
    WRITE_REG(ENC_ETXNDH, tx_end >> 8);

    /*
    tprintf("Write pointer start: [0x%x]\r\n",
        READ_REG(ENC_EWRPTL) | (READ_REG(ENC_EWRPTH) << 8));
    tprintf("End pointer: [0x%x]\r\n",
        READ_REG(ENC_ETXNDL) | (READ_REG(ENC_ETXNDH) << 8));
    */

    control = 0x00;
    enc_wbm(&control, 1);
}

void enc_send_packet_end(uint16_t count)
{
    uint8_t status[7];
    //uint16_t transmit_count;
    uint16_t tx_end;

    tx_end = TX_START + count;

    SET_REG_BITS(ENC_ECON1, ENC_ECON1_TXRTS);

    /* Busy wait for the transmission to complete */
    while (READ_REG(ENC_ECON1) & ENC_ECON1_TXRTS);

    uint16_t txst_after =
        READ_REG(ENC_ETXSTL) | (READ_REG(ENC_ETXSTH) << 8);
    uint16_t txnd_after =
        READ_REG(ENC_ETXNDL) | (READ_REG(ENC_ETXNDH) << 8);
    uint16_t wrpt_after = 
        READ_REG(ENC_EWRPTL) | (READ_REG(ENC_EWRPTL) << 8);

    /* Read status bits */
    tx_end++;
    WRITE_REG(ENC_ERDPTL, tx_end & 0xFF);
    WRITE_REG(ENC_ERDPTH, tx_end >> 8);
    enc_rbm(status, 7);

    //transmit_count = status[0] | (status[1] << 8);

    if (status[2] & 0x80) {
        //tprintf("Transmit OK\n");
    } else {
        tprintf("Transmit NOT OK\n");
        enc_print_status(status);
        while (1);
    }
}

void enc_send_packet(const uint8_t *buf, uint16_t count)
{
    //tprintf("enc_send_packet(..., %i)\r\n", count);
    enc_wbm(buf, count);
}

uint16_t enc_receive_packet_begin(void)
{
    /* Receive a single packet */
    uint8_t header[6];
    uint8_t *status = header + 2;
    uint16_t data_count;
    //tprintf("enc_receive_packet_begin()\r\n");

    if (READ_REG(ENC_EIR) & ENC_EIR_RXERIF) {
        tprintf("There are errors!!\r\n");
    }
    if (READ_REG(ENC_EPKTCNT) == 0) {
        tprintf("But there are no packets?!?!\r\n");
    }

    WRITE_REG(ENC_ERDPTL, enc_next_packet & 0xFF);
    WRITE_REG(ENC_ERDPTH, (enc_next_packet >> 8) & 0xFF);
    enc_rbm(header, 6);

    /* Update next packet pointer */
    enc_next_packet = header[0] | (header[1] << 8);

    data_count = status[0] | (status[1] << 8);
    if (!(status[2] & (1 << 7))) {
        data_count = 0;
        tprintf("Invalid!\r\n");
        enc_print_status(status);
    }
    return data_count;
}

void enc_receive_packet(uint8_t *buf, uint16_t len)
{
    //tprintf("enc_receive_packet(..., %i)\r\n", len);
    enc_rbm(buf, len);
}

void enc_receive_packet_end(void)
{
    uint16_t erxst;

    //tprintf("enc_receive_packet_end()\r\n");
    erxst = READ_REG(ENC_ERXSTL) | (READ_REG(ENC_ERXSTH) << 8);

    /* Mark packet as read */
    if (enc_next_packet == erxst) {
        WRITE_REG(ENC_ERXRDPTL, READ_REG(ENC_ERXNDL));
        WRITE_REG(ENC_ERXRDPTH, READ_REG(ENC_ERXNDH));
    } else {
        WRITE_REG(ENC_ERXRDPTL, (enc_next_packet-1) & 0xFF);
        WRITE_REG(ENC_ERXRDPTH, ((enc_next_packet-1) >> 8) & 0xFF);
    }
    SET_REG_BITS(ENC_ECON2, ENC_ECON2_PKTDEC);
}

uint8_t enc_test(void)
{
    uint8_t mc[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    uint8_t rev;

    rev = READ_REG(ENC_EREVID);
    enc_get_mac_addr(mc);

    tprintf("REV: 0x%x, MAC: [%x:%x:%x:%x:%x:%x]\n",
        rev, mc[0], mc[1], mc[2], mc[3], mc[4], mc[5]);
    enc_regdump();

    return 0;
}

uint8_t enc_revid(void)
{
    return READ_REG(ENC_EREVID);
}

/* vim: set et sw=4: */
