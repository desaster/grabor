/* Copyright (C) 2013 Upi Tamminen
 * See the COPYRIGHT file for more information
 *
 * http://lwip.wikia.com/wiki/Raw/TCP
 *
 * This code is based on the tcpecho_raw from lwIP's contrib-1.4.1
 */

#include "lwip/opt.h"
#include "lwip/debug.h"
#include "lwip/stats.h"
#include "lwip/tcp.h"

#include "tprintf.h"
#include "uart0.h"
#include "cpld.h"

static const char http_response_200[] =
    "HTTP/1.1 200 OK\r\n"
    "Server: lp1768\r\n"
    "Access-Control-Allow-Origin: *\r\n"
    "Content-Type: text/html\r\n\r\n";

static const char http_response_404[] =
    "HTTP/1.1 404 Not Found\r\n"
    "Server: lp1768\r\n"
    "Content-Type: text/html\r\n\r\n";

static const char page_root[] =
"<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd\">"
"<html>"
  "<head>"
    "<link rel=\"stylesheet\" type=\"text/css\" href=\"http://www.rpg.fi/grabor/grabor.css\" />"
    "<script type=\"text/javascript\" src=\"http://www.rpg.fi/grabor/jquery-1.4.4-binary-ajax.js\"></script>"
    "<script type=\"text/javascript\" src=\"http://www.rpg.fi/grabor/jdataview.js\"></script>"
    "<script type=\"text/javascript\" src='http://www.rpg.fi/grabor/grabor_brd.js'></script>"
  "</head>"
  "<body>"
    "<h1>Grabor</h1>"
    "<div id=\"canvasblock\">"
      "<canvas id='canvas' width=\"704\" height=\"516\">"
        "Canvas not supported"
      "</canvas>"
    "</div>"
  "</body>"
"</html>";

static const char page_404[] =
"<html>"
  "<head>"
    "<title>404 Not Found!</title>"
  "</head>"
  "<body>"
    "<p>The page you requested is not here!</p>"
  "</body>"
"</html>";

char buf[2048]; // this can be smaller, but 2048 is currently used by _test

#define DMABUF_SIZE 2048
extern uint8_t dmabuf[2048];

static struct tcp_pcb *httpd_pcb;

enum httpd_states {
    ST_NONE = 0,
    ST_ACCEPTED,
    ST_READY, // ready to send data
    ST_CLOSING,
    ST_SENDING,
};

struct httpd_state {
    u8_t state;
    u8_t retries;
    struct tcp_pcb *pcb;
    u32_t bodycount;
    u8_t (*handler)(struct tcp_pcb *, struct httpd_state *);
    u32_t data_to_send;
    u8_t *sendbuf;
    u8_t *hdrbuf;
    u16_t grabbufpos;
};

void httpd_init(void);
err_t httpd_accept(void *arg, struct tcp_pcb *newpcb, err_t err);
err_t httpd_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
err_t httpd_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
void httpd_error(void *arg, err_t err);
err_t httpd_poll(void *arg, struct tcp_pcb *tpcb);
static err_t httpd_sent(void *arg, struct tcp_pcb *tpcb, u16_t len);
//void httpd_send(struct tcp_pcb *tpcb, struct httpd_state *st);
u8_t httpd_send_test(struct tcp_pcb *tpcb, struct httpd_state *st);
u8_t httpd_send_grab(struct tcp_pcb *tpcb, struct httpd_state *st);
u8_t httpd_send_buf(struct tcp_pcb *tpcb, struct httpd_state *st);
void httpd_close(struct tcp_pcb *tpcb, struct httpd_state *st);

void httpd_init(void)
{
    err_t err;

    httpd_pcb = tcp_new();
    if (httpd_pcb == NULL) {
        LWIP_DEBUGF(HTTPD_DEBUG, ("Could not create PCB!\r\n"));
        while (1);
    }

    err = tcp_bind(httpd_pcb, IP_ADDR_ANY, 80);
    if (err != ERR_OK) {
        LWIP_DEBUGF(HTTPD_DEBUG, ("Could not bind!\r\n"));
        while (1);
    }
    httpd_pcb = tcp_listen(httpd_pcb);
    tcp_accept(httpd_pcb, httpd_accept);
}

/*
 * Connection arrives here
 */
