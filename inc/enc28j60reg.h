/*
 * This is from
 * https://github.com/xpgdk/stellaris-enc28j60-booster
 * Thank you, author
 */

#ifndef _ENC28J60_REG_H
#define _ENC28J60_REG_H

#define ANY_BANK 4
#define ENC_BANK(REG)		REG ## _BANK

#define ENC_ERDPTL			0x00
#define ENC_ERDPTL_BANK		0
#define ENC_ERDPTH			0x01
#define ENC_ERDPTH_BANK		0
#define ENC_EWRPTL			0x02
#define ENC_EWRPTL_BANK		0
#define ENC_EWRPTH			0x03
#define ENC_EWRPTH_BANK		0
#define ENC_ETXSTL			0x04
#define ENC_ETXSTL_BANK		0
#define ENC_ETXSTH			0x05
#define ENC_ETXSTH_BANK		0
#define ENC_ETXNDL			0x06
#define ENC_ETXNDL_BANK		0
#define ENC_ETXNDH			0x07
#define ENC_ETXNDH_BANK		0
#define ENC_ERXSTL			0x08
#define ENC_ERXSTL_BANK		0
#define ENC_ERXSTH			0x09
#define ENC_ERXSTH_BANK		0
#define ENC_ERXNDL			0x0A
#define ENC_ERXNDL_BANK		0
#define ENC_ERXNDH			0x0B
#define ENC_ERXNDH_BANK		0
#define ENC_ERXRDPTL		0x0C
#define ENC_ERXRDPTL_BANK	0
#define ENC_ERXRDPTH		0x0D
#define ENC_ERXRDPTH_BANK	0
#define ENC_ERXWRPTL		0x0E
#define ENC_ERXWRPTL_BANK	0
#define ENC_ERXWRPTH		0x0F
#define ENC_ERXWRPTH_BANK	0

#define ENC_ERXFCON			0x18
#define ENC_ERXFCON_BANK	1
#define		ENC_ERXFCON_BCEN	(1<<0)
#define		ENC_ERXFCON_MCEN	(1<<1)
#define		ENC_ERXFCON_HTEN	(1<<2)
#define		ENC_ERXFCON_MPEN	(1<<3)
#define		ENC_ERXFCON_PMEN	(1<<4)
#define		ENC_ERXFCON_CRCEN	(1<<5)
#define		ENC_ERXFCON_ANDOR	(1<<6)
#define		ENC_ERXFCON_UCEN	(1<<7)
#define ENC_EPKTCNT			0x19
#define ENC_EPKTCNT_BANK	1

#define ENC_MACON1			0x00
#define		ENC_MACON1_MARXEN	(1<<0)
#define		ENC_MACON1_PASSALL	(1<<1)
#define		ENC_MACON1_RXPAUS	(1<<2)
#define		ENC_MACON1_TXPAUS	(1<<3)
#define ENC_MACON1_BANK		2
#define	ENC_MACON3			0x02
#define		ENC_MACON3_FULDPX	(1<<0)
#define		ENC_MACON3_FRMLNEN	(1<<1)
#define		ENC_MACON3_HFRMEN	(1<<2)
#define 	ENC_MACON3_PHDREN	(1<<3)
#define 	ENC_MACON3_TXRCEN	(1<<4)
#define 	ENC_MACON3_PADCFG_MASK	0x07
#define 	ENC_MACON3_PADCFG_SHIFT 5
#define ENC_MACON3_BANK		2
#define ENC_MACON4			0x03
#define		ENC_MACON4_NOBKOFF	(1<<4)
#define		ENC_MACON4_BPEN		(1<<5)
#define		ENC_MACON4_DEFER	(1<<6)
#define ENC_MACON4_BANK		2
#define ENC_MABBIPG			0x04
#define ENC_MABBIPG_BANK	2
#define ENC_MAIPGL			0x06
#define ENC_MAIPGL_BANK		2
#define ENC_MAIPGH			0x07
#define ENC_MAIPGH_BANK		2
#define ENC_MACLCON1		0x08
#define ENC_MACLCON1_BANK	2
#define ENC_MICMD			0x12
#define ENC_MICMD_BANK		2
#define ENC_MIREGADR		0x14
#define ENC_MIREGADR_BANK	2
#define ENC_MIWRL			0x16
#define ENC_MIWRL_BANK		2
#define ENC_MIWRH			0x17
#define ENC_MIWRH_BANK		2
#define ENC_MIRDL			0x18
#define	ENC_MIRDL_BANK		2
#define	ENC_MIRDH			0x19
#define	ENC_MIRDH_BANK		2
#define ENC_MAMXFLL			0x0A
#define ENC_MAMXFLL_BANK	2
#define ENC_MAMXFLH			0x0B
#define ENC_MAMXFLH_BANK	2

