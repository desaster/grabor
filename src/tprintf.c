/*
 * Tiny printf by oPossum
 * http://www.43oh.com/forum/viewtopic.php?f=10&t=1732
 * + Small changes by me
 */

#include <stdarg.h>

#include "uart0.h"

static const unsigned long dv[] = {
//  4294967296      // 32 bit unsigned max
    1000000000,     // +0
     100000000,     // +1
      10000000,     // +2
       1000000,     // +3
        100000,     // +4
//       65535      // 16 bit unsigned max     
         10000,     // +5
          1000,     // +6
           100,     // +7
            10,     // +8
             1,     // +9
};

static void xtoa(unsigned long x, const unsigned long *dp)
{
    char c;
    unsigned long d;
    if(x) {
        while(x < *dp) ++dp;
        do {
            d = *dp++;
            c = '0';
            while(x >= d) ++c, x -= d;
            UART0_Sendchar(c);
        } while(!(d & 1));
    } else
        UART0_Sendchar('0');
}

static void puth(unsigned n)
{
    static const char hex[16] = { '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
    UART0_Sendchar(hex[n & 15]);
}
 
void tprintf(char *format, ...)
{
    char c;
    int i;
    long n;
    
    va_list a;
    va_start(a, format);
    while((c = *format++)) {
        if(c == '%') {
            switch(c = *format++) {
                case 's':                       // String
                    UART0_PrintString(va_arg(a, char*));
                    break;
                case 'c':                       // Char
                    UART0_Sendchar((char) va_arg(a, int));
                    break;
                case 'i':                       // 16 bit Integer
                case 'u':                       // 16 bit Unsigned
                    i = va_arg(a, int);
                    if(c == 'i' && i < 0) i = -i, UART0_Sendchar('-');
                    xtoa((unsigned)i, dv + 5);
                    break;
                case 'l':                       // 32 bit Long
                case 'n':                       // 32 bit uNsigned loNg
                    n = va_arg(a, long);
                    if(c == 'l' &&  n < 0) n = -n, UART0_Sendchar('-');
                    xtoa((unsigned long)n, dv);
                    break;
                case 'x':                       // 16 bit heXadecimal
                    i = va_arg(a, int);
                    puth(i >> 12);
                    puth(i >> 8);
                    puth(i >> 4);
                    puth(i);
                    break;
                case 0: return;
                default: goto bad_fmt;
            }
        } else
bad_fmt:    UART0_Sendchar(c);
    }
    va_end(a);
}

/* vim: set sw=4 et: */
