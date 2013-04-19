/* Copyright (C) 2013 Upi Tamminen
 * See the COPYRIGHT file for more information */

#include "inc/LPC17xx.h"
#include "tvp7002.h"
#include "i2c.h"
#include "tprintf.h"
#include "tvp7002_reg.h"

uint32_t tvp7002_set(uint8_t addr, uint8_t val)
{
    i2c_clearbuffers();

    I2CWriteLength = 3;
    I2CReadLength = 0;
    I2CMasterBuffer[0] = TVP7002_ADDR;          /* EEPROM address */
    I2CMasterBuffer[1] = addr;                  /* key */
    I2CMasterBuffer[2] = val;                   /* value */

    return I2CEngine();
}

uint8_t tvp7002_get(uint8_t addr)
{
    i2c_clearbuffers();

    I2CWriteLength = 2;
    I2CReadLength = 1;
    I2CMasterBuffer[0] = TVP7002_ADDR;  /* EEPROM address */
    I2CMasterBuffer[1] = (addr);        /* key */
    I2CMasterBuffer[2] = TVP7002_ADDR | RD_BIT;

    while (I2CEngine() == I2CSTATE_SLA_NACK);

    return I2CSlaveBuffer[0];
}

void tvp7002_info(void)
{
    uint16_t cpln, lnpf, val;

    //tprintf("Reading from TVP7002...\r\n");
    val = tvp7002_get(0x00);
    if (val != 0x02) {
        tprintf("The data was not a proper revision number!\r\n");
        return;
    }

    //tprintf("Input Mux Select 1: [0x%x]\r\n", tvp7002_get(0x19));
    //tprintf("Input Mux Select 2: [0x%x]\r\n", tvp7002_get(0x1a));
    //tprintf("Lines per frame: [0x%x][0x%x]\r\n",
    //    tvp7002_get(0x37), tvp7002_get(0x38));
    lnpf = ((tvp7002_get(0x38) & 0xf) << 8) | tvp7002_get(0x37);
    cpln = ((tvp7002_get(0x3a) & 0xf) << 8) | tvp7002_get(0x39);
    tprintf("Lines per frame: %d\r\n", lnpf);
    tprintf("Clocks per line: %d\r\n", cpln);
    tprintf("Frame rate: ~%d\r\n", (6553600L / cpln / lnpf));

    tprintf("Sync Control 1: [0x%x]\r\n", tvp7002_get(0x0e));
    val = tvp7002_get(0x14);
    if (tvp7002_get(0x14) != 0x95) {
        tprintf("HSYNC detection status: [0x%x]\r\n", tvp7002_get(0x14));
        tprintf(" HSYNC activity detected: %s\r\n",
                (val & (1 << 7)) ? "yes" : "no");
        tprintf(" Active HSYNC: %s\r\n",
                (val & (1 << 6)) ? "selected SOG input" : "selected HSYNC input");
        tprintf(" Input HSYNC Polarity Detect: %s\r\n",
                (val & (1 << 5)) ? "Active-high" : "Active-low");
        tprintf(" VSYNC activity detected: %s\r\n",
                (val & (1 << 4)) ? "yes" : "no");
        tprintf(" Input VSYNC Polarity Detect: %s\r\n",
                (val & (1 << 3)) ? "Active-high" : "Active-low");
        tprintf(" SOG activity detected: %s\r\n",
                (val & (1 << 2)) ? "yes" : "no");
        tprintf(" Input Coast Polarity: %s\r\n",
                (val & (1 << 1)) ? "Active-high" : "Active-low");
    }
}