#define ENC_MAADR5			0x00
#define	ENC_MAADR5_BANK		3
#define ENC_MAADR6			0x01
#define ENC_MAADR6_BANK		3
#define ENC_MAADR3			0x02
#define ENC_MAADR3_BANK		3
#define ENC_MAADR4			0x03
#define ENC_MAADR4_BANK		3
#define ENC_MAADR1			0x04
#define ENC_MAADR1_BANK		3
#define ENC_MAADR2			0x05
#define ENC_MAADR2_BANK		3
#define ENC_MISTAT			0x0A
#define		ENC_MISTAT_BUSY		(1<<0)
#define		ENC_MISTAT_SCAN		(1<<1)
#define		ENC_MISTAT_NVALID 	(1<<2)
#define ENC_MISTAT_BANK		3
#define ENC_EREVID			0x12
#define ENC_EREVID_BANK		3

/* Common registers */
#define ENC_ECON1 	0x1F
#define		ENC_ECON1_BSEL_MASK	0x03
#define		ENC_ECON1_BSEL_SHIFT 0
#define 	ENC_ECON1_RXEN		(1<<2)
#define 	ENC_ECON1_TXRTS		(1<<3)
#define 	ENC_ECON1_CSUMEN	(1<<4)
#define 	ENC_ECON1_DMAST		(1<<5)
#define 	ENC_ECON1_RXRST		(1<<6)
#define 	ENC_ECON1_TXRST		(1<<7)
#define	ENC_ECON1_BANK	ANY_BANK
#define ENC_ECON2 	0x1E
#define		ENC_ECON2_VRPS		(1<<3)
#define		ENC_ECON2_PWSRV		(1<<5)
#define		ENC_ECON2_PKTDEC	(1<<6)
#define		ENC_ECON2_AUTOINC	(1<<7)
#define ENC_ECON2_BANK	ANY_BANK
#define ENC_ESTAT 	0x1D
#define		ENC_ESTAT_CLKRDY	(1<<0)
#define		ENC_ESTAT_TXABRT	(1<<1)
#define		ENC_ESTAT_RXBUSY	(1<<2)
#define		ENC_ESTAT_LATECOL	(1<<4)
#define		ENC_ESTAT_BUFFER	(1<<6)
#define		ENC_ESTAT_INT		(1<<7)
#define	ENC_ESTAT_BANK	ANY_BANK
#define ENC_EIR		0x1C
#define		ENC_EIR_RXERIF		(1<<0)
#define		ENC_EIR_TXERIF		(1<<1)
#define		ENC_EIR_TXIF		(1<<3)
#define		ENC_EIR_LINKIF		(1<<4)
#define		ENC_EIR_DMAIF		(1<<5)
#define		ENC_EIR_PKTIF		(1<<6)
#define	ENC_EIR_BANK ANY_BANK
#define ENC_EIE		0x1B
#define		ENC_EIE_RXERIE		(1<<0)
#define		ENC_EIE_TXERIE		(1<<1)
#define		ENC_EIE_TXIE		(1<<3)
#define		ENC_EIE_LINKIE		(1<<4)
#define		ENC_EIE_DMAIE		(1<<5)
#define		ENC_EIE_PKTIE		(1<<6)
#define		ENC_EIE_INTIE		(1<<7)
#define ENC_EIE_BANK ANY_BANK

/* PHY Registers */
#define ENC_PHSTAT1	0x01
#define ENC_PHSTAT2 0x11
#define		ENC_PHSTAT2_DPXSTAT (1<<9)
#define ENC_PHCON1	0x00
#define		ENC_PHCON_PDPXMD	(1<<8)

/* ENC28J60 Transmit Status Vector */
#define TSV_TXBYTECNT           0
#define TSV_TXCOLLISIONCNT      16
#define TSV_TXCRCERROR          20
#define TSV_TXLENCHKERROR       21
#define TSV_TXLENOUTOFRANGE     22
#define TSV_TXDONE              23
#define TSV_TXMULTICAST         24
#define TSV_TXBROADCAST         25
#define TSV_TXPACKETDEFER       26
#define TSV_TXEXDEFER           27
#define TSV_TXEXCOLLISION       28
#define TSV_TXLATECOLLISION     29
#define TSV_TXGIANT             30
#define TSV_TXUNDERRUN          31
#define TSV_TOTBYTETXONWIRE     32
#define TSV_TXCONTROLFRAME      48
#define TSV_TXPAUSEFRAME        49
#define TSV_BACKPRESSUREAPP     50
#define TSV_TXVLANTAGFRAME      51

#define TSV_SIZE                7
#define TSV_BYTEOF(x)           ((x) / 8)
#define TSV_BITMASK(x)          (1 << ((x) % 8))
#define TSV_GETBIT(x, y)        (((x)[TSV_BYTEOF(y)] & TSV_BITMASK(y)) ? 1 : 0)

#endif
