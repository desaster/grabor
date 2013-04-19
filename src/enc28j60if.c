/* Copyright (C) 2013 Upi Tamminen
 * See the COPYRIGHT file for more information
 *
 * Based on the Ethernet Interface skeleton (ethernetif.c)
 * which can be found in the lwip distribution in
 * lwip-1.4.1/src/netif/ethernetif.c */

#include <stdint.h>
#include "inc/LPC17xx.h"
#include "config.h"
#include "hdr/hdr_sc.h"

#include "lwipopts.h"

#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/pbuf.h"
#include "lwip/udp.h"
#include "lwip/tcp.h"
#include "lwip/tcp_impl.h"
#include "lwip/stats.h"

#include "netif/etharp.h"

#include "enc28j60.h"
#include "httpd.h"

#define IFNAME0 'e'
#define IFNAME1 'n'

const uint8_t mac_addr[] = { 0x00, 0xC0, 0x033, 0x50, 0x48, 0x12 };

static struct ip_addr my_ipaddr_data;
static struct ip_addr my_netmask_data;
static struct ip_addr my_gw_data;

struct netif lwip_netif;

/**
 * Helper struct to hold private data used to operate your ethernet interface.
 * Keeping the ethernet address of the MAC in this struct is not necessary
 * as it is already kept in the struct netif.
 * But this is only an example, anyway...
 */
struct ethernetif {
    struct eth_addr *ethaddr;
    /* Add whatever per-interface state that is needed here. */
};

/**
 * Should allocate a pbuf and transfer the bytes of the incoming
 * packet from the interface into the pbuf.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return a pbuf filled with the received packet (including MAC header)
 *         NULL on memory error
 */
static struct pbuf * low_level_input(struct netif *netif)
{
    struct pbuf *p, *q;
    u16_t len;

    LWIP_UNUSED_ARG(netif);

    /* Obtain the size of the packet and put it into the "len"
       variable. */
    len = enc_receive_packet_begin();
    if (!len) return NULL;
    //tprintf("Received packet with len != NULL\r\n");

#if ETH_PAD_SIZE
    len += ETH_PAD_SIZE; /* allow room for Ethernet padding */
#endif

    /* We allocate a pbuf chain of pbufs from the pool. */
    p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);

    if (p != NULL) {
        //tprintf("And p != NULL\r\n");

#if ETH_PAD_SIZE
        pbuf_header(p, -ETH_PAD_SIZE); /* drop the padding word */
#endif

        /* We iterate over the pbuf chain until we have read the entire
         * packet into the pbuf. */
        for (q = p; q != NULL; q = q->next) {
            //tprintf("Receiving data of %d bytes\r\n", q->len);
            /* Read enough bytes to fill this pbuf in the chain. The
             * available data in the pbuf is given by the q->len
             * variable.
             * This does not necessarily have to be a memcpy, you can also preallocate
             * pbufs for a DMA-enabled MAC and after receiving truncate it to the
             * actually received size. In this case, ensure the tot_len member of the
             * pbuf is the sum of the chained pbuf len members.
             */
            enc_receive_packet(q->payload, q->len);
        }
        //tprintf("done receiving packet :)\r\n");
        enc_receive_packet_end();

#if ETH_PAD_SIZE
        pbuf_header(p, ETH_PAD_SIZE); /* reclaim the padding word */
#endif

        LINK_STATS_INC(link.recv);
    } else {
        tprintf("Ending packet!\r\n");
        enc_receive_packet_end();
        LINK_STATS_INC(link.memerr);
        LINK_STATS_INC(link.drop);
    }

    return p;  
}

/**
 * This function should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @param p the MAC packet to send (e.g. IP packet including MAC addresses and type)
 * @return ERR_OK if the packet could be sent
 *         an err_t value if the packet couldn't be sent
 *
 * @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
 *       strange results. You might consider waiting for space in the DMA queue
 *       to become availale since the stack doesn't retry to send a packet
 *       dropped because of memory failure (except for the TCP timers).
 */

static err_t low_level_output(struct netif *netif, struct pbuf *p)
{
    struct pbuf *q;

    LWIP_UNUSED_ARG(netif);

    if (p->tot_len > 1530) {
        tprintf("p->tot_len: %i\r\n", p->tot_len);
    }
    enc_send_packet_begin(p->tot_len);

#if ETH_PAD_SIZE
    pbuf_header(p, -ETH_PAD_SIZE); /* drop the padding word */
#endif

    for (q = p; q != NULL; q = q->next) {
        /* Send the data from the pbuf to the interface, one pbuf at a
           time. The size of the data in each pbuf is kept in the ->len
           variable. */
        enc_send_packet(q->payload, q->len);
    }

    enc_send_packet_end(p->tot_len);

#if ETH_PAD_SIZE
    pbuf_header(p, ETH_PAD_SIZE); /* reclaim the padding word */
#endif

    LINK_STATS_INC(link.xmit);

    return ERR_OK;
}