err_t httpd_accept(void *arg, struct tcp_pcb *newpcb, err_t err)
{
    struct httpd_state *st;

    LWIP_UNUSED_ARG(arg);
    LWIP_UNUSED_ARG(err);
    LWIP_DEBUGF(HTTPD_DEBUG, ("httpd_accept()\r\n"));

    /* Unless this pcb should have NORMAL priority, set its priority now.
       When running out of pcbs, low priority pcbs can be aborted to create
       new pcbs of higher priority. */
    tcp_setprio(newpcb, TCP_PRIO_MIN);

    st = (struct httpd_state *)mem_malloc(sizeof(struct httpd_state));
    if (st == NULL) {
        return ERR_MEM;
    }

    st->state = ST_ACCEPTED;
    st->pcb = newpcb;
    st->retries = 0;
    st->bodycount = 0;
    /* pass newly allocated es to our callbacks */
    tcp_arg(newpcb, st);
    tcp_recv(newpcb, httpd_recv);
    tcp_err(newpcb, httpd_error);
    tcp_poll(newpcb, httpd_poll, 4);
    tcp_nagle_disable(newpcb);
    return ERR_OK;
}

err_t httpd_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
    struct httpd_state *st;
    err_t ret_err;
    u16_t i;

    LWIP_ASSERT("arg != NULL", arg != NULL);
    st = (struct httpd_state *) arg;
    if (p == NULL) {
        /* remote host closed connection */
        LWIP_DEBUGF(HTTPD_DEBUG, ("Remote host closed connection.\r\n"));
        st->state = ST_CLOSING;
        httpd_close(tpcb, st);
        ret_err = ERR_OK;
    } else if (err != ERR_OK) {
        /* cleanup, for unkown reason */
        pbuf_free(p);
        ret_err = err;
    } else if (st->state == ST_ACCEPTED) {
        /* first data chunk in p->payload */
        st->state = ST_READY;
        ret_err = ERR_OK;
        memcpy(buf, p->payload, p->len);
        buf[p->len - 1] = '\0';
        /* LWIP_DEBUGF(HTTPD_DEBUG, (
            "First chunk arrived: [%s] (%d)\r\n", buf, p->len)); */
        st->handler = NULL;
        st->data_to_send = 0;
        if (p->len > 4 && strncmp(buf, "GET ", 4) == 0) {
            //tprintf("Seems to be GET\r\n");
            for (i = 4; i < p->len; i ++) {
                if (buf[i] == ' ') {
                    buf[i] = '\0';
                }
            }
            tprintf("Request: [%s]\r\n", buf + 4);
            if (strcmp(buf + 4, "/") == 0) {
                st->handler = &httpd_send_buf;
                st->sendbuf = (u8_t *) page_root;
                st->hdrbuf = (u8_t *) http_response_200;
                st->data_to_send = sizeof(page_root) - 1;
                tprintf("body1 | sizeof: %d strlen: %d\r\n",
                    sizeof(page_root), strlen((char *) page_root));
            } else if (strcmp(buf + 4, "/test") == 0) {
                st->hdrbuf = (u8_t *) http_response_200;
                st->handler = &httpd_send_test;
                st->data_to_send = 4000000000;
                //st->data_to_send = 100000;
            } else if (strcmp(buf + 4, "/grab") == 0) {
                cpld_capture();
                st->hdrbuf = (u8_t *) http_response_200;
                st->handler = &httpd_send_grab;
                st->data_to_send = 704*516;
                st->grabbufpos = DMABUF_SIZE;
                //st->data_to_send = 100000;
            } else {
                tprintf("Was no match [%s]\r\n", buf + 4);
                st->handler = &httpd_send_buf;
                st->hdrbuf = (u8_t *) http_response_404;
                st->sendbuf = (u8_t *) page_404;
                st->data_to_send = sizeof(page_404) - 1;
            }
        }
        tcp_recved(tpcb, p->tot_len);
        pbuf_free(p);
        for (i = 0; i < 2048; i ++) buf[i] = 'a';
    } else if (st->state == ST_READY) {
        /* we are not really expecting another packet... */
        memcpy((char *) buf, (char *) p->payload, p->len);
        memcpy(buf, p->payload, p->len);
        buf[p->len - 1] = '\0';
        LWIP_DEBUGF(HTTPD_DEBUG, (
            "Another chunk arrived: [%s] (%d)\r\n", buf, p->len));
        tcp_recved(tpcb, p->tot_len);
        pbuf_free(p);
        ret_err = ERR_OK;
    } else if (st->state == ST_CLOSING) {
        /* odd case, remote side closing twice, trash data */
        tcp_recved(tpcb, p->tot_len);
        pbuf_free(p);
        ret_err = ERR_OK;
    } else {
        /* unkown st->state, trash data  */
        tcp_recved(tpcb, p->tot_len);
        pbuf_free(p);
        ret_err = ERR_OK;
    }
    return ret_err;
}