void tvp7002_init(void)
{
    /* some i2c settings according to
     * TVP7002_THS8200_RGB_graphics_settings_081811.xls */

    /* unique */
    tvp7002_set(TVP7002_HPLL_FDBK_DIV_MSBS, 0x32);
    tvp7002_set(TVP7002_HPLL_FDBK_DIV_LSBS, 0x00);
    tvp7002_set(TVP7002_HPLL_CRTL, 0x20);
    tvp7002_set(TVP7002_HSYNC_OUT_W, 0x60);
    tvp7002_set(TVP7002_SYNC_CTL_1, 0x00);
    tvp7002_set(TVP7002_ADC_SETUP, 0x50);
    tvp7002_set(TVP7002_AVID_START_PIXEL_LSBS, 0x92);
    tvp7002_set(TVP7002_AVID_START_PIXEL_MSBS, 0x00);
    tvp7002_set(TVP7002_AVID_STOP_PIXEL_LSBS, 0x12);
    tvp7002_set(TVP7002_AVID_STOP_PIXEL_MSBS, 0x03);
    tvp7002_set(TVP7002_VBLK_F_0_START_L_OFF, 0x12);
    tvp7002_set(TVP7002_VBLK_F_1_START_L_OFF, 0x12);
    tvp7002_set(TVP7002_VBLK_F_0_DURATION, 0x2d);
    tvp7002_set(TVP7002_VBLK_F_1_DURATION, 0x2d);

    /* common */
    tvp7002_set(0x04, 0x80);
    tvp7002_set(0x05, 0x06);
    tvp7002_set(0x06, 0x10);
    tvp7002_set(0x08, 0x3c);
    tvp7002_set(0x09, 0x3c);
    tvp7002_set(0x0a, 0x3c);
    tvp7002_set(0x0f, 0x2e);
    tvp7002_set(0x10, 0x58);
    tvp7002_set(0x11, 0x40);
    tvp7002_set(0x12, 0x01);
    tvp7002_set(0x13, 0x00);
    tvp7002_set(0x15, 0x04);
    tvp7002_set(0x17, 0x00);
    tvp7002_set(0x18, 0x01);
    tvp7002_set(0x19, 0xaa);
    tvp7002_set(0x1a, 0xc2);
    tvp7002_set(0x21, 0x0d);
    tvp7002_set(0x22, 0x00);
    tvp7002_set(0x26, 0x80); // 0x80 = enable, 0x00 = disable
    tvp7002_set(0x28, 0x53);
    tvp7002_set(0x2a, 0x07);
    tvp7002_set(0x2b, 0x00); // Power Control
    tvp7002_set(0x31, 0x18);
    tvp7002_set(0x35, 0x00);
    tvp7002_set(0x36, 0x00);
    tvp7002_set(0x3d, 0x06);

    /* 640x480 59.94hz recommended settings from datasheet */
    tvp7002_set(TVP7002_HPLL_FDBK_DIV_MSBS,     0x32);
    tvp7002_set(TVP7002_HPLL_FDBK_DIV_LSBS,     0x00);
    tvp7002_set(TVP7002_HPLL_CRTL,              0x20);
    tvp7002_set(TVP7002_HPLL_PHASE_SEL,         0x0);

    /* my additions */

    /* disable hsync/sog detect */
    tvp7002_set(TVP7002_SYNC_CTL_1,             0x12);
    tvp7002_set(0x36, 0x00);

    tvp7002_set(TVP7002_IN_MUX_SEL_1,           0x00);
    tvp7002_set(TVP7002_IN_MUX_SEL_2,           0xc2);

    /* RGB 4:4:4 out */
    tvp7002_set(TVP7002_OUT_FORMATTER,          0x04);
    // 0x47 = 01000111

    /* refclk at fdout */
    tvp7002_set(TVP7002_MISC_CTL_2,             0x42);

    /* enable automatic power management mode */
    tvp7002_set(TVP7002_HPLL_AND_CLAMP_CTL,     0x2a);

    /* disable blue & green for fun */
    //tvp7002_set(TVP7002_PWR_CTL,                0x00);
    // turn off colors
    //tvp7002_set(TVP7002_PWR_CTL,                0x67);
    tvp7002_set(TVP7002_PWR_CTL,                0x60);
}

