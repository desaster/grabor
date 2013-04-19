/* Copyright (C) 2013 Upi Tamminen
 * See the COPYRIGHT file for more information */

#ifndef __LWIP_LWIOPTS_H__
#define __LWIP_LWIOPTS_H__

#include <sys/time.h>

#include "tprintf.h"

//#define LWIP_HTTPD_CGI 1
//#define LWIP_HTTPD_SSI 1

#define LWIP_DEBUG 1
//#define IP_DEBUG LWIP_DBG_ON
//#define ETHARP_DEBUG LWIP_DBG_ON
//#define NETIF_DEBUG LWIP_DBG_ON
//#define ICMP_DEBUG LWIP_DBG_ON
#define HTTPD_DEBUG LWIP_DBG_ON
#define MEM_DEBUG LWIP_DBG_ON
//#define PBUF_DEBUG LWIP_DBG_ON

//#define LWIP_STATS 1
//#define MEM_STATS 1
//#define LWIP_STATS_DISPLAY 1

/* Align memory on 4 byte boundery (32-bit) */
#define MEM_ALIGNMENT                   4

/* No operating system present */
#define NO_SYS                          1
//#define SYS_LIGHTWEIGHT_PROT            1
//#define MEM_SIZE                        1600

/* Time in milliseconds to perform ARP processing */
#define ETHARP_TMR_INTERVAL             5000
#define LWIP_TIMEVAL_PRIVATE            0
#define LWIP_SOCKET                     0
#define LWIP_NETCONN                    0
#define LWIP_ARP                        1
//#define MEMP_NUM_PBUF                   1
//#define MEMP_NUM_RAW_PCB                1
//#define MEMP_NUM_UDP_PCB                1
//#define MEMP_NUM_TCP_PCB                1
//#define MEMP_NUM_TCP_PCB_LISTEN         0
//#define MEMP_NUM_TCP_SEG                8
//#define MEMP_NUM_NETBUF                 0
//#define MEMP_NUM_NETCONN                2
//#define MEMP_NUM_API_MSG                0
//#define MEMP_NUM_TCPIP_MSG              0
#define PBUF_POOL_SIZE                  4
//#define PBUF_POOL_BUFSIZE               64
//#define ARP_TABLE_SIZE                  2
//#define IP_REASS_BUFSIZE                600
#define TCP_WND                         4000
//#define TCP_QUEUE_OOSEQ                 0
#define TCP_MSS                         1460

#endif /* __LWIP_LWIOPTS_H__ */