void httpd_error(void *arg, err_t err)
{
    struct httpd_state *st;

    LWIP_UNUSED_ARG(err);

    st = (struct httpd_state *) arg;
    if (st != NULL) {
        mem_free(st);
    }
}

err_t httpd_poll(void *arg, struct tcp_pcb *tpcb)
{
    struct httpd_state *st;

    st = (struct httpd_state *) arg;
    if (st == NULL) {
        /* nothing to be done */
        LWIP_DEBUGF(HTTPD_DEBUG, ("Aborting!\r\n"));
        tcp_abort(tpcb);
        return ERR_ABRT;
    }

    if (st->state == ST_CLOSING) {
        httpd_close(tpcb, st);
    }

    if (st->state == ST_READY) {
        st->state = ST_SENDING;
        tcp_sent(tpcb, httpd_sent);
        if (st->handler != NULL && (*st->handler)(tpcb, st)) {
            tcp_output(tpcb);
        }
    }

    return ERR_OK;
}

static err_t httpd_sent(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
    struct httpd_state *st;

    LWIP_UNUSED_ARG(len);
    LWIP_UNUSED_ARG(tpcb);

    st = (struct httpd_state *) arg;

    /* LWIP_DEBUGF(HTTPD_DEBUG | LWIP_DBG_TRACE, (
        "Data was sent! [%d]\r\n", len)); */
    if (st == NULL) {
        return ERR_OK;
    }
    st->state = ST_READY;
    /*
    if (st->handler != NULL) {
        (*st->handler)(tpcb, st);
    }
    */

    return ERR_OK;
}

u8_t httpd_send_test(struct tcp_pcb *tpcb, struct httpd_state *st)
{
    u8_t retval = 0;
    err_t wr_err = ERR_OK;
    u16_t len;

    /* LWIP_DEBUGF(HTTPD_DEBUG | LWIP_DBG_TRACE, (
        "Okay, let's send some body [%d].\r\n", st->bodycount)); */
    /* if (!tcp_nagle_disabled(tpcb)) {
        tprintf("nagle is not disabled!\r\n");
    } */

    if (st->bodycount == 0) {
        LWIP_DEBUGF(HTTPD_DEBUG, ("Sending headers..\r\n"));
        wr_err = tcp_write(tpcb,
            st->hdrbuf, strlen((char *) st->hdrbuf), 1);
        retval = 1;
    } else {
        len = tcp_sndbuf(tpcb) - 1;
        if (len > 2048) {
            len = 2048;
            //tprintf("len: [%d]\r\n", len);
        }
        if (len > st->data_to_send) {
            len = st->data_to_send;
        }
        /* LWIP_DEBUGF(HTTPD_DEBUG | LWIP_DBG_TRACE, ("sendbuf is [%d]\r\n", len)); */
        /* LWIP_DEBUGF(HTTPD_DEBUG | LWIP_DBG_TRACE, ("Sending body #%d\r\n", st->bodycount)); */
        wr_err = tcp_write(tpcb, buf, len, 0);
        if (wr_err == ERR_OK) {
            st->data_to_send -= len;
            /* LWIP_DEBUGF(HTTPD_DEBUG | LWIP_DBG_TRACE, (
                "tcp_write says ERR_OK\r\n")); */
        } else if (wr_err == ERR_MEM) {
            LWIP_DEBUGF(HTTPD_DEBUG, ("tcp_write says ERR_MEM\r\n"));
        } else {
            LWIP_DEBUGF(HTTPD_DEBUG, ("tcp_write says fail!!\r\n"));
        }
        //tcp_output(tpcb);
        retval = 1;
    }

    if (st->data_to_send == 0) {
        LWIP_DEBUGF(HTTPD_DEBUG, ("Closing connection...\r\n"));
        st->state = ST_CLOSING;
        httpd_close(tpcb, st);
        retval = 0;
    }
    st->bodycount ++;

    return retval;
}