#if 0
/* init according to tvp7002.c in linux */
void tvp7002_init(void)
{
    /* default values */
    /*
    tvp7002_set(TVP7002_HPLL_FDBK_DIV_MSBS,     0x67);
    tvp7002_set(TVP7002_HPLL_FDBK_DIV_LSBS,     0x20);
    tvp7002_set(TVP7002_HPLL_CRTL,              0xa0);
    tvp7002_set(TVP7002_HPLL_PHASE_SEL,         0x80);
    tvp7002_set(TVP7002_CLAMP_START,            0x32);
    tvp7002_set(TVP7002_CLAMP_W,                0x20);
    tvp7002_set(TVP7002_HSYNC_OUT_W,            0x60);
    tvp7002_set(TVP7002_B_FINE_GAIN,            0x00);
    tvp7002_set(TVP7002_G_FINE_GAIN,            0x00);
    tvp7002_set(TVP7002_R_FINE_GAIN,            0x00);
    tvp7002_set(TVP7002_B_FINE_OFF_MSBS,        0x80);
    tvp7002_set(TVP7002_G_FINE_OFF_MSBS,        0x80);
    tvp7002_set(TVP7002_R_FINE_OFF_MSBS,        0x80);
    tvp7002_set(TVP7002_SYNC_CTL_1,             0x20);
    tvp7002_set(TVP7002_HPLL_AND_CLAMP_CTL,     0x2e);
    tvp7002_set(TVP7002_SYNC_ON_G_THRS,         0x5d);
    tvp7002_set(TVP7002_SYNC_SEPARATOR_THRS,    0x47);
    tvp7002_set(TVP7002_HPLL_PRE_COAST,         0x00);
    tvp7002_set(TVP7002_HPLL_POST_COAST,        0x00);
    tvp7002_set(TVP7002_OUT_FORMATTER,          0x47);
    tvp7002_set(TVP7002_MISC_CTL_1,             0x01);
    tvp7002_set(TVP7002_MISC_CTL_2,             0x00);
    tvp7002_set(TVP7002_MISC_CTL_3,             0x01);
    tvp7002_set(TVP7002_IN_MUX_SEL_1,           0x00);
    tvp7002_set(TVP7002_IN_MUX_SEL_2,           0x67);
    tvp7002_set(TVP7002_B_AND_G_COARSE_GAIN,    0x77);
    tvp7002_set(TVP7002_R_COARSE_GAIN,          0x07);
    tvp7002_set(TVP7002_FINE_OFF_LSBS,          0x00);
    tvp7002_set(TVP7002_B_COARSE_OFF,           0x10);
    tvp7002_set(TVP7002_G_COARSE_OFF,           0x10);
    tvp7002_set(TVP7002_R_COARSE_OFF,           0x10);
    tvp7002_set(TVP7002_HSOUT_OUT_START,        0x08);
    tvp7002_set(TVP7002_MISC_CTL_4,             0x00);
    tvp7002_set(TVP7002_AUTO_LVL_CTL_ENABLE,    0x80);
    tvp7002_set(TVP7002_AUTO_LVL_CTL_FILTER,    0x53);
    tvp7002_set(0x29,                           0x08);
    tvp7002_set(TVP7002_FINE_CLAMP_CTL,         0x07);
    tvp7002_set(TVP7002_PWR_CTL,                0x00);
    tvp7002_set(TVP7002_ADC_SETUP,              0x50);
    tvp7002_set(TVP7002_COARSE_CLAMP_CTL,       0x00);
    tvp7002_set(TVP7002_SOG_CLAMP,              0x80);
    tvp7002_set(TVP7002_RGB_COARSE_CLAMP_CTL,   0x00);
    tvp7002_set(TVP7002_SOG_COARSE_CLAMP_CTL,   0x04);
    tvp7002_set(TVP7002_ALC_PLACEMENT,          0x5a);
    tvp7002_set(0x32,                           0x18);
    tvp7002_set(0x33,                           0x60);
    tvp7002_set(TVP7002_MVIS_STRIPPER_W,        0xff);
    tvp7002_set(TVP7002_VSYNC_ALGN,             0x10);
    tvp7002_set(TVP7002_SYNC_BYPASS,            0x00);
    tvp7002_set(TVP7002_L_LENGTH_TOL,           0x03);
    tvp7002_set(0x3e,                           0x60);
    tvp7002_set(TVP7002_VIDEO_BWTH_CTL,         0x01);
    tvp7002_set(TVP7002_AVID_START_PIXEL_LSBS,  0x01);
    tvp7002_set(TVP7002_AVID_START_PIXEL_MSBS,  0x2c);
    tvp7002_set(TVP7002_AVID_STOP_PIXEL_LSBS,   0x06);
    tvp7002_set(TVP7002_AVID_STOP_PIXEL_MSBS,   0x2c);
    tvp7002_set(TVP7002_VBLK_F_0_START_L_OFF,   0x05);
    tvp7002_set(TVP7002_VBLK_F_1_START_L_OFF,   0x00);
    tvp7002_set(TVP7002_VBLK_F_0_DURATION,      0x1e);
    tvp7002_set(TVP7002_VBLK_F_1_DURATION,      0x00);
    tvp7002_set(TVP7002_FBIT_F_0_START_L_OFF,   0x00);
    tvp7002_set(TVP7002_FBIT_F_1_START_L_OFF,   0x00);
    tvp7002_set(TVP7002_YUV_Y_G_COEF_LSBS,      0xe3);
    tvp7002_set(TVP7002_YUV_Y_G_COEF_MSBS,      0x16);
    tvp7002_set(TVP7002_YUV_Y_B_COEF_LSBS,      0x4f);
    tvp7002_set(TVP7002_YUV_Y_B_COEF_MSBS,      0x02);
    tvp7002_set(TVP7002_YUV_Y_R_COEF_LSBS,      0xce);
    tvp7002_set(TVP7002_YUV_Y_R_COEF_MSBS,      0x06);
    tvp7002_set(TVP7002_YUV_U_G_COEF_LSBS,      0xab);
    tvp7002_set(TVP7002_YUV_U_G_COEF_MSBS,      0xf3);
    tvp7002_set(TVP7002_YUV_U_B_COEF_LSBS,      0x00);
    tvp7002_set(TVP7002_YUV_U_B_COEF_MSBS,      0x10);
    tvp7002_set(TVP7002_YUV_U_R_COEF_LSBS,      0x55);
    tvp7002_set(TVP7002_YUV_U_R_COEF_MSBS,      0xfc);
    tvp7002_set(TVP7002_YUV_V_G_COEF_LSBS,      0x78);
    tvp7002_set(TVP7002_YUV_V_G_COEF_MSBS,      0xf1);
    tvp7002_set(TVP7002_YUV_V_B_COEF_LSBS,      0x88);
    tvp7002_set(TVP7002_YUV_V_B_COEF_MSBS,      0xfe);
    tvp7002_set(TVP7002_YUV_V_R_COEF_LSBS,      0x00);
    tvp7002_set(TVP7002_YUV_V_R_COEF_MSBS,      0x10);
    */

    /* 640x480 59.94hz recommended settings from datasheet */
    tvp7002_set(TVP7002_HPLL_FDBK_DIV_MSBS,     0x32);
    tvp7002_set(TVP7002_HPLL_FDBK_DIV_LSBS,     0x00);
    tvp7002_set(TVP7002_HPLL_CRTL,              0x20);
    tvp7002_set(TVP7002_HPLL_PHASE_SEL,         0x0);

    /* some other stuff, try without these? */
    /*
    tvp7002_set(TVP7002_AVID_START_PIXEL_LSBS,  0x91);
    tvp7002_set(TVP7002_AVID_START_PIXEL_MSBS,  0x00);
    tvp7002_set(TVP7002_AVID_STOP_PIXEL_LSBS,   0x0B);
    tvp7002_set(TVP7002_AVID_STOP_PIXEL_MSBS,   0x00);
    tvp7002_set(TVP7002_VBLK_F_0_START_L_OFF,   0x03);
    tvp7002_set(TVP7002_VBLK_F_1_START_L_OFF,   0x01);
    tvp7002_set(TVP7002_VBLK_F_0_DURATION,      0x13);
    tvp7002_set(TVP7002_VBLK_F_1_DURATION,      0x13);
    */

    /* additions */

    /* disable hsync/sog detect */
    tvp7002_set(TVP7002_SYNC_CTL_1,             0x12);
    tvp7002_set(0x36, 0x00);

    tvp7002_set(TVP7002_IN_MUX_SEL_1,           0x00);
    tvp7002_set(TVP7002_IN_MUX_SEL_2,           0xc2);

    /* RGB 4:4:4 out */
    tvp7002_set(TVP7002_OUT_FORMATTER,          0x04);
    // 0x47 = 01000111

    /* refclk at fdout */
    tvp7002_set(TVP7002_MISC_CTL_2,             0x42);

    /* enable automatic power management mode */
    tvp7002_set(TVP7002_HPLL_AND_CLAMP_CTL,     0x2a);

    /* disable blue & green for fun */
    //tvp7002_set(TVP7002_PWR_CTL,                0x06);
    tvp7002_set(TVP7002_PWR_CTL,                0x60);

}
#endif

/* vim: set et sw=4: */