/**
 * In this function, the hardware should be initialized.
 * Called from ethernetif_init().
 *
 * @param netif the already initialized lwip network interface structure
 *        for this ethernetif
 */
static void low_level_init(struct netif *netif)
{
    /* set MAC hardware address length */
    netif->hwaddr_len = ETHARP_HWADDR_LEN;

    /* set MAC hardware address */
    netif->hwaddr[0] = mac_addr[0];
    netif->hwaddr[1] = mac_addr[1];
    netif->hwaddr[2] = mac_addr[2];
    netif->hwaddr[3] = mac_addr[3];
    netif->hwaddr[4] = mac_addr[4];
    netif->hwaddr[5] = mac_addr[5];

    /* maximum transfer unit */
    netif->mtu = 1500;

    /* device capabilities */
    /* don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */
    netif->flags =
        NETIF_FLAG_BROADCAST |
        NETIF_FLAG_ETHARP |
        NETIF_FLAG_LINK_UP;

    /* Do whatever else is needed to initialize interface. */  
    enc_init(mac_addr);
}

/**
 * Should be called at the beginning of the program to set up the
 * network interface. It calls the function low_level_init() to do the
 * actual setup of the hardware.
 *
 * This function should be passed as a parameter to netif_add().
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return ERR_OK if the loopif is initialized
 *         ERR_MEM if private data couldn't be allocated
 *         any other err_t on error
 */
err_t ethernetif_init(struct netif *netif)
{
    struct ethernetif *ethernetif;

    LWIP_ASSERT("netif != NULL", (netif != NULL));

    ethernetif = mem_malloc(sizeof(struct ethernetif));
    if (ethernetif == NULL) {
        LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_init: out of memory\n"));
        return ERR_MEM;
    }

#if LWIP_NETIF_HOSTNAME
    /* Initialize interface hostname */
    netif->hostname = "lwip";
#endif /* LWIP_NETIF_HOSTNAME */

    /*
     * Initialize the snmp variables and counters inside the struct netif.
     * The last argument should be replaced with your link speed, in units
     * of bits per second.
     */
    NETIF_INIT_SNMP(netif, snmp_ifType_ethernet_csmacd, LINK_SPEED_OF_YOUR_NETIF_IN_BPS);

    netif->state = ethernetif;
    netif->name[0] = IFNAME0;
    netif->name[1] = IFNAME1;
    /* We directly use etharp_output() here to save a function call.
     * You can instead declare your own function an call etharp_output()
     * from it if you have to do some checks before sending (e.g. if link
     * is available...) */
    netif->output = etharp_output;
    netif->linkoutput = low_level_output;

    ethernetif->ethaddr = (struct eth_addr *)&(netif->hwaddr[0]);

    /* initialize the hardware */
    low_level_init(netif);

    return ERR_OK;
}

/**
 * This function should be called when a packet is ready to be read
 * from the interface. It uses the function low_level_input() that
 * should handle the actual reception of bytes from the network
 * interface. Then the type of the received packet is determined and
 * the appropriate input function is called.
 *
 * @param netif the lwip network interface structure for this ethernetif
 */
static void ethernetif_input(struct netif *netif)
{
    struct eth_hdr *ethhdr;
    struct pbuf *p;

    /* move received packet into a new pbuf */
    p = low_level_input(netif);
    /* no packet could be read, silently ignore this */
    if (p == NULL) return;
    /* points to packet payload, which starts with an Ethernet header */
    ethhdr = p->payload;

    //tprintf("We have packet: [0x%x]\r\n", htons(ethhdr->type));
    switch (htons(ethhdr->type)) {
        /* IP or ARP packet? */
        case ETHTYPE_IP:
        case ETHTYPE_ARP:
            /* full packet send to tcpip_thread to process */
            //tprintf("Passing to netif->input\r\n");
            if (netif->input(p, netif) != ERR_OK) {
                LWIP_DEBUGF(NETIF_DEBUG,
                    ("ethernetif_input: IP input error\n"));
                pbuf_free(p);
                p = NULL;
            }
            break;

        default:
            pbuf_free(p);
            p = NULL;
            break;
    }
}

/*
 * init lwip & enc28j60
 */
void ethernet_init(void)
{
    mem_init();
    memp_init();
    pbuf_init();
    etharp_init();
    ip_init();
    udp_init();
    tcp_init();

    IP4_ADDR(&my_ipaddr_data,  192, 168, 88, 13);
    IP4_ADDR(&my_netmask_data, 255, 255, 255, 0);
    IP4_ADDR(&my_gw_data, 192, 168, 88, 147);

    netif_add(&lwip_netif, &my_ipaddr_data, &my_netmask_data, &my_gw_data,
        NULL, ethernetif_init, ethernet_input);
    netif_set_default(&lwip_netif);
    netif_set_up(&lwip_netif);

    httpd_init();
}

__INLINE void ethernet_action(void)
{
    while (enc_action()) {
        ethernetif_input(&lwip_netif);
    }
}

/* vim: set sw=4 et: */