u8_t httpd_send_grab(struct tcp_pcb *tpcb, struct httpd_state *st)
{
    u8_t retval = 0;
    err_t wr_err = ERR_OK;
    u16_t len;

    if (st->bodycount == 0) {
        LWIP_DEBUGF(HTTPD_DEBUG, ("Sending headers..\r\n"));
        wr_err = tcp_write(tpcb,
            st->hdrbuf, strlen((char *) st->hdrbuf), 1);
        retval = 1;
    } else {
        if (st->grabbufpos == DMABUF_SIZE) {
            //tprintf("Filling grabbuf...\r\n");
            st->grabbufpos = 0;
            cpld_xfr();
        }
        LWIP_ASSERT("st->grabbufpos > DMABUF_SIZE",
            st->grabbufpos <= DMABUF_SIZE);
        len = tcp_sndbuf(tpcb) - 1;
        if (len > st->data_to_send) {
            len = st->data_to_send;
        }
        if (len > (DMABUF_SIZE - st->grabbufpos)) {
            len = (DMABUF_SIZE - st->grabbufpos);
        }
        //tprintf("Sending %d bytes of data!\r\n", len);
        wr_err = tcp_write(tpcb, dmabuf + st->grabbufpos, len, 0);
        if (wr_err == ERR_OK) {
            st->data_to_send -= len;
            st->grabbufpos += len;
        } else if (wr_err == ERR_MEM) {
            LWIP_DEBUGF(HTTPD_DEBUG, ("tcp_write says ERR_MEM\r\n"));
        } else {
            LWIP_DEBUGF(HTTPD_DEBUG, ("tcp_write says fail!!\r\n"));
        }
        retval = 1;
    }

    if (st->data_to_send == 0) {
        LWIP_DEBUGF(HTTPD_DEBUG, ("Closing connection...\r\n"));
        st->state = ST_CLOSING;
        httpd_close(tpcb, st);
        retval = 0;
    }
    st->bodycount ++;

    return retval;
}

u8_t httpd_send_buf(struct tcp_pcb *tpcb, struct httpd_state *st)
{
    u8_t retval = 0;
    err_t wr_err = ERR_OK;
    u16_t len;

    if (st->bodycount == 0) {
        /* LWIP_DEBUGF(HTTPD_DEBUG, (
            "Sending headers.. (%d bytes)\r\n", strlen(st->hdrbuf))); */
        wr_err = tcp_write(tpcb,
            st->hdrbuf, strlen((char *) st->hdrbuf), 1);
        retval = 1;
    } else {
        len = tcp_sndbuf(tpcb) - 1; // we can send this much data
        if (len > 2048) {
            len = 2048;
            LWIP_DEBUGF(HTTPD_DEBUG, ("suspiciously big len: [%d]\r\n", len));
        }
        if (len > st->data_to_send) {
            len = st->data_to_send;
        }
        wr_err = tcp_write(tpcb, st->sendbuf, len, 1);
        if (wr_err == ERR_OK) {
            st->data_to_send -= len;
            st->sendbuf += len;
        } else if (wr_err == ERR_MEM) {
            LWIP_DEBUGF(HTTPD_DEBUG, ("tcp_write says ERR_MEM\r\n"));
        } else {
            LWIP_DEBUGF(HTTPD_DEBUG, ("tcp_write says fail!!\r\n"));
        }
        retval = 1;
    }

    if (st->data_to_send == 0) {
        LWIP_DEBUGF(HTTPD_DEBUG, ("Closing connection...\r\n"));
        st->state = ST_CLOSING;
        httpd_close(tpcb, st);
        //tcp_output(tpcb);
        retval = 0;
    }
    st->bodycount ++;

    //tprintf("Returing with [%d]\r\n", retval);
    return retval;
}

void httpd_close(struct tcp_pcb *tpcb, struct httpd_state *st)
{
    err_t err;

    tcp_arg(tpcb, NULL);
    tcp_sent(tpcb, NULL);
    tcp_recv(tpcb, NULL);
    tcp_err(tpcb, NULL);
    tcp_poll(tpcb, NULL, 0);
  
    if (st != NULL) {
        mem_free(st);
    }  
    err = tcp_close(tpcb);
    if (err != ERR_OK) {
        LWIP_DEBUGF(HTTPD_DEBUG, ("Error %d closing %p\n", err, (void*)tpcb));
    }
}

/* vim: set sw=4 et: */
